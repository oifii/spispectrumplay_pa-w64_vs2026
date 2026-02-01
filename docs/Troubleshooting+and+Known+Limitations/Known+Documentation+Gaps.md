## Troubleshooting and Known Limitations – Known Documentation Gaps 🔍

The **oifii/spispectrumplay_pa-w64_vs2026** project currently lacks a dedicated user guide or README. As a result, developers and end users must explore the source to discover how to build, configure, and run the application. Below is a summary of the most significant gaps, along with guidance on where to find authoritative details.

| Gap | Impact | Primary Source File |
| --- | --- | --- |
| Undocumented command-line syntax | Users cannot know which parameters are optional, their order, or valid value ranges. | *spispectrumplay.cpp* |
| Incomplete visualization mode docs | Spectrum/waveform modes (values of `specmode`) are not fully described or exemplified in comments. | *spispectrumplay.cpp* |
| Ambiguous end-of-file behavior | EOF handling when `global_fSecondsPlay` is negative depends on commented or conditional code. | *spispectrumplay.cpp* |


---

### 1. Undocumented Command-Line Syntax

The application accepts up to 18 positional arguments (file name, ASIO channels, timeout, window position, visualization mode, palettes, etc.), but no external documentation explains:

- Which arguments are **required** vs. **optional**
- The **valid range** for numeric values
- Default behaviors when parameters are omitted

All parsing logic resides in **spispectrumplay.cpp**. For example:

```cpp
// default to infinite play (negative loops)
//   5: fSecondsPlay   6: x-pos  7: y-pos  8: specmode  ...
global_fSecondsPlay = -1.0;
if(argCount > 5)  global_fSecondsPlay = atof(szArgList[5]);
if(argCount > 6)  global_x           = atoi(szArgList[6]);
if(argCount > 7)  global_y           = atoi(szArgList[7]);
if(argCount > 8)  specmode           = atoi(szArgList[8]);
```

Refer directly to the parsing block for exact parameter indices and types  .

---

### 2. Incomplete Spectrum Mode Descriptions

The integer `specmode` drives 19 visualization styles, including raw FFT bars, filled waveforms, and various backgrounds. Yet, code comments only hint at grouping (e.g., “waveform” vs. “filled waveform”) without:

- Naming each mode
- Illustrating visual differences
- Explaining how `global_bands` or `SPECWIDTH` affect rendering

Key declarations live in **spispectrumplay.cpp**:

```cpp
int specmode = 0, specpos = 0;  // spectrum mode (and marker pos for 2nd mode)
// ...
case WM_LBUTTONUP:
    // cycle forward through specmode 0..18
    specmode = (specmode + 1) % 19;
    memset(specbuf, 0, SPECWIDTH * SPECHEIGHT);
    return 0;
```

Explore the `UpdateSpectrum` and window-procedure cases for mode-specific logic  .

---

### 3. Ambiguous End-of-File Behavior

When playing files via libsndfile + PortAudio, the callback checks for zero samples read (`itemsread == 0`). However, much of the EOF-handling logic is commented out or tied to older BASS code:

```cpp
sf_count_t itemsread = sf_readf_float(global_pSNDFILE, (float*)outputBuffer, framesPerBuffer);
if (itemsread == 0)  // end of file
{
    // commented out: stop-once logic using BASS_STREAMPROC_END
    // f_close(global_pSNDFILE);
    // PostMessage(win, WM_DESTROY, 0, 0);
}
```

With `global_fSecondsPlay < 0`, the stream loops indefinitely, but the exact mechanism depends on legacy comments. Inspect this section in **spispectrumplay.cpp** for reliable behavior  .

---

## Recommendations

- **Refer directly** to **spispectrumplay.cpp** for command-line and visualization details.
- **Consult** **spiaudiodevice.cpp** for low-level PortAudio configuration and device selection.
- **Augment** the repository with a **README** or **user manual** to cover:
- Build instructions
- Usage examples
- Detailed mode gallery

```card
{
    "title": "Key Takeaway",
    "content": "Add a dedicated README to document command-line options, modes, and EOF behavior."
}
```