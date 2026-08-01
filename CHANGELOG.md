#### 12 Step Editor and Firmware Changelog

### Editor 3.0.8, Firmware 1.0.9 (2026-07-31)
- New Features/Changes:
	- Migrated to Windows MIDI Services (WMS), with automatic fallback to WinMM on machines without the WMS runtime installed.
	- Firmware updates now use a packetized, chunk-safe transfer with a per-packet identity handshake instead of sending the whole firmware image as one block.
	- Devices running firmware older than 1.0.0 are no longer offered an in-editor update; the editor shows a support link instead.
	- Raised the minimum allowed key on/off thresholds (on: 15, off: 10) and the defaults (on: 20, off: 15), and the off threshold can no longer be set within 1 of the on threshold (moving either slider adjusts the other if needed). Presets saved before these limits existed are corrected to defaults on load.
	- Added a "Open Log File Location" item to the Help menu.
	- Updated firmware to 1.0.9: renames the unit's USB MIDI ports to "12Step Control Surface" and "12Step Expander" (previously unnamed "12Step"/"12Step 2" ports); adds a boot-time tare capture for the key/select sensors to reduce stuck-key false triggers; raises the on-device default on/off pressure thresholds and adds a device-side minimum gap between them. The editor recognizes units on both the old and new port-naming scheme.

### Editor 3.0.7, Firmware 1.0.7 (2025-01-22)
- Bug fixes:
	- To address issues with preset corruption, we've updated how the 12 Step display behaves when receiving presets from the editor and loading presets on the device. 
		- Context: with previous version of the 12 Step firmware, it was possible to send an incomplete and/or corrupted preset setlist. While this did not happen often, when it did happen there was no indication given to the end user. For instance, a setlist with 30 presets could be sent, but only the first 25 were successfully loaded. This would work without issue until the end user attempted to load any of the presets 26-30, in which case undefined behavior could occur. In most cases that we were able to reproduce, this would cause the unit to load factory presets, but with no indication given to the end user. To address this:
		- When the preset setlist is sent from the editor to the device, the 12 Step display will now show a counter with the number of presets received. If there is an error when the presets are sent (i.e. transmission times out, or the number of presets received doesn't match the number that were supposed to be sent), the display will show an error message "PRST / ERR / SEND / AGAN", and the device will not process any presets. If the unit is power cycled before the setlist is successfully re-sent, then the factory presets will load, and the display will show the message "FACT / PRST / LOAD / ED" until the select button is activated. 
		- When loading a preset on the device, the red LEDs next to each preset slot (numbere keys) will correctly indicate the available preset slots to load in the current "decade". For instance, if 21 presets have been loaded on the device:
			- Decade 0: slots 1-9 will blink red, 0 remains dark (presets are not zero indexed)
			- Decade 1: slots 1-9 and 0 (ie 10) will blink red
			- Decade 2: slots 0 and 1 (ie 20 and 21) will blink red, the rest remain dark and if selected will revert to the current preset
		- When loading a preset on the device, the 12 Step will briefly (1 second) show the preset number (i.e. "P 10") before showing the preset name. 
		- In the incredibly unlikely chance that presets become corrupted after the setlist has been succesfully sent and verified with the guards implemented above, the unit will display the "PRST / ERR / SEND / AGAN" message and load factory presets on the next power cycle unless the presets are re-sent first. 
		- Flash memory read/writes now suspend ADC scanning. This has a secondary affect of setting the backlight brightness to max. If the backlight is dimmed, this will give the apprearance of a flash of light when loading presets. 
		- 
	- False triggers (ghost notes)
		- Context: When firmware 1.0.4 was released to the public, the Facebook group received a post where someone was questioning the latency of the 12 Step keys. This honestly had never come up before, and upon investigation it was discovered that the firmware was weighting 8 ADC scans before activating a key. As far as we could tell this was how 12 Step had always behaved, and there was no documentation as to WHY this was in place. So we lowered this to a single ADC scan and tested the behavior, and sure enough we found that this significantly lowered the latency of the key action, and we released this update as firmware 1.0.5. 
			- While this was tested with a handful of volunteer beta testers, in hindsight we should have been more careful making this change, because it resulted in a handful of users experiencing false triggers and ghost notes caused by random noise in the ADC scan. Our best guess is that the 8 ADC scans were being used as a noise filter, where all 8 scans had to be above the on threshold before a key would be activated. 
		- The fix: We've implemented a more sophisticated hysteresis noise filter that only needs 2 ADC samples, and we've lowered the overall incidence of noise by properly synchromizing the ADC scan to the key cooking algorithm. This should prevent ghost notes and false triggers experienced by some users after updating to the previous firmware version 1.0.5.
	- Note: if the above reads more like a blog entry than a changelog update, that's intentional given the nature of this update. If you do or don't appreciate this kind of information, please feel free to reach out directly to eric (sign of the at) musekinetics (punctuation) com and I'd be happy to discuss it with you. 

### Editor 3.0.6A, Firmware 1.0.6 (2024-12-31)
- Beta, never released to the public

### Editor 3.0.5, Firmware 1.0.5 (2024-06-24)
- New Features/Changes:
	- Added global key on/off thresholds to settings tab. 
		- On threshold determines how much pressure is needed to turn a key on
			- A higher on threshold setting requires more pressure to turn a key on and reduces unintended notes
		- Off threshold determines when an active key is turned off.
			- A higher off theshold setting prevents stuck keys and allows more rapid playing
			- A lower off threshold setting prevents double triggering
	- Reduced key-on latency and improved key sensitivity, especially for black/accidental keys
	- Keys pressure now automatically scales/calibrates based on the peak key value received since power on
- Bug Fixes:
	- Fixed an over-indexing error that caused unexpected behavior under very specific conditions 

### Editor 3.0.4, Firmware 1.0.4 (2024-04-08)
- Bug Fixes:
	- Program Changes received from expander port (legact 12 Step1) now control presets
	- Pitch Bend messages sent to CV interface now properly scale with calibration values
	- Program Change RX channel now correctly updates NRPN messaging for CV calibration
	- Firmware update process throttled to 256 bytes every 4ms, bootloader locked

### Editor 3.0.3 - not released to public

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
