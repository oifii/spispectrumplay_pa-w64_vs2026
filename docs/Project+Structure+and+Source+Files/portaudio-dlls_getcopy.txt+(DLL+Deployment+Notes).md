# Project Structure and Source Files

## DLL Deployment Notes: portaudio-dlls_getcopy.txt

This section describes how to ensure the PortAudio runtime DLLs are correctly deployed alongside the built executable. Without these DLLs in the appropriate folders, the application will fail to launch due to missing dependencies.

---

### 🎯 Purpose

- Provide **manual copy commands** for PortAudio’s 32-bit and 64-bit DLLs.
- Ensure `portaudio_x86.dll` and `portaudio_x64.dll` reside next to the `.exe` in both root and configuration subfolders.
- Support both **Debug** and **Release** builds for **Win32** and **x64** targets.

---

### ⚙️ Deployment Commands

```batch
D:\spicode\audio_spi\spispectrumplay_pa-w64_vs2026>copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.dll .\portaudio_x86.dll
D:\spicode\audio_spi\spispectrumplay_pa-w64_vs2026>copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.dll .\Debug\portaudio_x86.dll
D:\spicode\audio_spi\spispectrumplay_pa-w64_vs2026>copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Release\portaudio_x86.dll .\Release\portaudio_x86.dll

D:\spicode\audio_spi\spispectrumplay_pa-w64_vs2026>copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Debug\portaudio_x64.dll .\portaudio_x64.dll
D:\spicode\audio_spi\spispectrumplay_pa-w64_vs2026>copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Debug\portaudio_x64.dll .\x64\Debug\portaudio_x64.dll
D:\spicode\audio_spi\spispectrumplay_pa-w64_vs2026>copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.dll .\x64\Release\portaudio_x64.dll
```

These commands are extracted from `portaudio-dlls_getcopy.txt` .

---

### 📁 Directory Layout

Below is an overview of where each DLL must be placed:

| Platform | Configuration | Source DLL Path | Destination Folder |
| --- | --- | --- | --- |
| Win32 | Debug | `lib-src/portaudio-2021/portaudio_vs2026/build/msvc/Win32/Debug/portaudio_x86.dll` | `.\` & `.\Debug\` |
| Win32 | Release | `lib-src/portaudio-2021/portaudio_vs2026/build/msvc/Win32/Release/portaudio_x86.dll` | `.\Release\` |
| x64 | Debug | `lib-src/portaudio-2021/portaudio_vs2026/build/msvc/x64/Debug/portaudio_x64.dll` | `.\` & `.\x64\Debug\` |
| x64 | Release | `lib-src/portaudio-2021/portaudio_vs2026/build/msvc/x64/Release/portaudio_x64.dll` | `.\x64\Release\` |


- **Root folder (**`**.\**`**)**: ensures the DLL is found when launching from the project directory.
- **Configuration subfolders** (`Debug`, `Release`, `x64\Debug`, `x64\Release`): ensures the DLL is available next to the compiled `.exe` when running inside those output directories.

---

### 🔗 Integration into Build Process

To automate deployment, add these commands as **Post-Build Events** in Visual Studio:

1. Open your project’s **Properties → Build Events → Post-Build Event**.
2. Paste the relevant `copy` commands for each platform/configuration.
3. Use macros to replace hard-coded paths, e.g.:

```batch
   copy "$(SolutionDir)lib-src\portaudio-2021\portaudio_vs2026\build\msvc\$(Platform)\$(Configuration)\portaudio_$(PlatformShort).dll" "$(OutDir)"
```

1. Verify that $(Configuration)|$**(Platform)** matches your project settings.

---

### 🛠 Troubleshooting

- **Missing DLL error**: Ensure **both** the root and configuration folders contain the correct DLL.
- **Incorrect bitness**: Do not mix x86 DLLs with x64 executables (and vice versa).
- **Path typos**: Double-check backslashes (`\`) and folder names against your local structure.
- **Build clean**: After adjusting copy commands, perform a **Clean** and **Rebuild**.

---

> **Note:** Without deploying `portaudio_x86.dll` or `portaudio_x64.dll` next to the executable, calls to `Pa_Initialize()` will fail at runtime, preventing the audio stream from opening.