#pragma once

#include <sstream>

/**
 * @brief String buffer which writes both to stdout and the PSP Debug Screen
 *
 * @note PSP Debug Screen supports a subset of ANSI escape codes, including:
 *
 * - Reset (0)
 *
 * - Inverted (7)
 *
 * - NOT Inverted (27)
 *
 * - Foreground colors (30-37, 90-97)
 *
 * - 8-bit Foreground color (38;5;N)
 *
 * - 24-bit Foreground color (38;2;R;G;B)
 *
 * - Background colors (40-47, 100-107)
 *
 * - 8-bit Background color (48;5;N)
 *
 * - 24-bit Background color (48;2;R;G;B)
 */
class psp_streambuf : public std::stringbuf {
  protected:
    static psp_streambuf* _instance;

    psp_streambuf() : std::stringbuf() {
        pspDebugScreenInit();
        pspDebugScreenSetXY(0, 0);
    }

  private:
    static constexpr uint32_t DEFAULT_TEXT_COLOR = 0xFFFFFFFF;
    static constexpr uint32_t DEFAULT_BACK_COLOR = 0x00000000;

    /**
     * @brief ABGR color values for the 4-bit color palette
     *
     * @note Alpha is unused and set to 0xFF
     */
    static constexpr std::array<uint32_t, 1 << 4> COLORS_4BIT = {
      0xff000000,  // Black
      0xff000080,  // Red
      0xff008000,  // Green
      0xff008080,  // Yellow
      0xff800000,  // Blue
      0xff800080,  // Magenta
      0xff808000,  // Cyan
      0xffc0c0c0,  // White
      0xff808080,  // Bright Black
      0xff0000ff,  // Bright Red
      0xff00ff00,  // Bright Green
      0xff00ffff,  // Bright Yellow
      0xffff0000,  // Bright Blue
      0xffff00ff,  // Bright Magenta
      0xffffff00,  // Bright Cyan
      0xffffffff   // Bright White
    };

    /**
     * @brief ABGR color values for the 8-bit color palette
     *
     * @note Alpha is unused and set to 0xFF
     */
    static constexpr std::array<uint32_t, 1 << 8> COLORS_8BIT = []() consteval -> auto {
        std::array<uint32_t, 1 << 8> colors;
        // First 16 colors are the same as 4-bit colors
        for (uint32_t i = 0; i < 16; i++) {
            colors[i] = COLORS_4BIT[i];
        }

        // Next 216 colors are a 6x6x6 color cube
        constexpr uint32_t first_step       = 0x5f;
        constexpr uint32_t consequent_steps = 0x28;
        constexpr auto     x_part           = [](uint32_t x) consteval -> uint32_t {
            return x ? (x - 1) * consequent_steps + first_step : x;
        };
        for (uint32_t r = 0; r < 6; r++) {
            const uint32_t r_part = x_part(r);
            for (size_t g = 0; g < 6; g++) {
                const uint32_t g_part = x_part(g);
                for (size_t b = 0; b < 6; b++) {
                    const uint32_t b_part = x_part(b);

                    const size_t index = 16 + b + g * 6 + r * 36;
                    colors[index]      = 0xff000000 | b_part << 16 | g_part << 8 | r_part;
                }
            }
        }

        // Last 24 colors are a grayscale ramp
        for (uint32_t i = 0; i < 24; i++) {
            const size_t  index = 232 + i;
            const uint8_t gray  = 8 + i * 10;
            colors[index]       = 0xff000000 | gray << 16 | gray << 8 | gray;
        }
        return colors;
    }();

  public:
    psp_streambuf(const psp_streambuf&)                = delete;
    psp_streambuf& operator=(const psp_streambuf&)     = delete;
    psp_streambuf(psp_streambuf&&) noexcept            = delete;
    psp_streambuf& operator=(psp_streambuf&&) noexcept = delete;
    ~psp_streambuf() override { pspDebugScreenClear(); }

    static psp_streambuf* get_instance() {
        if (!_instance) {
            _instance = new psp_streambuf();
        }
        return _instance;
    }

    int sync() override;
};

void test_table(const char* title, const char* mode);
void test_table_rich(const char* title, const char* mode);
