# ModernCMakePSPExample

But this time for PSP and automated with VSCode.

## TODO

- [ ] Automate tests execution directly on PSP.
- [ ] Shift running and debugging from VSCode to CMake.
- [ ] Use PSPSDK libraries for more relatable examples.

## Build and debug on PSP

> Psst! Check [dependencies](#references) first.

### Using VSCode

1. Open project in VSCode.
    - Select `PSP` CMake toolchain if prompted.
2. Configure:
    - CMake Tools should autoconfigure the project
        - If not, then open CMake tab and on `Project Status` click on `Delete Cache and Reconfigure` (this is an icon).
3. Select target:
    - In CMake tab
    - In `Project Outline` select desired target (must be `Executable`)
    - Right click and select `Set as Launch/Debug Target`.
4. Debug:
    - Same as last step above but now click `Debug` to build and debug the project on PSP.
        1. [PSPLink](https://github.com/pspdev/psplinkusb) must be installed and running on the PSP.
        2. PSP must be connected to the computer via USB.

### Using terminal

More info on [PSPDEV Docs](https://pspdev.github.io/debugging.html).

1. Configure project using:

    ```bash
    psp-cmake -S . -B build
    ```

2. Build project using:

    ```bash
    cmake --build build
    ```

3. Initialize connection to PSP using:

    ```bash
    usbhostfs_pc
    ```

    1. [PSPLink](https://github.com/pspdev/psplinkusb) must be installed and running on the PSP.
    2. PSP must be connected to the computer via USB.

4. Start debug server using:

    ```bash
    pspsh -e debug ./build/apps/app.prx
    ```

    1. This is an example, the actual command may vary depending on the project.

5. Start debug client using:

    ```bash
    psp-gdb ./build/apps/app -q
    ```

    then inside gdb:

    ```bash
    target remote localhost:10001
    ```

    1. This is an example, the actual command may vary depending on the project.

## References

- Based on:
  - [Modern CMake](https://cliutils.gitlab.io/modern-cmake/README.html)
  - [PSP CMake Template](https://github.com/max-ishere/psp-cmake)
- Knowledge:
  - [PSPDEV Docs](https://pspdev.github.io/debugging.html)
    - [PSP Packages](https://github.com/pspdev/psp-packages)
  - [VitaSDK Samples](https://github.com/vitasdk/samples)
  - [VSCode C++ Tools (Issue 3998)](https://github.com/microsoft/vscode-cpptools/issues/3998)
  - [Hello World PSP (Using VSCode to debug)](https://github.com/fjtrujy/helloWorldPSP)
- Direct dependencies:
  - [PSPDEV](https://github.com/pspdev)
    - [PSPSDK](https://github.com/pspdev/pspsdk)
    - [PSP Toolchain](https://github.com/pspdev/psptoolchain)
    - [PSP Toolchain (Extra)](https://github.com/pspdev/psptoolchain-extra)
  - [Catch2](https://github.com/catchorg/Catch2)
  - [ArduinoJson](https://arduinojson.org/)
- Indirect dependencies:
  - [CMake](https://cmake.org/)
  - [VSCode](https://code.visualstudio.com/)
    - [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
    - [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
