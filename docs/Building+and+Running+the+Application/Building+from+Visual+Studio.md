# Building and Running the Application

This section describes how to compile **spispectrumplay** in Visual Studio 2022 (v145 toolset) for both 32-bit and 64-bit targets. You will learn how to select the right configuration, build the solution, and verify that the required PortAudio DLL is in place.

## Building from Visual Studio

Follow these steps in Visual Studio’s GUI or via **MSBuild** to produce the executable and its dependencies.

### 1. Select the Desired Configuration

Choose the correct build configuration and platform before compiling. This ensures the compiler and linker use the appropriate settings and libraries.

- In Visual Studio, open **Build → Configuration Manager**.
- Under **Active solution configuration**, pick **Release** or **Debug**.
- Under **Active solution platform**, pick **x64** or **Win32**.
- Click **Close** to apply.

### 2. Build the Solution or Project

> **Note:** The project targets the v145 toolset (Visual Studio 2022). Ensure you have it installed.

Trigger a full rebuild so that all include paths, preprocessor definitions, and libraries are applied correctly.

- In the **Solution Explorer**, right-click the solution or the **spispectrumplay** project.
- Choose **Build** (or **Rebuild** for a clean build).

The compiler uses the `<AdditionalIncludeDirectories>` and `<PreprocessorDefinitions>` you configured in the `.vcxproj` file. The linker pulls in Windows system libraries and third-party static libs from `lib-src`, as specified in `<AdditionalDependencies>` .

```bash
# (Optional) Build from command line using MSBuild
msbuild spispectrumplay.sln /p:Configuration=Release /p:Platform=x64
```

#### Key Settings for Release | x64

| Parameter | Value |
| --- | --- |
| **Include Paths** | `.\lib-src\portaudio-2021\portaudio_vs2026\include; \lib-src\rfftw; \lib-src\libsndfile(x64)\include` |
| **Preprocessor Definitions** | `WIN32;NDEBUG;PA_USE_ASIO=1` |
| **Link Libraries** | `user32.lib; gdi32.lib; comdlg32.lib; winmm.lib; ` |


`.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.lib; `

`.\lib-src\rfftw_vs2026\x64\Release\rfftw.lib; `

`.\lib-src\libsndfile(x64)\lib\libsndfile-1.lib`                                                                                                                   |

| **Output Directory** | `x64\Release\` |
| --- | --- |


### 3. Verify the PortAudio DLL

> ⚙️ **Debug Builds** Debug configurations use similar include paths, but add `_DEBUG;DEBUG` flags and link against debug variants of the libraries (e.g., `portaudio_x64.lib` from the `Debug` folder).

After a successful build, you must ensure that the matching PortAudio dynamic library is present alongside the executable. Post-build events in the project copy the DLL from your PortAudio build folder into the output directory. Confirm that this step completed without error.

```bat
rem Copy PortAudio DLL into the project root and output folder
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.dll .\portaudio_x64.dll
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.dll .\x64\Release\portaudio_x64.dll
```

- If you target **Win32** (x86), the equivalent DLL is `portaudio_x86.dll` under the `Win32\Release` or `Win32\Debug` folder.
- Without the correct DLL in place, the application will fail at runtime with a missing-DLL error.

---

✔️ At this point, you should have:

- A compiled `spispectrumplay.exe` in `\<Platform>\<Configuration>\`.
- The matching `portaudio_*.dll` alongside the executable.
- All dependencies (FFTW, libsndfile) statically linked.

You can now launch **spispectrumplay.exe** to stream audio files and visualize their spectrum in real time.