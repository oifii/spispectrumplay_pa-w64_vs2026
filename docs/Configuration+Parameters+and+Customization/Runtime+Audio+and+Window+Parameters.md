## Configuration Parameters and Customization

This section describes the global variables that control playback and visualization at runtime. You can adjust window appearance, audio playback, and rendering behavior without recompiling. Many settings support command-line overrides for quick customization.

### Runtime Audio and Window Parameters

Several globals govern how the app opens files, streams audio, and renders the spectrum. Defaults are defined in the main source (e.g., `spispectrumplay.cpp`) and can be changed via command-line arguments  .

#### 🪟 Window Geometry and Appearance

These parameters define the spectrum window’s size, position, and look.

| Parameter | Type | Default | Description | CLI Arg |
| --- | --- | --- | --- | --- |
| **SPECWIDTH** | int | 500 | Window **width** in pixels. Affects spectrum resolution and buffer size. | 15 |
| **SPECHEIGHT** | int | 250 | Window **height** in pixels. Affects spectrum resolution and buffer size. | 16 |
| **global_x** | int | 200 | Initial **X** screen coordinate of the window’s top-left corner. | 6 |
| **global_y** | int | 200 | Initial **Y** screen coordinate of the window’s top-left corner. | 7 |
| **global_alpha** | byte | 200 (0–255) | Layered-window **opacity**; higher is more opaque. | 17 |
| **global_classname** | string | "SPI-Spectrum-Play" | Window **class** name (used by `RegisterClass`). | 9 |
| **global_title** | string | "spispectrumplay (click to toggle mode)" | Window **caption** shown in the title bar. | 10 |


```cpp
int SPECWIDTH    = 500;          // display width
int SPECHEIGHT   = 250;          // display height
int global_x     = 200;          // screen X position
int global_y     = 200;          // screen Y position
BYTE global_alpha = 200;         // window opacity
string global_classname = "SPI-Spectrum-Play";
string global_title     = "spispectrumplay (click to toggle mode)";
```

#### 🎵 Audio Playback Parameters

These globals control which file plays, how long, and how many channels to use.

| Parameter | Type | Default | Description | CLI Arg |
| --- | --- | --- | --- | --- |
| **global_filename** | string | "testbeat2.w64" | Path to the audio file opened by libsndfile. | 1 |
| **global_fSecondsPlay** | float | –1.0 | Duration in seconds; negative loops continuously. | 5 |
| **global_numchannels** | int | `NUM_CHANNELS` | Number of audio channels for PortAudio stream and FFT. | – |


```cpp
string global_filename      = "testbeat2.w64";
float  global_fSecondsPlay   = -1.0f;    // negative = continuous playback
int    global_numchannels    = NUM_CHANNELS;
```

#### 📊 Visualization Parameters

These govern which spectrum or waveform mode to render, band layout, and color palette.

| Parameter | Type | Default | Description | CLI Arg |
| --- | --- | --- | --- | --- |
| **specmode** | int | 0 | Current visualization **mode** index (0–18). | 8 |
| **prev_specmode** | int | 0 | Tracks previous mode to reset state when switching. | – |
| **specpos** | int | 0 | Position marker for 2D spectrum “scroll” modes. | – |
| **global_idcolorpalette** | int | 0 | Index of the selected **color palette** for the 8-bit DIBSection. | 13 |
| **global_bands** | int | 20 | Target **number of spectrum bands**; combined with width and FFT size. | 14 |


```cpp
int specmode              = 0;    // current display mode
int prev_specmode         = 0;    // previous mode
int specpos               = 0;    // marker position in scroll modes
int global_idcolorpalette = 0;    // palette selector
int global_bands          = 20;   // target band count
```

#### Command-Line Overrides

You can override defaults by supplying arguments to the executable. The argument list maps as follows:

| Arg Index | Variable | Usage Example |
| --- | --- | --- |
| 1 | `global_filename` | `"mysong.wav"` |
| 2 | Audio output device name | `"ASIO Driver"` |
| 3,4 | ASIO channel selectors | `0 1` |
| 5 | `global_fSecondsPlay` | `60` |
| 6,7 | `global_x` `global_y` | `100 150` |
| 8 | `specmode` | `3` |
| 9 | `global_classname` | `"MyClassName"` |
| 10 | `global_title` | `"Spectrum Viewer"` |
| 11,12 | `global_begin` `global_end` | `"start.bat" "stop.bat"` |
| 13 | `global_idcolorpalette` | `2` |
| 14 | `global_bands` | `32` |
| 15,16 | `SPECWIDTH` `SPECHEIGHT` | `800 400` |
| 17 | `global_alpha` | `180` |


```bash
spispectrumplay.exe \
  "mysong.wav"    \
  "ASIO Driver"   \
  0 1             \
  60              \
  100 150         \
  3               \
  "MyClassName"   \
  "Spectrum View" \
  "start.bat"     \
  "stop.bat"      \
  2 32            \
  800 400         \
  180
```

*(Sets file, device, channels, play time, window pos, mode, class, title, batch scripts, palette, bands, size, opacity.)*

---

> **Tip:** Adjust **SPECWIDTH**, **SPECHEIGHT**, and **global_bands** together to maintain visual clarity and avoid buffer overruns. Ensure **global_alpha** stays within 0–255 for valid opacity.