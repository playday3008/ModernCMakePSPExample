#include <charconv>
#include <iomanip>
#include <mutex>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include <cstddef>
#include <cstdint>

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>

#include <modern/pspdebugio.hpp>

psp_streambuf* psp_streambuf::_instance = nullptr;
std::mutex     psp_streambuf::_mutex;

psp_streambuf* psp_streambuf::get_instance() {
    std::lock_guard lock(_mutex);
    if (!_instance) {
        _instance = new psp_streambuf();
    }
    return _instance;
}

int psp_streambuf::sync() {
    int ret = std::stringbuf::sync();
    if (ret == -1) return ret;

    // Handle PSP Debug Screen

    // Make a copy of the string to avoid modifying the buffer
    const std::string input = str();

    size_t i                = 0;
    size_t lastPrintedIndex = 0;

    // Used for swapping text and background colors
    void (*setTextColor)(u32) = pspDebugScreenSetTextColor;
    void (*setBackColor)(u32) = pspDebugScreenSetBackColor;
    while (i < input.length()) {
        // Handle escape sequences
        if (input[i] == '\033' && i + 1 < input.length() && input[i + 1] == '[') {
            // Print any pending text before the escape sequence
            if (i > lastPrintedIndex) {
                pspDebugScreenPrintData(input.data() + lastPrintedIndex, i - lastPrintedIndex);
            }

            // Find the end of the escape sequence
            size_t end = input.find('m', i + 2);
            if (end != std::string_view::npos) {
                // Extract the escape sequence
                std::string_view escapeSequence = input.substr(i + 2, end - (i + 2));
                if (escapeSequence.empty()) {
                    // Empty escape sequence, treat it as "0"
                    escapeSequence = "0";
                }

                // Parse the escape sequence
                const auto seq =
                    escapeSequence | std::ranges::views::split(';') |
                    std::ranges::views::transform([](const auto&& range) {
                        int ret{};
                        if (std::from_chars(range.begin(), range.end(), ret).ec == std::errc{})
                            return ret;
                        return -1;
                    }) |
                    std::ranges::to<std::vector<int>>();

                // Walk through the escape sequence
                for (size_t j = 0; j < seq.size(); j++) {
                    const int prevCode = j > 0 ? seq[j - 1] : -1;
                    const int code     = seq[j];
                    switch (code) {
                        case -1:
                            // Some error occurred while parsing the escape sequence
                            return code;
                        case 0:
                            setTextColor(DEFAULT_TEXT_COLOR);
                            setBackColor(DEFAULT_BACK_COLOR);
                            [[fallthrough]];
                        case 27:  // NOT Inverted
                            setTextColor = pspDebugScreenSetTextColor;
                            setBackColor = pspDebugScreenSetBackColor;
                            break;

                        case 7:  // Inverted
                            setTextColor = pspDebugScreenSetBackColor;
                            setBackColor = pspDebugScreenSetTextColor;
                            break;

                        // Rich color handling (when previous code was 38 or 48)
                        case 2:  // RGB (24-bit) ^]38;2;R;G;B^
                            if (j + 3 < seq.size()) {
                                const u32 r         = seq[j + 1];
                                const u32 g         = seq[j + 2];
                                const u32 b         = seq[j + 3];
                                const u32 richColor = 0xff000000 | (b << 16) | (g << 8) | r;
                                if (prevCode == 38) {
                                    setTextColor(richColor);
                                } else if (prevCode == 48) {
                                    setBackColor(richColor);
                                }
                                j += 3;
                            }
                            break;
                        case 5:  // 8-bit color ^]38;5;N^
                            if (j + 1 < seq.size()) {
                                const u32 color = seq[j + 1];
                                if (prevCode == 38) {
                                    setTextColor(COLORS_8BIT[color]);
                                } else if (prevCode == 48) {
                                    setBackColor(COLORS_8BIT[color]);
                                }
                                j++;
                            }
                            break;

                        // Foreground colors
                        case 30 ... 37:  // Predefined colors
                            setTextColor(COLORS_4BIT[code - 30]);
                            break;
                        case 38:  // Custom color
                            // Do nothing, will be handled on next iteration
                            break;
                        case 39:  // Default foreground color
                            setTextColor(DEFAULT_TEXT_COLOR);
                            break;

                        // Background colors
                        case 40 ... 47:  // Predefined colors
                            setBackColor(COLORS_4BIT[code - 40]);
                            break;
                        case 48:  // Custom color
                            // Do nothing, will be handled on next iteration
                            break;
                        case 49:  // Default background color
                            setBackColor(DEFAULT_BACK_COLOR);
                            break;

                        // High-intensity colors
                        case 90 ... 97:  // Foreground
                            setTextColor(COLORS_4BIT[code - 90 + 8]);
                            break;
                        case 100 ... 107:  // Background
                            setBackColor(COLORS_4BIT[code - 100 + 8]);
                            break;

                        default:
                            /**
                             * Bold, faint, italic, underline, blink, etc. are not supported
                             * and probably won't be
                             */
                            break;
                    }
                }

                i                = end + 1;
                lastPrintedIndex = i;
            } else {
                // Invalid escape sequence, print it as-is
                pspDebugScreenPrintData(input.data() + i, 2);
                i               += 2;
                lastPrintedIndex = i;
            }
        } else {
            i++;
        }
    }

    // Print any remaining text after the last escape sequence
    if (lastPrintedIndex < input.length()) {
        pspDebugScreenPrintData(input.data() + lastPrintedIndex, input.length() - lastPrintedIndex);
    }
    sceDisplayWaitVblankStart();
    pspDebugScreenSetXY(0, 0);

    return ret;
}

void test_table(const char* title, const char* mode) {
    static std::ostream cout(psp_streambuf::get_instance());

    int front, back;
    cout << "\n\033[1m" << title << "\033[m\n bg  fg\n";
    for (back = 40; back <= 107; back++) {
        if (back == 48) back = 100;
        cout << std::setw(3) << back << " \033[" << mode << back << "m";
        for (front = 30; front <= 97; front++) {
            if (front == 38) front = 90;
            cout << "\033[" << front << "m " << front;
        }
        cout << "\033[m\n";
    }
}

void test_table_rich(const char* title, const char* mode) {
    static std::ostream cout(psp_streambuf::get_instance());

    int back;
    cout << "\n\033[1m" << title << "\033[m\n";

    for (back = 0; back < 16; back++) {
        cout << "\033[" << mode << "48;5;" << back << "m" << std::setw(4) << back << "\033[m";
        if (back % 8 == 7) cout << "\n";
    }

    for (back = 16; back < 232; back++) {
        cout << "\033[" << mode << "48;5;" << back << "m" << std::setw(4) << back << "\033[m";
        if ((back - 15) % 12 == 0) cout << "\n";
    }

    for (back = 232; back < 256; back++) {
        cout << "\033[" << mode << "48;5;" << back << "m" << std::setw(4) << back << "\033[m";
        if ((back - 231) % 6 == 0) cout << "\n";
    }
}
