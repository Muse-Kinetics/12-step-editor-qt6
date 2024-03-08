#### 12 Step Editor and Firmware Changelog

### Editor 3.0.2, Firmware 1.0.2 (2024-03-06)
- Bug Fixes:
	- Added extra steps to bootloader installation process (unplug device, relaunch app) to account for Windows MIDI drivers refusing to let go of device ports during reboot

### Editor 3.0.1, Firmware 1.0.2 (2024-02-29)
Major release update for 12 Step2
- New Features/Changes:
	- 12 Step2 Editor UI additions:
		- Added CV Out routing controls to MIDI Tab
		- Added Backlight Brightness control to Settings Tab
		- Replaced key frames and setlist button PNGs with SVGs (much cleaner)
		- Windows scaling and sizing fixes for combobox widths and fonts 
	- Added Expression Pedal Calibration to the Editor Hardware menu
		- User can define min/max values to scale expression pedal input to output
		- Added table assignment (linear, exponential, log, etc). This is a global setting that will always be applied to the expression pedal before preset modlines.
	- Added CV Calibration to the Editor Hardware Menu
		- User can calibrate Octaves (6 values per CV out for 0/1/2/3/4/5V), or Notes (calibrate every note individually)
	- Complete overhaul of USB MIDI driver. 12 Step now uses the same driver as KMI's latest products.
		- 12 Step2 uses new USB MIDI port names: "12 Step Control Surface", "12 Step TRS MIDI Out", "12 Step CV Out"
		- Legacy hardware (12 Step1) uses old port names "12 Step Port 1" and "12 Step Port 2", this retains backwards compatibility with older editors.
	- Added Bank MSB messages for each voice
		- The previous "Bank" message was for the LSB (CC32), and was always enabled if Program Changes were enabled. Now all three messages can be individually enabled as needed.
		- This feature replaced Pitch Bend Range. 12 Step now sends the full 14bit pitch bend range, so users should configure pitch bend range at the receiving MIDI device.
	- Added Program Change Preset Control dropdown to Settings tab. 
		- This is a global setting for all presets that determines which MIDI channel 12 Step listens to for Program Chance, CC, and NRPN commands. 
	- Presets are now stored in the OS Application Data Directory rathern than the application directory/package. 
		- A README.PDF has been included in the MacOS and Windows installation files that explains how to back up presets from previous editors before upgrading.
		- When loading editor 3.0.0 for the first time, you will be prompted if you want to import your old presets or load factory defaults.
	- Updated Ableton remote script for Live11/python3.
	- Settings tab sliders now display actual values
	- 12 Steps now report serial numbers to the USB Host. 12 Step2 units have unique serials burned into memory at the factory.
- Bug Fixes:
	- All preset data, settings, and calibration information is now preserved even after a firmware update
	- Fixes to UI scaling for both MacOS and Windows
	- Improved analog sensor scanning and removed some crosstalk noise present when pressing multiple keys
	- Increased KeyOn and KeyOff thresholds to clean up false triggers and stuck notes
	- Global Transposition is now preserved when settings updates are sent from the editor
- NOTE FOR MACOS USERS
	- MacOS Sonoma 14.1.1 has a known bug where CoreMIDI misroutes port data to devices with asymetric USB MIDI port configurations (ie 1in 3out). This means that data sent to the 12 Step2 TRS MIDI and CV out ports will fail. This bug has been fixed in the developer 14.4 beta release. 


### Editor 2.1.0, Firmware 0.3.0 (2022-09-30)
- New Features/Changes:
	- Overhauled editor MIDI backend using RtMidi, now builds/runs on modern operating systems.
	- Added a MIDI THRU port to route incomming 12 Step midi to a virtual port, workaround for Windows inability to share MIDI ports with multiple applications
- Bug Fixes:
	- Sensitivity Sliders
		- 12 Step now reacts to slider adjustments in realtime, you no longer have to send the setlist for them to take affect.
		- Renamed the "Select Sensitivity" slider to the more accurate "Select Button Hold Time", and fixed a bug where a 0 value would disable it
	- Replaced old low-res UI images with vector fonts.


### Editor 2.0.0, Firmware 0.2.8 (2014-09-01)
- New Features/Changes:
	- Entirely new UI & design.
	- Support for Windows 8 & Mac OS 10.9 (Mavericks).
	- New preset management features:
		- Copy / Paste preset
		- Import / Export preset
		- Save As & Delete preset
	- Enable and disable cc7 127 on preset change option.
	- Added the ability to control 12 Step LEDs from the MIDI Expander.
	- Note entry is now editable from both the full keyboard and the 12 Step keyboard layout.
	- Clearing all keys at once is now possible for note entry.
	- Presets are now numbered 1-64 instead of 0-63.
	- Added a global velocity override function that turns off velocity response for all presets.  Velocity will be a full 127 value on every note if velocity override is enabled.
- Bug Fixes:
	- USB buffer problem causing MIDI input to crash 12 Step is resolved.
	- Pitch bend can now return to true center when used with tilt source.
	- Connectivity issues between 12 Step and Windows 8 resolved.
	- Preset corrections.


12 Step Team:
Eric Bateman, Tom Ferguson, Adi Handach, Andrej Hronco, Conner Lacy, Chuck Carlson, Evan Adams, Noah Ambrose, Evan Bogunia, Emmett Corman, Diane Douglas, Tom Ferguson, Matt Hettich, David Hishinuma, Sarah Howe, Daniel McAnulty, Dennis Saputelli, Dustin Schultz, Barry Threw, Nick Wang, Carson Whitley, Steve Wright, and Keith McMillen
