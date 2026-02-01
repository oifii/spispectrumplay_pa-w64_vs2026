# Getting Started

This section guides you through preparing your development environment for **spispectrumplay**, covering how to leverage the vendored dependencies and ensure the application links and runs correctly.

## Installation and Setup

Before building the project, verify that **Visual Studio 2022** (Toolset v145) is installed. The solution targets both 32-bit and 64-bit platforms. All external C libraries are included in the `lib-src` folder—no system-wide installation is required.

### Installing Dependencies 🔧

The project relies on three key libraries:

- **PortAudio** (with ASIO support)
- **rfftw** (a real-to-real FFTW wrapper)
- **libsndfile** (audio file I/O)

These libraries are vendored under `lib-src` and automatically referenced by the `.vcxproj` file.

```card
{
    "title": "No External Installs",
    "content": "All dependencies live in lib-src; you don't need to install PortAudio, FFTW, or libsndfile separately."
}
```

#### 1. Include Directories

The Visual Studio project file defines the include paths for each build configuration:

|  | Configuration | Include Paths |
| --- | --- | --- |
| Release | Win32 | `.\lib-src\portaudio-2021\portaudio_vs2026\include` |


 

`.\lib-src\rfftw`

 

`.\lib-src\libsndfile(rel)\include`   |

| Release | x64 | `.\lib-src\portaudio-2021\portaudio_vs2026\include` |
| --- | --- | --- |


 

`.\lib-src\rfftw`

 

`.\lib-src\libsndfile(x64)\include`   |

#### 2. Library Dependencies

Linker settings pull in the static or import libraries for each platform:

| Configuration | Linked Libraries |
| --- | --- |
| Debug Win32 | `user32.lib`, `gdi32.lib`, `comdlg32.lib`, `winmm.lib` |


 

`.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.lib`

 

`.\lib-src\rfftw_vs2026\Debug\rfftw.lib`

 

`.\lib-src\libsndfile(rel)\libsndfile-1.lib`  |

| Debug x64 | `user32.lib`, `gdi32.lib`, `comdlg32.lib`, `winmm.lib` |
| --- | --- |


 

`.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Debug\portaudio_x64.lib`

 

`.\lib-src\rfftw_vs2026\x64\Debug\rfftw.lib`

 

`.\lib-src\libsndfile(x64)\lib\libsndfile-1.lib`  |

| Release Win32 | `user32.lib`, `gdi32.lib`, `comdlg32.lib`, `winmm.lib` |
| --- | --- |


 

`.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Release\portaudio_x86.lib`

 

`.\lib-src\rfftw_vs2026\Release\rfftw.lib`

 

`.\lib-src\libsndfile(rel)\libsndfile-1.lib`   |

| Release x64 | `user32.lib`, `gdi32.lib`, `comdlg32.lib`, `winmm.lib` |
| --- | --- |


 

`.\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.lib`

 

`.\lib-src\rfftw_vs2026\x64\Release\rfftw.lib`

 

`.\lib-src\libsndfile(x64)\lib\libsndfile-1.lib`   |

#### 3. PortAudio Runtime DLLs

> **Note:** Windows system libraries (`user32.lib`, `gdi32.lib`, etc.) are included by default.

When running the built executable, ensure the PortAudio DLLs are available alongside `spispectrumplay.exe`. The provided `portaudio-dlls_getcopy.txt` outlines the copy operations:

| Platform | DLL Name | Destinations |
| --- | --- | --- |
| Win32 Debug | portaudio_x86.dll | Project root, `.\Debug\` |
| Win32 Release | portaudio_x86.dll | `.\Release\` |
| x64 Debug | portaudio_x64.dll | Project root, `.\x64\Debug\` |
| x64 Release | portaudio_x64.dll | `.\x64\Release\` |


```bash
# Copy PortAudio DLLs (excerpt)
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.dll .\
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.dll .\Debug\
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.dll .\x64\Release\
```

> Tip: You can automate these steps via post-build events in Visual Studio.

---

With dependencies in place, open the `spispectrumplay.vcxproj` in Visual Studio 2022, select your desired **Configuration** and **Platform**, then **Build**. The vendored libraries and runtime DLLs ensure a smooth setup without external installs.