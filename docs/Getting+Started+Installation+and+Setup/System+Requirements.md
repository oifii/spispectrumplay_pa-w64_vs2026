# Getting Started: Installation and Setup – System Requirements

This section describes the **system prerequisites** and **development environment** needed to build and run SPI Spectrum Play. Ensure your machine meets these requirements before proceeding with the installation.

## Supported Operating Systems 💻

SPI Spectrum Play targets the **Windows desktop** platform using the Win32 API and GDI. Both **32-bit** and **64-bit** editions of Windows are supported:

- Windows 7, 8, 8.1, 10, or 11
- Any edition capable of running standard Win32 GUI applications

## Development Environment 🛠️

You must install a supported version of **Microsoft Visual Studio** with the appropriate C++ toolset and Windows SDK:

| Component | Minimum Version / Requirement |
| --- | --- |
| Visual Studio | 2022 (with **v145** C++ toolset) |
| C++ Toolset | v145 ( in project files) |
| Windows SDK | 10.0 () |
| Workload | **Desktop development with C++** |


## Hardware Requirements

> **Card** ```card { "title": "Tip", "content": "Install the 'Desktop development with C++' workload in Visual Studio to get the Win32 SDK and v145 toolset." } ```

- A PC capable of running 32-bit or 64-bit Windows.
- An audio interface compatible with the **PortAudio** host APIs (MME, WASAPI, ASIO, etc.).

## Bundled External Libraries 📦

The project includes the following libraries under the `lib-src` directory. These are prebuilt for both Win32 and x64 configurations and wired into the Visual Studio project:

| Library | Source Path | Notes |
| --- | --- | --- |
| PortAudio | `lib-src/portaudio-2021/portaudio_vs2026` | ASIO support enabled () |
| rfftw/FFTW | `lib-src/rfftw` | FFT wrapper from *Audio Programming Book* |
| libsndfile | `lib-src/libsndfile(rel)` (Win32) <br> `lib-src/libsndfile(x64)` | Reads `.wav`, `.w64`, etc. formats |


## Project Configuration Snippets

The Visual Studio project file (`spispectrumplay.vcxproj`) wires these libraries into each build configuration via **AdditionalIncludeDirectories** and **AdditionalDependencies**:

```xml
<!-- Release | x64 Configuration -->
<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
  <ClCompile>
    <AdditionalIncludeDirectories>
      .\lib-src\portaudio-2021\portaudio_vs2026\include;
      \lib-src\rfftw;
      \lib-src\libsndfile(x64)\include;
      %(AdditionalIncludeDirectories)
    </AdditionalIncludeDirectories>
  </ClCompile>
  <Link>
    <AdditionalDependencies>
      user32.lib;gdi32.lib;comdlg32.lib;winmm.lib;
      \lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.lib;
      \lib-src\rfftw_vs2026\x64\Release\rfftw.lib;
      \lib-src\libsndfile(x64)\lib\libsndfile-1.lib;
      %(AdditionalDependencies)
    </AdditionalDependencies>
    <SubSystem>Windows</SubSystem>
  </Link>
</ItemDefinitionGroup>
```

```xml
<!-- Debug | Win32 Configuration -->
<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
  <ClCompile>
    <AdditionalIncludeDirectories>
      .\lib-src\portaudio-2021\portaudio_vs2026\include;
      \lib-src\rfftw;
      \lib-src\libsndfile(rel)\include;
      %(AdditionalIncludeDirectories)
    </AdditionalIncludeDirectories>
    <PreprocessorDefinitions>
      WIN32;_DEBUG;DEBUG;PA_USE_ASIO=1;%(PreprocessorDefinitions)
    </PreprocessorDefinitions>
  </ClCompile>
  <Link>
    <AdditionalDependencies>
      user32.lib;gdi32.lib;comdlg32.lib;winmm.lib;
      \lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.lib;
      \lib-src\rfftw_vs2026\Debug\rfftw.lib;
      \lib-src\libsndfile(rel)\libsndfile-1.lib;
      %(AdditionalDependencies)
    </AdditionalDependencies>
    <SubSystem>Windows</SubSystem>
    <GenerateDebugInformation>true</GenerateDebugInformation>
  </Link>
</ItemDefinitionGroup>
```

These settings ensure that the compiler and linker can find the **headers** and **static/dynamic libraries** for PortAudio, rfftw, and libsndfile during build time  .

---

With these **system requirements** satisfied, you’re ready to clone the repository and proceed to the next step: configuring and building the solution in Visual Studio.