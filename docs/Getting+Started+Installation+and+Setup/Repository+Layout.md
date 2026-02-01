# Getting Started: Installation and Setup

Welcome to **spispectrumplay_pa-w64_vs2026**. This section guides you through the repository layout, so you can quickly locate key components and understand how third-party libraries are organized.

## Repository Layout

The repository has a simple structure at the root and a `lib-src` folder for vendored libraries.

Below is an overview of the top-level contents and the purpose of each directory.

```plaintext
/
├─ spispectrumplay.vcxproj
├─ spispectrumplay.cpp
├─ spiaudiodevice.h
├─ spiaudiodevice.cpp
├─ resource.h
├─ spispectrumplay.rc
├─ icon1.ico
├─ lib-src/
│  ├─ portaudio-2021/
│  ├─ rfftw/
│  ├─ rfftw_vs2026/
│  ├─ libsndfile(rel)/
│  └─ libsndfile(x64)/
└─ makefile
```

---

### 🔧 Root Directory

The root contains the main application and its build configuration:

- **spispectrumplay.vcxproj**

Main Visual Studio 2022 C++ project file. Defines configurations for **Debug**/​**Release** on **Win32**/​**x64**, include paths, libraries, and toolset **v145** .

- **spispectrumplay.cpp**

Implements the entry point (`WinMain`), PortAudio stream setup, FFT processing, and real-time rendering of spectrum/window modes .

- **spiaudiodevice.h** / **spiaudiodevice.cpp**

`SPIAudioDevice` class: scans and matches audio devices via PortAudio, supports ASIO, WDM-KS, WASAPI, etc., and configures `PaStreamParameters` for input/output .

- **resource.h**

Resource identifiers for icons and dialogs used in **spispectrumplay.rc** .

- **spispectrumplay.rc**

Resource script referencing **resource.h** and embedding the application icon.

- **icon1.ico**

Application icon displayed in the window title bar.

- **makefile**

Legacy NMAKE build instructions that invoke the same sources and libraries.

> **Note:** You’ll build the solution using Visual Studio 2022. The project relies on the **v145** toolset and targets both 32-bit and 64-bit platforms.

---

### 📁 lib-src Directory

All third-party dependencies are vendored under `lib-src`. These are referenced directly in the project’s include and linker settings.

| Path | Description | Purpose |
| --- | --- | --- |
| lib-src/portaudio-2021/portaudio_vs2026 | PortAudio source, headers, and MSVC build outputs | Builds `portaudio_x86.lib`/`portaudio_x64.lib` and matching DLLs for audio I/O with ASIO support |
| lib-src/rfftw & lib-src/rfftw_vs2026 | FFTW real-FFT helper headers (`fourier.h`) and static `rfftw.lib` | Provides optimized real FFT routines used in spectrum analysis |
| lib-src/libsndfile(rel) & lib-src/libsndfile(x64) | libsndfile headers and `libsndfile-1.lib` for Win32 and x64 builds | Enables reading a variety of audio formats (WAV, AIFF, FLAC, etc.) |


#### Example: Include and Link Settings

```xml
<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
  <ClCompile>
    <AdditionalIncludeDirectories>
      .\lib-src\portaudio-2021\portaudio_vs2026\include;
      .\lib-src\rfftw;
      .\lib-src\libsndfile(x64)\include;
      %(AdditionalIncludeDirectories)
    </AdditionalIncludeDirectories>
    <PreprocessorDefinitions>
      WIN32;NDEBUG;PA_USE_ASIO=1;
      %(PreprocessorDefinitions)
    </PreprocessorDefinitions>
  </ClCompile>
  <Link>
    <AdditionalDependencies>
      user32.lib;gdi32.lib;comdlg32.lib;winmm.lib;
      .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.lib;
      .\lib-src\rfftw_vs2026\x64\Release\rfftw.lib;
      .\lib-src\libsndfile(x64)\lib\libsndfile-1.lib;
      %(AdditionalDependencies)
    </AdditionalDependencies>
  </Link>
</ItemDefinitionGroup>
```

*Excerpt from **spispectrumplay.vcxproj***

---

```card
{
    "title": "Build Toolset",
    "content": "Requires Visual Studio 2022 with the v145 toolset for 32-bit and 64-bit targets."
}
```

With this layout in mind, you can navigate the codebase, ensure all dependencies are in place, and proceed to build and run the spectrum visualizer.