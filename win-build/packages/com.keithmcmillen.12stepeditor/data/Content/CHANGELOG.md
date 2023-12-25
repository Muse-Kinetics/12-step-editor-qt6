#### 12 Step Editor and Firmware Changelog

### Editor 2.1.0, Firmware 0.3.0 (2022-09-30)
- New Features/Changes:
	- Overhauled MIDI backend using RtMidi, now builds/runs on modern operating systems.
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
Keith McMillen, Eric Bateman, Andrej Hronco, Conner Lacy, Chuck Carlson, Evan Adams, Noah Ambrose, Evan Bogunia, Emmett Corman, Diane Douglas, Tom Ferguson, Matt Hettich, David Hishinuma, Sarah Howe, Daniel McAnulty, Dennis Saputelli, Dustin Schultz, Barry Threw, Nick Wang, Carson Whitley, and Steve Wright
