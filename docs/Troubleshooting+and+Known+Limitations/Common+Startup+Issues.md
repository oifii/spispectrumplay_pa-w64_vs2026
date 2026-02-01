# Troubleshooting and Known Limitations

This section covers the most common issues users encounter at startup. Each subsection describes the **cause**, **symptoms**, and **resolution** steps.

## Common Startup Issues

| Error Scenario | Error Message | Action Required |
| --- | --- | --- |
| Missing audio file | “Error, Can’t open file” | Ensure the audio file exists and is readable. |
| PortAudio initialization failure | “portaudio initialization failed” | Verify PortAudio DLLs and installation. |
| Output device selection failure | Silent failure (stream won’t open) | Check `devices.txt` and adjust device string or fallback selection. |


### 🗄️ Missing Audio File

The application uses **libsndfile** to open the default audio file (`testbeat2.w64`). If the file is missing or in an unsupported format, `sf_open` returns `NULL`, and the program aborts with an error dialog.

- **Cause**
- `global_filename` does not point to a valid file in the working directory.
- **Symptom**
- A MessageBox pops up with:

```plaintext
    Error, Can't open file
```

- Program returns exit code `1`.
- **Resolution**
- Place the audio file (`testbeat2.w64` or your custom file) in the working directory.
- Confirm file permissions allow read access.
- If using another format, ensure it is supported by **libsndfile**.

```cpp
if (!(global_pSNDFILE = sf_open(global_filename.c_str(), SFM_READ, &global_SF_INFO))) {
    char errorbuf[2048];
    sprintf(errorbuf, "Error, Can't open file\n");
    MessageBox(0, errorbuf, 0, MB_ICONERROR);
    return 1;  // Can't load the file
}
```

---

### 🎛️ PortAudio Initialization Failure

Before streaming audio, the app calls **PortAudio**’s `Pa_Initialize`. If this fails, no audio backend is available and the app exits.

- **Cause**
- Missing or mismatched PortAudio DLL (e.g., x86 vs. x64).
- Conflicting installations of PortAudio on the system.
- **Symptom**
- A MessageBox appears with:

```plaintext
    portaudio initialization failed
```

- Program returns exit code `1`.
- **Resolution**
- Ensure the correct `portaudio_x86.dll` or `portaudio_x64.dll` is present alongside the executable.
- Verify DLL version matches the build (Debug/Release) and architecture.
- Remove or rename other PortAudio DLLs in the PATH to avoid conflicts.

```cpp
mySPIAudioDevice.global_err = Pa_Initialize();
if (mySPIAudioDevice.global_err != paNoError) {
    MessageBox(0, "portaudio initialization failed", 0, MB_ICONERROR);
    return 1;
}
```

---

### 🔈 Output Device Selection Failure

The function `**SelectAudioOutputDevice**` attempts to match a user-specified device string. If no match is found, it returns `false`, potentially causing the subsequent stream open to fail silently.

- **Cause**
- Specified output device name or host API string does not match any available device.
- **Symptom**
- No explicit MessageBox; the PortAudio stream open may fail with a generic error.
- The generated `devices.txt` report shows available devices.
- **Resolution**
- Open `devices.txt` in the working directory to view all host APIs and device names.
- Adjust the command-line argument or hardcoded `global_audiooutputdevicename` to one of the listed names.
- To fall back to the default device, modify the code:

```cpp
     if (deviceid == paNoDevice) {
         deviceid = Pa_GetDefaultOutputDevice();
         // return true;  // Uncomment to allow default fallback
     }
```

1. Rebuild and rerun.

```cpp
bool SPIAudioDevice::SelectAudioOutputDevice() {
    if (global_outputdevicemap.empty()) {
        ScanAudioDevices();  // populate all devices
    }
    int deviceid = ScanAudioDevices("loosely", spiaudiodeviceOUTPUT);
    if (deviceid == paNoDevice) {
        // deviceid = Pa_GetDefaultOutputDevice();  // default fallback
        return false;
    }
    // … configure global_outputParameters …
    return true;
}
```

---

**Known Limitations**

- The app expects a single audio file in the working directory; no built-in file selector UI is provided.
- Device matching is **loosely** string-based; slight name mismatches can cause failures.
- No retries or prompts are offered on failure; the user must relaunch with corrected parameters.

By following these troubleshooting steps, most startup issues can be resolved quickly, ensuring the spectrum visualizer launches successfully.