12 Step firmware versions 30 (0.3.0) and below did not use a bootloader. This would occasionally allow some units to become corrupted/bricked. A bootloader protects firmware from corruption, so we have opted to upgrade all units. This process is done automatically with the new 3.0.0 editor, but can be done manually as well.

The following files are included in this package:

##########################################
12Step99-TrojanBootloader.syx
##########################################
this file contains a "trojan horse" bootloader image, disguised in the older firmware format. Sending this image to a version 30 or older 12 Steo will install the new bootloader. This change cannot be undone.

##########################################
12Step-enter-bootloader.syx 
##########################################
Once the bootloader is installed, send this file to the SoftStep to put it into bootloader mode. This is required prior to sending any of the below updates. 

##########################################
12Step_Firmware_v1.x.x.syx
##########################################
This is the latest version of the firmware provided with the editor. Firmware version 1.x.x is compatible with both 12 Step and 12 Step2 hardware, and older hardware running the new firmware will use the old USB MIDI port names ("12Step Port 1", etc). This will allow older hardware to still talk to older editors, just ignore any prompts to update firmware. New 12 Step2 hardware is only compatible with editor 3.0.0 and later, and uses USB MIDI port names "12 Step Control Surface" and "12 Step TRS MIDI Out".

Note: Windows does not properly display USB MIDI portname descriptors, and some applications will report "12 Step" for the control surface, and "MIDIIN2 (12 Step)" for the expander and/or TRS MIDI out. Microsoft should be fixing this when they roll out support for MIDI 2.0. 

Recomended SysEx software:

MacOS - SysEx Librarian: https://www.snoize.com/sysexlibrarian/

Windows - SysExBox: http://miostools.midibox.org/ 