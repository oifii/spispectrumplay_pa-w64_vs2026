# Getting Started: Installation and Setup

Begin by preparing your development environment and loading the Visual Studio project. This section walks you through cloning the repository, opening the solution, verifying dependencies, and ensuring all required files are in place.

## Opening the Project in Visual Studio

Follow these steps to load and build the application in Visual Studio 2022.

### 1. Clone the Repository

Clone the project to your local machine, preserving the folder hierarchy so that all relative paths remain valid.

```bash
git clone https://github.com/oifii/spispectrumplay_pa-w64_vs2026.git
cd spispectrumplay_pa-w64_vs2026
```

### 2. Open the Visual Studio Project

1. Launch **Visual Studio 2022**.
2. Choose **File → Open → Project/Solution**.
3. Select **spispectrumplay.vcxproj** from the repository root.

### 3. Verify `lib-src` Folder Placement

> 🔧 The project is configured as a Windows Application with **Debug**/​**Release** and **Win32**/​**x64** platforms. All include directories and linker inputs for PortAudio, rfftw, and libsndfile are pre-set in the project file  .

The build relies on native libraries located under **lib-src**. Ensure **lib-src** sits alongside **spispectrumplay.vcxproj**, for example:

```plaintext
/your/path/
├─ lib-src/
│   ├─ portaudio-2021/
│   ├─ rfftw/
│   └─ libsndfile(rel)/
└─ spispectrumplay.vcxproj
```

| Library | Include Path |
| --- | --- |
| PortAudio | .\lib-src\portaudio-2021\portaudio_vs2026\include |
| rfftw | .\lib-src\rfftw |
| libsndfile | .\lib-src\libsndfile(rel)\include |


```card
{
    "title": "Folder Structure",
    "content": "Ensure `lib-src` and `spispectrumplay.vcxproj` share the same parent directory."
}
```

### 4. Copy PortAudio DLLs

> ⚠️ Misplacing **lib-src** will trigger missing header or library errors during compile .

The application requires the PortAudio runtime DLLs for playback. Copy the correct DLLs into both the project root and the configuration output folders:

| DLL | Source Path | Destinations |
| --- | --- | --- |
| **portaudio_x86.dll** | .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.dll | .\portaudio_x86.dll <br/> .\Debug\portaudio_x86.dll |
| .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Release\portaudio_x86.dll | .\Release\portaudio_x86.dll |  |
| **portaudio_x64.dll** | .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Debug\portaudio_x64.dll | .\portaudio_x64.dll <br/> .\x64\Debug\portaudio_x64.dll |
| .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.dll | .\x64\Release\portaudio_x64.dll |  |


```bash
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Release\portaudio_x86.dll .\Release\portaudio_x86.dll
```

### 5. Place the Test Audio File

> 📋 Use the copy commands listed in **portaudio-dlls_getcopy.txt** as a guide .

By default, the app loads **testbeat2.w64** at runtime. Confirm that this file (or another supported audio file) is present in the working directory:

```cpp
string global_filename = "testbeat2.w64";
```

This default is defined in the source code . If you wish to use a different file, place it here or update the command line accordingly.

---

With these steps completed, your environment is set up to build and run spispectrumplay in Visual Studio. Proceed to build the solution (press **F7** or **Build → Build Solution**) and launch the application.