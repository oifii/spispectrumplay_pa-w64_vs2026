# Configuration Parameters and Customization – Advanced Audio Device Configuration

This section details advanced settings for fine-tuning audio device selection and behavior. These options empower users to specify host APIs, choose precise ASIO channel pairs, and generate exhaustive device reports for diagnostics.

## 🚀 Host API and Device Specification

Users can define the audio host API and device in a single string. The format is:

```plaintext
"<HostAPI>:<DeviceName>"
```

When no colon is present, only the device name is considered. The helper performs a **loose, case-insensitive match** against available APIs and devices.

```cpp
// In MatchDevice (loosely):
std::size_t found = global_audiooutputdevicename.find(":");
if (found != std::string::npos) {
    global_audiooutputhostapi     = global_audiooutputdevicename.substr(0, found);
    global_audiooutputdevicename  = global_audiooutputdevicename.substr(found + 1);
    // Log the specified API and device
    if (m_pFILE) {
        fprintf(m_pFILE,
                "hostapi has been specified:\n"
                "global_audiooutputhostapi=%s, global_audiooutputdevicename=%s\n",
                global_audiooutputhostapi.c_str(),
                global_audiooutputdevicename.c_str());
        fflush(m_pFILE);
    }
    // Find matching host API and device
    int hostapitype = MatchHostAPI("loosely", spiaudiodeviceOUTPUT);
    deviceid        = MatchDevice("loosely", spiaudiodeviceOUTPUT, hostapitype);
}
```

(Source: SPIAudioDevice::MatchDevice implementation )

| Example String | Parsed Host API | Parsed Device Name | Use Case |
| --- | --- | --- | --- |
| `"ASIO:My ASIO Card"` | ASIO | My ASIO Card | Pinpoint an ASIO interface |
| `"WASAPI:Speakers"` | WASAPI | Speakers | Select the default WASAPI output |
| `"Realtek High Def"` | (none) | Realtek High Def | Match via partial device name |


## 🔧 ASIO Channel Selectors

On multi-channel ASIO interfaces, you can choose which two channels form the stereo pair using the global selectors:

```cpp
// Default left/right mapping
mySPIAudioDevice.global_outputAudioChannelSelectors[0] = 0; // Left channel index
mySPIAudioDevice.global_outputAudioChannelSelectors[1] = 1; // Right channel index

// Override via command-line arguments:
// argv[3] → left channel index, argv[4] → right channel index
if (argCount > 3) {
    mySPIAudioDevice.global_outputAudioChannelSelectors[0] = atoi(szArgList[3]);
}
if (argCount > 4) {
    mySPIAudioDevice.global_outputAudioChannelSelectors[1] = atoi(szArgList[4]);
}
```

(Source: Command-line parsing in `spispectrumplay.cpp` )

| Parameter | Index in argv | Description |
| --- | --- | --- |
| `outputChannel0` | `argv[3]` | ASIO channel index for the **left** audio |
| `outputChannel1` | `argv[4]` | ASIO channel index for the **right** audio |


These selectors feed into the PortAudio ASIO stream info:

```cpp
global_asioOutputInfo.flags            = paAsioUseChannelSelectors;
global_asioOutputInfo.channelSelectors = global_outputAudioChannelSelectors;
```

(Source: SPIAudioDevice::SelectAudioOutputDevice )

## 📋 Detailed Device Reporting

When diagnosing complex hardware setups, enabling a **detailed device report** can reveal host API names, device IDs, channel counts, latencies, and ASIO buffer capabilities.

1. **Enable reporting**

In the `SPIAudioDevice` constructor, the flag defaults to `true`:

```cpp
   SPIAudioDevice::SPIAudioDevice() {
       bdetailedreport = true;
       m_pFILE         = nullptr;
   }
```

(Source: `spiaudiodevice.cpp` )

1. **Open a writable log file**

```cpp
   FILE* pFILE = fopen("devices.txt", "w");
   mySPIAudioDevice.m_pFILE = pFILE;
```

1. **Invoke device selection**

```cpp
   mySPIAudioDevice.SelectAudioOutputDevice();
   fclose(mySPIAudioDevice.m_pFILE);
   mySPIAudioDevice.m_pFILE = nullptr;
```

(Source: Main setup in `spispectrumplay.cpp` )

1. **Inspect the generated **`**devices.txt**`

The log includes entries such as:

```plaintext
   --------------------------------------- device #3
   [ Default Output ]
   id=3, hostapi=ASIO, devicename=My ASIO Card
   Max inputs  = 0, Max outputs = 8
   Default low output latency  = 0.0020
   ASIO minimum buffer size    = 64
   ASIO maximum buffer size    = 4096
   ASIO preferred buffer size  = 256
   ASIO buffer granularity     = power of 2
   ----------------------------------------------
```

| Setting | Effect |
| --- | --- |
| `bdetailedreport` | Toggles verbose device logging |
| `m_pFILE` | File handle for writing detailed reports |


## Flow of Advanced Configuration

```mermaid
flowchart LR
    A[Start Application] --> B[Parse CLI Arguments]
    B --> C[Set Device Name & HostAPI]
    C --> D[Set ASIO Channel Selectors]
    D --> E[Open Report File if Enabled]
    E --> F[Scan All Audio Devices]
    F --> G[Match Host API Loosely]
    G --> H[Match Device Loosely]
    H --> I[Configure PortAudio Stream]
    I --> J[Run Audio Visualization]
```

This flowchart illustrates how **host API matching**, **device selection**, and **custom channel mapping** integrate before launching the real-time spectrum visualization.

---

<p style="text-align:center">🎯 Tailor these parameters to your hardware for optimal audio routing and diagnostics.</p>