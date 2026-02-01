# Project Structure and Source Files – PortAudio Device Helper 🎛

This section covers the **SPIAudioDevice** helper, encapsulated in `spiaudiodevice.h` and `spiaudiodevice.cpp`. It isolates complex PortAudio device enumeration and selection logic from the main application, enabling flexible device matching, ASIO support, and detailed reporting.

## spiaudiodevice.h

This header declares the **SPIAudioDevice** class and related types. It defines data structures for storing device maps, user preferences, and stream configuration handles.

```cpp
/* spiaudiodevice.h */
typedef enum spiaudiodevicetypeflag {
  spiaudiodeviceALL = 0,
  spiaudiodeviceINPUT = 1,
  spiaudiodeviceOUTPUT = 2
} spiaudiodevicetypeflag;

class SPIAudioDevice {
public:
  // Device maps
  std::map<int, std::string> global_alldevicemap;
  std::map<int, std::string> global_inputdevicemap;
  std::map<int, std::string> global_outputdevicemap;

  // Host API maps
  std::map<int, std::string> global_hostapimap;
  std::map<int, std::string> global_hostapimap_hostapitype;
  std::map<int, std::string> global_hostapimap_directsound;
  std::map<int, std::string> global_hostapimap_mme;
  std::map<int, std::string> global_hostapimap_asio;
  // … additional host APIs (WASAPI, JACK, etc.)

  // Stream handles and parameters
  PaStream*               global_stream;
  PaStreamParameters      global_inputParameters;
  PaStreamParameters      global_outputParameters;
  PaError                 global_err;

  // User selections
  std::string             global_audioinputdevicename;
  std::string             global_audiooutputdevicename;
  std::string             global_audioinputhostapi;
  std::string             global_audiooutputhostapi;
  int                     global_inputAudioChannelSelectors[2];
  int                     global_outputAudioChannelSelectors[2];

  // ASIO-specific info (Windows)
#if (defined(_WIN64) || defined(_WIN32))
# if PA_USE_ASIO
    PaAsioStreamInfo     global_asioInputInfo;
    PaAsioStreamInfo     global_asioOutputInfo;
# endif
#endif

  // Reporting
  bool                    bdetailedreport;
  FILE*                   m_pFILE;

  // Constructor / Destructor
  SPIAudioDevice();
  ~SPIAudioDevice();

  // Core methods
  int   ScanAudioDevices(std::string matchmode = "", spiaudiodevicetypeflag ioflag = spiaudiodeviceALL);
  int   MatchHostAPI   (std::string matchmode = "", spiaudiodevicetypeflag ioflag = spiaudiodeviceALL);
  int   MatchDevice    (std::string matchmode = "", spiaudiodevicetypeflag ioflag = spiaudiodeviceALL, int hostapitype = -1);
  bool  SelectAudioInputDevice();
  bool  SelectAudioOutputDevice();
  std::string GetHostAPIName(int deviceid);
  std::string GetDeviceName   (int deviceid);
};
```

### Core Members ⚙️

| Member | Type | Description |
| --- | --- | --- |
| global_alldevicemap | `map<int,string>` | All PortAudio device names by ID |
| global_inputdevicemap | `map<int,string>` | Devices with input capability |
| global_outputdevicemap | `map<int,string>` | Devices with output capability |
| global_hostapimap | `map<int,string>` | Host API name per device |
| global_asioInputInfo / global_asioOutputInfo | `PaAsioStreamInfo` | ASIO stream info for input/output (Windows only) |
| bdetailedreport | `bool` | Enables verbose device reporting |
| m_pFILE | `FILE*` | File handle for writing detailed reports |


## spiaudiodevice.cpp

This source file implements all **SPIAudioDevice** methods.

### Constructor & Destructor

- **SPIAudioDevice()**
- Initializes device and host‐API name strings to empty.
- Sets `bdetailedreport = true` to enable logs by default.
- Sets `m_pFILE = NULL`; user must assign a log file before scanning.

- **~SPIAudioDevice()**
- Default destructor, no special cleanup required.

### ScanAudioDevices

Populates device and host‐API maps by querying PortAudio. Logs details if `bdetailedreport` is true.

- Clears existing maps.
- Iterates over `Pa_GetDeviceCount()`.
- For each device:
- Inserts into `global_alldevicemap`.
- Adds to input/output maps if channels > 0.
- Records host API names in `global_hostapimap` and specific API maps (DirectSound, WASAPI, ASIO, etc.).

### MatchHostAPI

Performs flexible, case‐insensitive partial matching of a user‐provided host API name against discovered APIs.

- Reads user preference (`global_audioinputhostapi` or `global_audiooutputhostapi`).
- Scans `global_hostapimap_hostapitype` for substring matches.
- Returns the first matching host API type code or `-1` if unspecified.

### MatchDevice

Matches a device name string to one of the enumerated devices, optionally filtering by host API.

- Uses `matchmode` to control strictness (exact vs. loose).
- Filters by `ioflag` (input, output, or all).
- Returns the device ID (`int`) or `paNoDevice`.

### SelectAudioInputDevice

Configures **global_inputParameters** for streaming.

1. Ensures device maps are populated (calls `ScanAudioDevices` if needed).
2. Finds desired device ID via `ScanAudioDevices("loosely", spiaudiodeviceINPUT)`.
3. If none found, falls back to `Pa_GetDefaultInputDevice()`.
4. Sets `channelCount`, `sampleFormat`, and `suggestedLatency`.
5. Configures `hostApiSpecificStreamInfo` for ASIO or other APIs.
6. Returns `true` on success.

### SelectAudioOutputDevice

Analogous to input selection, but for **global_outputParameters**. It handles ASIO channel selectors and latency settings.

### Utility Getters

- **GetHostAPIName(deviceid)**

Returns the recorded host API name for a given device ID.

- **GetDeviceName(deviceid)**

Returns the device name for a given device ID.

## How It Fits Together

The main application (`spispectrumplay.cpp`) uses **SPIAudioDevice** to initialize PortAudio, scan devices, and open the audio stream:

```cpp
// In spispectrumplay.cpp
mySPIAudioDevice.global_err = Pa_Initialize();
mySPIAudioDevice.m_pFILE = fopen("devices.txt","w");
mySPIAudioDevice.SelectAudioOutputDevice();
fclose(mySPIAudioDevice.m_pFILE);

Pa_OpenStream(&mySPIAudioDevice.global_stream,
              NULL,
              &mySPIAudioDevice.global_outputParameters,
              SAMPLE_RATE, FRAMES_PER_BUFFER,
              paClipOff, renderCallback, NULL);
Pa_StartStream(mySPIAudioDevice.global_stream);
```

This encapsulation ensures that device discovery, matching, and parameter setup are cleanly separated from rendering and audio processing logic.

---

Throughout, **SPIAudioDevice** provides a robust, extensible foundation for multi-API audio device management, simplifying cross-platform adaptation and user-driven preferences.