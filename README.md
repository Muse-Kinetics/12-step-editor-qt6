# 12 Step Editor

A comprehensive editor application for the 12 Step foot controller, enabling users to configure presets, key sensitivities, CV calibration, expression pedal settings, and advanced MIDI routing.

## Overview

The 12 Step Editor is a cross-platform Qt application that provides complete control over your 12 Step foot controller. Configure everything from individual key sensitivities to CV output routing, manage preset libraries, calibrate expression pedals, and update firmware.

## Features

- **Preset Management**: Create, edit, and organize performance presets
- **Key Sensitivity Configuration**: Individual per-key velocity and pressure settings with global on/off thresholds
- **CV Calibration**: Precise calibration of continuous voltage outputs (octave or per-note calibration)
- **Expression Pedal Calibration**: Define min/max values and response curves (linear, exponential, logarithmic)
- **CV Output Routing**: Configure CV out destinations and MIDI routing from the MIDI tab
- **MIDI Configuration**: 
  - Configure voice messages (Note, CC, Program Change, Bank MSB/LSB, Pitch Bend, Aftertouch)
  - TRS MIDI output routing
  - Program Change preset control (global setting for incoming PC, CC, NRPN commands)
- **Firmware Updates**: Built-in firmware update capability with bootloader installation
- **Factory Reset Options**: Reset CV calibration, key sensitivities, and restore factory presets
- **Legacy Support**: Backward compatibility with original 12 Step hardware (12 Step1)
- **Cross-Platform Support**: Native builds for macOS and Windows

## System Requirements

### Minimum Requirements
- **macOS**: 10.14 (Mojave) or later
- **Windows**: Windows 10 or later. Uses Windows MIDI Services (WMS) when available, with automatic fallback to WinMM on machines without the WMS runtime installed.
- **Qt Framework**: 6.2.1 or later (Qt 5.11.3+ for legacy macOS builds)

### Development Requirements
- Qt 6.2.1+ with Qt Creator (or Qt 5.11.3+ for legacy builds)
- C++17 compatible compiler
- qmake

## Installation

### Pre-built Releases
Download the latest release for your platform from the [Muse Kinetics downloads page](https://www.musekinetics.com/downloads).

### Building from Source

1. **Clone the repository**:
   ```bash
   git clone https://github.com/Muse-Kinetics/12-step-editor-qt6.git
   cd 12-step-editor-qt6
   ```

2. **Install Qt**:
   - **Modern builds (macOS 10.14+, Windows 10+)**: Download Qt 6.2.1+ from [Qt.io](https://www.qt.io/download)
   - **Legacy builds (macOS 10.11-10.13)**: Use Qt 5.11.3
   - Ensure Qt Creator and development tools are installed

3. **Build the application**:
   ```bash
   # Using Qt Creator (recommended)
   # Open QT/12StepEditor.pro in Qt Creator and build

   # Using command line with qmake
   cd QT
   qmake
   make
   ```

4. **Platform-specific build options**:
   - **macOS Universal Binary (M1/Intel)**: Built automatically with Qt 6.2.1+ (requires macOS 10.14+)
   - **macOS Legacy Intel**: Built with Qt 5.11.3 (supports macOS 10.11-10.13)
   - **Windows**: Additional deployment steps create installer automatically when DEPLOY flag is set in .pro file

### Deployment

#### macOS Deployment
The `Signing and Notarization` directory contains a shell script to:
- Deploy the macOS binary and content into a DMG file
- Code sign the application
- Notarize with Apple for distribution
- An Apple developer ID is required

```bash
cd "Signing and Notarization"
./build_sign_package_mac.sh
```

#### Windows Deployment
Windows deployment is built into the Qt .pro file, which creates a Windows installer automatically. The `win-build` directory contains:
- `packages/`: Qt Installer Framework package configuration
- `config/`: Qt Installer Framework configuration scripts

To enable deployment, you must add a build step called "DEPLOY" and ensure the `DEPLOY = 1` flag is uncommented in `12StepEditor.pro`.

## Usage

### Connecting Your 12 Step
1. Connect your 12 Step controller via USB
2. Launch the 12 Step Editor
3. The application will automatically detect your device

**Port Names:**
- **12 Step2**: "12 Step Control Surface", "12 Step TRS MIDI Out", "12 Step CV Out"
- **12 Step1, firmware 1.0.9+**: "12Step Control Surface", "12Step Expander"
- **12 Step1, firmware < 1.0.9 (legacy)**: "12Step Port 1"/"12Step Port 2" (Mac) or "12Step"/"MIDIIN2 (12Step)" (Windows)

The editor recognizes 12 Step1 units on either the old or new port-naming scheme.

### Basic Operation
- **Load Presets**: Access factory and user presets from the preset library
- **Edit Settings**: Modify key sensitivities, CV routing, MIDI configuration, and global parameters
- **Save Changes**: Store your modifications to device memory
- **Backup/Restore**: Export and import preset configurations

### Advanced Features

#### CV Calibration
Hardware menu → CV Calibration
- **Octave Calibration**: Set 6 values per CV output (0V, 1V, 2V, 3V, 4V, 5V)
- **Note Calibration**: Calibrate every note individually for precise tuning

#### Expression Pedal Calibration
Hardware menu → Expression Pedal Calibration
- Define min/max values to scale expression pedal input to output
- Choose response curve: Linear, Exponential, Logarithmic, etc.
- Global setting applied before preset modulation lines

#### Key Sensitivity Configuration
Settings Tab → Key On/Off Thresholds
- **On Threshold**: Pressure required to trigger a key (higher = less sensitive, reduces unintended notes)
- **Off Threshold**: When an active key turns off
  - Higher setting: Prevents stuck keys, allows rapid playing
  - Lower setting: Prevents double triggering
- **Auto-scaling**: Key pressure automatically calibrates based on peak values since power-on

#### Factory Reset
Hardware menu → Factory Reset
- Reset CV calibration values
- Reset key sensitivities to defaults
- Restore factory presets

#### Firmware Updates
File menu → Load Firmware
- Select firmware file from the `Content/Firmware/` directory
- Follow on-screen prompts for bootloader installation (may require unplugging device)
- Firmware is sent as a packetized, chunk-safe transfer with a per-packet identity handshake, rather than as a single blob
- Devices running firmware older than 1.0.0 are no longer offered an in-editor update

## Development

### Project Structure
```
QT/                         # Main application source
├── inc/                    # Include files
│   ├── KMI_KMDM/          # KMI MIDI Device Manager (submodule)
│   └── rtmidi/            # RtMidi library (submodule)
├── mainwindow.cpp/h       # Main application window
├── 12StepEditor.pro       # Qt project file
Content/                    # Application resources
├── Firmware/              # Firmware files
├── Icons/                 # UI graphics and icons
├── 12 Step Legacy Presets/ # Factory presets for 12 Step1
├── Ableton Scripts/       # Ableton Live integration
└── Bitwig/                # Bitwig Studio integration
Signing and Notarization/  # macOS code signing scripts
win-build/                 # Windows installer configuration
├── packages/              # Qt Installer Framework packages
└── config/                # Qt Installer Framework config
```

### Build Configuration
The `.pro` file includes several build flags:
- `DEPLOY = 1`: Enable deployment steps (code signing, installer creation)
- `BUILD_CONSOLE = 1`: Build console version for debugging
- `INCLUDE_QML = 1`: Add qmldir option for Qt deployment if project includes QML

### Code Signing (macOS)
The project includes automated code signing and notarization scripts for macOS distribution. Ensure you have:
- Valid Apple Developer ID certificate
- App-specific password for notarization
- Configured environment variables for signing

### USB MIDI Driver
- **12 Step2**: Uses modern USB MIDI driver (same as latest KMI products)
- **12 Step1**: Firmware 1.0.9+ renamed its ports to "12Step Control Surface"/"12Step Expander"; the editor still recognizes the original unnamed port scheme for units on older firmware

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly on both platforms and with both hardware versions (12 Step1 and 12 Step2)
5. Submit a pull request

## Version History

Current version: **3.0.8** (Editor), **1.0.9** (Firmware)

See [CHANGELOG.md](CHANGELOG.md) for complete version history and release notes.

### Recent Updates (3.0.8)
- Migrated to Windows MIDI Services (WMS), with automatic fallback to WinMM
- Firmware updates now use a packetized, chunk-safe transfer instead of a single blob
- Devices on firmware older than 1.0.0 are no longer offered an in-editor update
- Raised the minimum/default key on/off thresholds and enforce a minimum gap between them
- Added an "Open Log File Location" item to the Help menu
- Firmware 1.0.9 renames the 12 Step1's USB MIDI ports to "12Step Control Surface"/"12Step Expander"; the editor recognizes units on either the old or new naming scheme

### Recent Updates (3.0.5)
- Added global key on/off thresholds to settings tab
- Reduced key-on latency and improved key sensitivity
- Auto-scaling key pressure calibration
- Bug fixes for over-indexing error

### Major Update (3.0.1)
- Complete UI overhaul for 12 Step2
- Added CV Output routing controls
- Added Expression Pedal Calibration
- Added CV Calibration (octave and per-note)
- New USB MIDI driver with modern port names
- Added Bank MSB messages for each voice
- Program Change preset control
- Presets now stored in OS Application Data Directory

## Support

12 Step is currently available for sale through Muse Kinetics with a one year warranty that includes customer support, however this source code is provided as-is with no warranty. For product support please visit https://support.musekinetics.com. For feature requests or bug reports, please create an issue in this repository and encourage other open source developers to assist you.

## License

The 12 Step Editor is distributed under the Mozilla Public License 2.0 (MPL-2.0). You may use, modify, and redistribute the source as long as each modified source file continues to carry the MPL notice and the file is made available under the MPL. See [LICENSE](LICENSE) for the complete terms.

Unless otherwise noted, all contributions are accepted under the MPL-2.0 with the header:

```cpp
// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
```

## Third-Party Licenses

### Qt Framework

This application is built using the Qt framework, which is available under the GNU Lesser General Public License (LGPL) v3.

- **Qt Website**: https://www.qt.io/
- **Qt Open Source License**: LGPL v3
- **Qt Source Code**: https://www.qt.io/download-open-source
- **Qt License Details**: https://www.qt.io/licensing/open-source-lgpl-obligations

The 12 Step Editor application dynamically links with Qt libraries and does not modify the Qt framework source code. Users have the right to obtain, modify, and redistribute the Qt libraries under the terms of the LGPL v3 license.

### Qt LGPL v3 Summary

The Qt framework is licensed under LGPL v3, which allows:
- Use in both open source and commercial applications
- Dynamic linking without affecting your application's license
- Distribution of Qt libraries alongside your application

For complete LGPL v3 license terms, see: https://www.gnu.org/licenses/lgpl-3.0.html

### RtMidi Library

This application uses the RtMidi library for cross-platform MIDI I/O.

- **RtMidi Website**: https://www.music.mcgill.ca/~gary/rtmidi/
- **License**: MIT-style license
- **Source Code**: Available in `QT/inc/rtmidi/` 
