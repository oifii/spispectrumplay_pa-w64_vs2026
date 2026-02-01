## Building and Running the Application – Required Runtime Files

Before launching spispectrumplay, ensure all external libraries and assets are present in the executable’s working folder. Missing runtime files will cause load failures or silent audio errors.

### PortAudio DLLs 🎛️

The application uses **PortAudio** for cross-platform audio I/O. At runtime you must provide the matching DLL for your build:

| **DLL** | **Architecture** | **Copy To** | **Purpose** |
| --- | --- | --- | --- |
| **portaudio_x86.dll** | Win32 | `./` and `./Debug/` / `./Release/` | Load-time audio engine (32-bit) |
| **portaudio_x64.dll** | x64 | `./` and `./x64/Debug/` / `./x64/Release/` | Load-time audio engine (64-bit) |


The file `portaudio-dlls_getcopy.txt` shows sample copy commands:

```bash
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.dll .\portaudio_x86.dll
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Debug\portaudio_x86.dll .\Debug\portaudio_x86.dll
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\Win32\Release\portaudio_x86.dll .\Release\portaudio_x86.dll
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Debug\portaudio_x64.dll .\portaudio_x64.dll
copy .\lib-src\portaudio-2021\portaudio_vs2026\build\msvc\x64\Release\portaudio_x64.dll .\x64\Release\portaudio_x64.dll
```

These commands ensure the executable can locate **portaudio_x86.dll** or **portaudio_x64.dll** at load time .

### FFTW Library (rfftw)

The real-to-complex FFT routines come from **rfftw**, which is linked statically into `rfftw.lib`. No separate FFTW DLL is required at runtime.

- The header **fourier.h** wraps **rfftw** and is compiled into a static library .
- At build time, the linker pulls in `rfftw.lib` from the appropriate configuration folder.

### libsndfile Library 🧮

Audio file reading is handled by **libsndfile**. Depending on your libsndfile build, you must link either statically or via an import library:

| **Library** | **Configuration** | **Path** |
| --- | --- | --- |
| **libsndfile-1.lib** | 32-bit Rel/Dbg | `.\lib-src\libsndfile(rel)\libsndfile-1.lib` |
| **libsndfile-1.lib** | 64-bit Rel/Dbg | `.\lib-src\libsndfile(x64)\lib\libsndfile-1.lib` |


The project’s `<AdditionalDependencies>` entries reference these `.lib` files so no **libsndfile-1.dll** is needed .

### Default Audio File 🎵

By default, the global filename is set to `**"testbeat2.w64"**` in the code:

```cpp
// default sound file name
string global_filename = "testbeat2.w64";
SNDFILE* global_pSNDFILE = NULL;
```

This file must reside in the application’s current working directory, or you must override `global_filename` before building or via command-line parameters .

> **Tip:** To use a different audio file, edit `global_filename` in **spispectrumplay.cpp** or pass the filename as the first argument when launching the executable.