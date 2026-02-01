# Using the Audio Spectrum Visualizer – User Interface and Interaction

The Audio Spectrum Visualizer presents a **single semi-transparent layered window** that renders real-time spectrum and waveform visualizations. Users can reposition, resize, and interact with this window to explore multiple display modes. External automation scripts may be chained at startup and shutdown.

## Layered Spectrum Window

A **layered window** is created and configured at startup to host the spectrum display.

| Parameter | Default Value | Description |
| --- | --- | --- |
| **Class name** | `"SPI-Spectrum-Play"` | Window class used for registration and creation. |
| **Window caption** | `"spispectrumplay (click to toggle mode)"` | Title shown in the window (and used by automation scripts). |
| **Position** | `global_x = 200`, `global_y = 200` | X and Y coordinates of the window’s top-left corner on screen. |
| **Size** | `SPECWIDTH = 500`, `SPECHEIGHT = 250` | Width and height of the client area in pixels. |
| **Transparency** | `global_alpha = 200` | Alpha value (0–255) for the layered window; higher is more opaque. |


Configuration is exposed as global variables users may override via command-line arguments .

## Internal Rendering Pipeline

The visualizer uses an **8-bit DIBSection** for fast pixel writes and a periodic timer for updates:

- On **WM_CREATE**, the window is made layered and a DIBSection is created:

```cpp
  SetWindowLong(h, GWL_EXSTYLE,
      GetWindowLong(h, GWL_EXSTYLE) | WS_EX_LAYERED);
  SetLayeredWindowAttributes(h, 0, global_alpha, LWA_ALPHA);
  // Create 8-bit DIBSection and memory DC
  specbmp = CreateDIBSection(dc, (BITMAPINFO*)data,
      DIB_PAL_COLORS, (void**)&specbuf, NULL, 0);
  specdc = CreateCompatibleDC(dc);
  SelectObject(specdc, specbmp);
```

- A **timer** is set up at ~40 Hz to invoke `UpdateSpectrum`, which:
- Reads latest FFT or waveform data.
- Writes pixel values into the **linear buffer** `specbuf`.
- Performs a `BitBlt` from `specdc` to the window DC.

```cpp
  timer = timeSetEvent(
      25, 25,
      (LPTIMECALLBACK)&UpdateSpectrum,
      0, TIME_PERIODIC);
```

- The actual blit occurs in `UpdateSpectrum`:

```cpp
  HDC dc = GetDC(win);
  BitBlt(dc, 0, 0,
         SPECWIDTH, SPECHEIGHT,
         specdc, 0, 0, SRCCOPY);
  ReleaseDC(win, dc);
```

## Mode Switching Controls 🎛

Users cycle through **19 visualization modes** via mouse clicks on the window:

- **Left click** advances to the next mode:

```cpp
  case WM_LBUTTONUP:
      specmode = (specmode + 1) % 19;
      memset(specbuf, 0, SPECWIDTH * SPECHEIGHT);
      return 0;
```

- **Right click** goes back one mode:

```cpp
  case WM_RBUTTONUP:
      specmode = specmode - 1;
      if (specmode < 0) specmode = 18;
      memset(specbuf, 0, SPECWIDTH * SPECHEIGHT);
      return 0;
```

- **Clearing** `specbuf` upon mode change avoids artifacts between displays.

This simple interaction model puts **mode control** under direct user input .

## External Automation Hooks ⚙️

At startup and shutdown, the application can launch external scripts for automation or window management:

- **Startup hook** (`global_begin`, default `"begin.ahk"`):

```cpp
  if (!global_begin.empty())
      ShellExecuteA(
          NULL, "open",
          global_begin.c_str(),
          "", NULL, nShowCmd);
```

- **Shutdown hook** (`global_end`, default `"end.ahk"`):

```cpp
  if (!global_end.empty())
      ShellExecuteA(
          NULL, "open",
          global_end.c_str(),
          "", NULL, nShowCmd);
```

Users may supply custom script names via command-line arguments to coordinate with external tools .

```card
{
    "title": "Automation Hooks",
    "content": "Use `begin.ahk` and `end.ahk` (or custom scripts) to automate window placement, start/stop recording, or integrate with other tools."
}
```

---

This section has detailed the **creation**, **rendering**, and **interaction** mechanisms of the layered spectrum window, as well as its **automation integration points**.