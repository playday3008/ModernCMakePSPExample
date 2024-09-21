#include <modern/lib.hpp>

#include <sstream>

#include <pspkernel.h>
#include <pspdebug.h>

#ifdef CATCH_CONFIG_NOSTDOUT

    #include <modern/pspdebugio.hpp>

namespace Catch {
    std::ostream& cout() {
        static std::ostream ret(psp_streambuf::get_instance());
        return ret;
    }
    std::ostream& clog() {
        static std::ostream ret(psp_streambuf::get_instance());
        return ret;
    }
    std::ostream& cerr() { return clog(); }
}  // namespace Catch

#endif  // CATCH_CONFIG_NOSTDOUT

#include <catch2/catch_session.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_config_wchar.hpp>
#include <catch2/internal/catch_leak_detector.hpp>
#include <catch2/internal/catch_platform.hpp>

namespace Catch {
    CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
    CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
    static LeakDetector leakDetector;
    CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
}  // namespace Catch

PSP_MODULE_INFO("testlib", PSP_MODULE_USER, 1, 0);

PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int exit_callback([[maybe_unused]] int   arg1,
                  [[maybe_unused]] int   arg2,
                  [[maybe_unused]] void* common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread([[maybe_unused]] SceSize args, [[maybe_unused]] void* argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, nullptr);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, nullptr);
    if (thid >= 0) sceKernelStartThread(thid, 0, nullptr);
    return thid;
}

int main(int argc, char** argv) {
    // Use above functions to make exiting possible
    setup_callbacks();

    // Initialize the debug screen
    // pspDebugScreenInit(); // This is done in the psp_streambuf constructor

    // We want to force the linker not to discard the global variable
    // and its constructor, as it (optionally) registers leak detector
    (void)&Catch::leakDetector;

    // test_table("4-bit color", "0;");
    // test_table("4-bit color (inverted), "7;");
    // Catch::cout() << "\033[7;1;2;3;4;6;9;38;2;123;0;123myo\033[m\n";
    // test_table_rich("8-bit color", "0;");

    int ret = Catch::Session().run(argc, argv);

    while (1);

    return ret;
}

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Quick Check", "[main]") {
    std::vector<double> values{1, 2., 3.};
    auto [mean, moment] = accumulate_vector(values);

    REQUIRE(mean == 2.0);
    REQUIRE(moment == Catch::Approx(4.666666));
}
