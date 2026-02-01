# Building and Running the Application

## Build Configurations ⚙️

The Visual Studio project (`spispectrumplay.vcxproj`) defines four primary configurations, pairing **Release** and **Debug** modes with **Win32** and **x64** platforms. Each configuration:

- Sets up include paths for PortAudio, rfftw and libsndfile
- Defines platform- and mode-specific preprocessor symbols
- Chooses the appropriate C/C++ runtime library
- Configures optimization and debug settings
- Links against system and third-party libraries

### Available Configurations

- **Release | Win32**
- **Release | x64**
- **Debug   | Win32**
- **Debug   | x64**

---

## Release | Win32

Builds an optimized 32-bit executable with ASIO support.

| Setting | Value |
| --- | --- |
| **Include Paths** | `.\lib-src\portaudio-2021\portaudio_vs2026\include; .\lib-src\rfftw; .\lib-src\libsndfile(rel)\include; …` |
| **Preprocessor Definitions** | `WIN32; NDEBUG; PA_USE_ASIO=1; …` |
| **Runtime Library** | `MultiThreadedDLL` |
| **Optimization** | Enabled |
| **System Libraries** | `user32.lib; gdi32.lib; comdlg32.lib; winmm.lib` |
| **Third-Party Libraries** | • `.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Release\portaudio_x86.lib` <br>• `.\lib-src\rfftw_vs2026\Release\rfftw.lib` <br>• `.\lib-src\libsndfile(rel)\libsndfile-1.lib` |
| **SubSystem** | `Windows` |


---

## Release | x64

Targets 64-bit, linking x64 builds of all dependencies.

| Setting | Value |
| --- | --- |
| **Include Paths** | `.\lib-src\portaudio-2021\portaudio_vs2026\include; .\lib-src\rfftw; .\lib-src\libsndfile(x64)\include; …` |
| **Preprocessor Definitions** | `WIN32; NDEBUG; PA_USE_ASIO=1; …` |
| **Runtime Library** | `MultiThreadedDLL` |
| **Optimization** | Enabled |
| **System Libraries** | `user32.lib; gdi32.lib; comdlg32.lib; winmm.lib` |
| **Third-Party Libraries** | • `.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.lib` <br>• `.\lib-src\rfftw_vs2026\x64\Release\rfftw.lib` <br>• `.\lib-src\libsndfile(x64)\lib\libsndfile-1.lib` |
| **SubSystem** | `Windows` |
| **Ignore Default Libraries** | `false` |


---

## Debug | Win32

Enables debug symbols, disables optimization, and links debug variants.

| Setting | Value |
| --- | --- |
| **Include Paths** | `.\lib-src\portaudio-2021\portaudio_vs2026\include; .\lib-src\rfftw; .\lib-src\libsndfile(rel)\include; …` |
| **Preprocessor Definitions** | `WIN32; _DEBUG; DEBUG; PA_USE_ASIO=1; …` |
| **Runtime Library** | `MultiThreadedDebugDLL` |
| **Optimization** | Disabled |
| **Browse Information** | `true` |
| **System Libraries** | `user32.lib; gdi32.lib; comdlg32.lib; winmm.lib` |
| **Third-Party Libraries** | • `.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.lib` <br>• `.\lib-src\rfftw_vs2026\Debug\rfftw.lib` <br>• `.\lib-src\libsndfile(rel)\libsndfile-1.lib` |
| **SubSystem** | `Windows` |
| **Debug Info** | `true` |


---

## Debug | x64

Mirrors **Debug | Win32** for 64-bit builds.

| Setting | Value |
| --- | --- |
| **Include Paths** | `.\lib-src\portaudio-2021\portaudio_vs2026\include; .\lib-src\rfftw; .\lib-src\libsndfile(x64)\include; …` |
| **Preprocessor Definitions** | `WIN32; _DEBUG; DEBUG; PA_USE_ASIO=1; …` |
| **Runtime Library** | `MultiThreadedDebugDLL` |
| **Optimization** | Disabled |
| **Browse Information** | `true` |
| **System Libraries** | `user32.lib; gdi32.lib; comdlg32.lib; winmm.lib` |
| **Third-Party Libraries** | • `.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Debug\portaudio_x64.lib` <br>• `.\lib-src\rfftw_vs2026\x64\Debug\rfftw.lib` <br>• `.\lib-src\libsndfile(x64)\lib\libsndfile-1.lib` |
| **SubSystem** | `Windows` |
| **Debug Info** | `true` |


---

## Sample Project Snippet

Below is how the **Release | Win32** settings appear in `spispectrumplay.vcxproj`:

```xml
<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
  <ClCompile>
    <AdditionalIncludeDirectories>
      .\lib-src\portaudio-2021\portaudio_vs2026\include;
      .\lib-src\rfftw;
      .\lib-src\libsndfile(rel)\include;
      %(AdditionalIncludeDirectories)
    </AdditionalIncludeDirectories>
    <PreprocessorDefinitions>
      WIN32;NDEBUG;PA_USE_ASIO=1;%(PreprocessorDefinitions)
    </PreprocessorDefinitions>
    <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
  </ClCompile>
  <Link>
    <AdditionalDependencies>
      user32.lib;gdi32.lib;comdlg32.lib;winmm.lib;
      .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Release\portaudio_x86.lib;
      .\lib-src\rfftw_vs2026\Release\rfftw.lib;
      .\lib-src\libsndfile(rel)\libsndfile-1.lib;
      %(AdditionalDependencies)
    </AdditionalDependencies>
    <SubSystem>Windows</SubSystem>
  </Link>
</ItemDefinitionGroup>
```

This excerpt demonstrates conditional MSBuild settings for platform- and mode-specific dependencies and flags .

---

## Building in Visual Studio

1. Open **spispectrumplay.vcxproj** with Visual Studio 2022 (Platform Toolset v145).
2. Choose **Configuration** (Release/Debug) and **Platform** (Win32/x64) from the toolbar.
3. Select **Build → Build Solution**.
4. Find the resulting binaries in the `Release\` or `Debug\` folder matching your chosen configuration.