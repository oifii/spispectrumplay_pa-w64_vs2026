# Using the Audio Spectrum Visualizer – Command-Line Arguments

The application parses its command-line parameters at startup with `CommandLineToArgvA`, mapping each positional argument to a global runtime variable. Supplying arguments allows you to customize audio source, output device, window appearance, visualization modes, automation scripts, and more. If no arguments are provided, defaults from the code are used.

| Arg # | Variable | Description |
| --- | --- | --- |
| 1 | `global_filename` | Path to audio file to open (default: `testbeat2.w64`) |
| 2 | `mySPIAudioDevice.global_audiooutputdevicename` | Output audio device or `"hostapi:devicename"` string |
| 3 – 4 | `mySPIAudioDevice.global_outputAudioChannelSelectors[0..1]` | ASIO channel indices for left/right output (defaults 0, 1) |
| 5 | `global_fSecondsPlay` | Playback duration in seconds; negative for continuous playback |
| 6 – 7 | `global_x`, `global_y` | Initial window X and Y position on screen |
| 8 | `specmode` | Initial visualization mode index |
| 9 – 10 | `global_classname`, `global_title` | Window class name and window title |
| 11 – 12 | `global_begin`, `global_end` | Paths to “begin” and “end” automation scripts (e.g. AutoHotkey) |
| 13 | `global_idcolorpalette` | Color palette ID for spectrum schemes |
| 14 | `global_bands` | Number of spectral bands (controls resolution) |
| 15 – 16 | `SPECWIDTH`, `SPECHEIGHT` | Window width and height in pixels |
| 17 | `global_alpha` | Layered window opacity (0–255) |


## Example: Argument Parsing Snippet

The core parsing logic lives in `WinMain`, immediately after converting the command line into `szArgList`. Here’s a condensed excerpt:

```cpp
LPSTR *szArgList;
int argCount;
szArgList = CommandLineToArgvA(GetCommandLine(), &argCount);
if (!szArgList) { MessageBox(NULL, "Unable to parse command line", "Error", MB_OK); return 10; }

// 1️⃣ Audio file
global_filename = "testwav.wav";
if (argCount > 1) 
    global_filename = szArgList[1];                              //  

// 2️⃣ Output device
mySPIAudioDevice.global_audiooutputdevicename = "E-MU ASIO";
if (argCount > 2) 
    mySPIAudioDevice.global_audiooutputdevicename = szArgList[2]; //  

// 3–4️⃣ ASIO channels
mySPIAudioDevice.global_outputAudioChannelSelectors[0] = 0;
mySPIAudioDevice.global_outputAudioChannelSelectors[1] = 1;
if (argCount > 3) 
    mySPIAudioDevice.global_outputAudioChannelSelectors[0] = atoi(szArgList[3]);
if (argCount > 4) 
    mySPIAudioDevice.global_outputAudioChannelSelectors[1] = atoi(szArgList[4]);

// 5️⃣ Duration
global_fSecondsPlay = -1.0f;                                      // loop indefinitely
if (argCount > 5) 
    global_fSecondsPlay = atof(szArgList[5]);

// 6–7️⃣ Window position
if (argCount > 6) global_x = atoi(szArgList[6]);
if (argCount > 7) global_y = atoi(szArgList[7]);

// 8️⃣ Spec mode
if (argCount > 8) specmode = atoi(szArgList[8]);

// 9–10️⃣ Class & title
if (argCount > 9)  global_classname = szArgList[9];
if (argCount > 10) global_title     = szArgList[10];

// 11–12️⃣ Automation scripts
if (argCount > 11) global_begin = szArgList[11];
if (argCount > 12) global_end   = szArgList[12];

// 13️⃣ Palette ID
if (argCount > 13) global_idcolorpalette = atoi(szArgList[13]);

// 14️⃣ Band count
if (argCount > 14) global_bands = atoi(szArgList[14]);

// 15–16️⃣ Window size
if (argCount > 15) SPECWIDTH  = atoi(szArgList[15]);
if (argCount > 16) SPECHEIGHT = atoi(szArgList[16]);

// 17️⃣ Alpha transparency
if (argCount > 17) global_alpha = (BYTE)atoi(szArgList[17]);

LocalFree(szArgList);
```

## Detailed Argument Reference

- **🎵 Audio File (**`**arg 1**`**)**

Sets the input audio file path via `global_filename`. If omitted, the embedded default (e.g. `testbeat2.w64`) is used.

- **🔊 Output Device (**`**arg 2**`**)**

Assigns `mySPIAudioDevice.global_audiooutputdevicename`. You can specify:

- A simple device name (e.g. `"ASIO:My Device"`)
- Or `"hostapi:devicename"` to bias selection toward a particular host API  .

- **🎚 ASIO Channel Selectors (**`**arg 3**`** & **`**arg 4**`**)**

Choose left/right ASIO channel indices. Defaults are `0` and `1`. Values beyond `1` can select alternate channel pairs on multi-channel hardware  .

- **⏱️ Playback Duration (**`**arg 5**`**)**

Controls `global_fSecondsPlay`. A positive number stops playback after that many seconds; a negative value (default) loops or runs indefinitely  .

- **🖥️ Window Position (**`**arg 6**`** & **`**arg 7**`**)**

Specify the initial upper-left corner of the layered spectrum window in pixels. Mapped to `global_x` and `global_y`  .

- **🎛️ Initial Visualization Mode (**`**arg 8**`**)**

Picks the starting `specmode` (0 = normal FFT, 1 = log scale, 3–18 = various waveform and filled modes)  .

- **🏷️ Window Class & Title (**`**arg 9**`** & **`**arg 10**`**)**

Customize the window’s **class name** (`global_classname`) and **caption text** (`global_title`) for multi-instance scenarios  .

- **⚙️ Automation Scripts (**`**arg 11**`** & **`**arg 12**`**)**

Paths to “begin” and “end” scripts, invoked via `ShellExecuteA`. Ideal for triggering AutoHotkey or batch workflows at startup/shutdown  .

- **🎨 Color Palette (**`**arg 13**`**)**

Selects one of the predefined color mappings (`global_idcolorpalette`), allowing green→red, red→green, grayscale, pink, etc.

- **📊 Spectrum Bands (**`**arg 14**`**)**

Sets `global_bands`. Although the rendering logic may override this to fit the window width, it serves as a high-level spectral resolution control  .

- **📏 Window Size (**`**arg 15**`** & **`**arg 16**`**)**

Define the display dimensions (`SPECWIDTH`, `SPECHEIGHT`) in pixels. Adjusting these alters aspect ratio and rendering scale  .

- **🖌️ Alpha Transparency (**`**arg 17**`**)**

A byte value (0 = fully transparent, 255 = opaque) controlling the layered window opacity via `SetLayeredWindowAttributes`  .

---

By leveraging these arguments, you can script and automate complex visualization setups, tailor the UI to varied displays, and integrate with external workflows seamlessly.