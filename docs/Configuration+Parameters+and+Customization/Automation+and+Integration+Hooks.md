# Configuration Parameters and Customization – Automation and Integration Hooks

This section describes the **automation and integration hooks** that let you launch external scripts or applications at startup and shutdown. These hooks enable window positioning, system audio tweaks, synchronized visuals, and more by calling user-defined scripts.

## Available Hooks

Below are the two global hook parameters you can configure via command-line arguments or by editing their default values in code:

| Hook Variable | Purpose | Default Script | Code Definition Location |
| --- | --- | --- | --- |
| `global_begin` | Script executed at application startup | `begin.ahk` | Defined near top of `spispectrumplay.cpp` |
| `global_end` | Script executed at application shutdown (teardown phase) | `end.ahk` | Defined near top of `spispectrumplay.cpp` |


## How to Invoke the Startup Hook

On startup—after opening the audio file and before initializing PortAudio—the application calls **ShellExecuteA** to launch `global_begin` if it’s non-empty:

```cpp
// Launch user-defined startup script
if (!global_begin.empty())
    ShellExecuteA(
        NULL,               // Parent window handle
        "open",             // Operation
        global_begin.c_str(), // Script path (e.g. "begin.ahk")
        "",                 // Parameters
        NULL,               // Default directory
        nShowCmd            // Window show flag
    );
```

This snippet is located just after the call to `sf_open(...)` and before `Pa_Initialize()` .

## Overriding Hook Scripts via Command Line

You can override the default script names by passing additional arguments. The parser maps argument indices **11** and **12** to these hooks:

```cpp
// argv[11] → global_begin, argv[12] → global_end
if (argCount > 11) global_begin = szArgList[11];
if (argCount > 12) global_end   = szArgList[12];
```

This logic appears in the argument-parsing block of `spispectrumplay.cpp` .

## Use Cases 🎯

- Launch an AutoHotkey script to **reposition** the visualization window and set it **always on top**.
- Adjust system mixer settings or switch audio devices automatically.
- Trigger lighting or multi-screen projection scripts in sync with the spectrum display.
- Integrate with OBS or other streaming tools at startup/shutdown.

## Customizing Shutdown Behavior

The `global_end` hook isn’t automatically invoked in the shipped code. To enable end-of-session automation, extend the **WM_DESTROY** or teardown path:

```cpp
case WM_DESTROY:
    // ... existing cleanup (stop timer, close stream, terminate PortAudio) ...
    if (!global_end.empty())
        ShellExecuteA(NULL, "open", global_end.c_str(), "", NULL, SW_HIDE);
    PostQuitMessage(0);
    return 0;
```

Insert this snippet in the `SpectrumWindowProc` under the `WM_DESTROY` case to ensure your shutdown script runs reliably .

```card
{
    "title": "Shutdown Hook Reminder",
    "content": "Don\u2019t forget to invoke `global_end` in your WM_DESTROY or teardown code for consistent cleanup."
}
```

## Summary

By configuring `global_begin` and `global_end`, you can seamlessly integrate external automation scripts with **spispectrumplay**. Whether it’s window management, audio routing, or multi-app synchronization, these hooks provide a simple yet powerful extension point.