# Project Structure and Source Files

## resource.h (Application Resources)

The **resource.h** header defines symbolic constants for resources used by the Windows resource compiler and the application. It centralizes identifiers for icons, commands, controls, and other symbols, ensuring consistency across the `.rc` file, source code, and project configuration.

### Defined Resource Identifiers 🎨

Below is a summary of the identifiers declared in **resource.h**:

| Identifier | Value | Description |
| --- | --- | --- |
| **IDI_ICON1** | 101 | Application icon resource |
| **_APS_NEXT_RESOURCE_VALUE** | 102 | Next available resource ID |
| **_APS_NEXT_COMMAND_VALUE** | 40001 | Next available command ID |
| **_APS_NEXT_CONTROL_VALUE** | 1001 | Next available control ID |
| **_APS_NEXT_SYMED_VALUE** | 101 | Next available symbol ID |


```c
// resource.h
#define IDI_ICON1                       101
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        102
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
```

—

### Integration in the Resource Script

The `.rc` file includes **resource.h** to map these constants to actual resource files. For example, the application icon is declared as:

```rc
#include "resource.h"
IDI_ICON1 ICON "icon1.ico"
```

—

This ensures that the icon file `icon1.ico` is assigned the identifier `IDI_ICON1` during compilation.

### Runtime Usage in Source Code

At runtime, the application loads resources by referencing these constants. In `spispectrumplay.cpp`, the icon is loaded into the window class:

```cpp
wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
```

—

Here, `MAKEINTRESOURCE(IDI_ICON1)` tells the Windows API to fetch the compiled icon resource with ID `101`.

### Customizing Resources

To add or modify resources:

- Add new entries to **resource.h** for unique IDs.
- Declare resources in the `.rc` file using those IDs.
- Reference the IDs in code or menu/dialog definitions.

Example:

1. Define a dialog ID in **resource.h**:

```c
   #define IDD_SETTINGS_DIALOG  201
```

1. Declare the dialog in `spispectrumplay.rc`:

```rc
   IDD_SETTINGS_DIALOG DIALOGEX 0, 0, 200, 100
   BEGIN
       CONTROL         "OK", IDOK, "Button", WS_TABSTOP, 50, 70, 50, 14
   END
```

1. In code, create or display the dialog:

```cpp
   DialogBox(hInstance, MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), hwndParent, SettingsDlgProc);
```

### Build Integration

The Visual Studio project file includes **resource.h** and the resource script for compilation:

```xml
<ItemGroup>
  <ClInclude Include="resource.h" />
  <ResourceCompile Include="spispectrumplay.rc" />
  <None Include="icon1.ico" />
</ItemGroup>
```

—

This ensures that resource identifiers and assets are part of every build configuration.

---

By organizing resource identifiers in **resource.h**, the project maintains a clear, centralized mapping between symbolic constants, compiled resources, and runtime usage, simplifying maintenance and extension of application resources.