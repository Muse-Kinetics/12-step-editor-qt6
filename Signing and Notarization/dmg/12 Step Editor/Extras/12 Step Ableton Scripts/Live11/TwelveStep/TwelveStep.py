# -----------------------------------------------
# 
# Keith McMillen Instruments 2023      
# 
# Authors:                   
#   Evan Bogunia  
#   Sarah Howe    
#   Eric Bateman  (updated to Live 11)
# 
# -----------------------------------------------

from __future__ import with_statement

import Live  # Now the Live API is available
import time  # Now we can use time functions for time-stamping our log file outputs

from _Framework.ButtonElement import ButtonElement
from _Framework.ClipSlotComponent import ClipSlotComponent
from _Framework.ControlElement import ControlElement
from _Framework.ControlSurface import ControlSurface
from _Framework.ControlSurfaceComponent import ControlSurfaceComponent
from _Framework.InputControlElement import *
from _Framework.SceneComponent import SceneComponent
# from _Framework.SliderElement import SliderElement
from _Framework.DeviceComponent import DeviceComponent

from .TwelveStepSessionComponent import TwelveStepSessionComponent

# Define global variables
CHANNEL = 8  # channels are numbered 0 - 15
is_momentary = True

class TwelveStep(ControlSurface):
    __module__ = __name__
    __doc__ = "12Step Keyboard controller script"
  
    def __init__(self, c_instance):
        ControlSurface.__init__(self, c_instance)
        with self.component_guard():
            self._suppress_session_highlight = True
            self._suppress_send_midi = True
            self.num_tracks = 7
            self.num_scenes = 1
            self.session = None
            self._setup_session_control()
            self.set_highlighting_session_component(self.session)
            self._suppress_session_highlight = False

    def _setup_session_control(self):
        # (num_tracks, num_scenes) a session highlight ("red box") will appear with any two non-zero values
        self.session = TwelveStepSessionComponent(self.num_tracks, self.num_scenes, 0)
        self.sessionUpper = TwelveStepSessionComponent(self.num_tracks, self.num_scenes, 1)
        self.sessionLower = TwelveStepSessionComponent(self.num_tracks, self.num_scenes, 2)
        self.session.set_offsets(0, 0)
        self.sessionUpper.set_offsets(0, 0)
        self.sessionLower.set_offsets(0, 0)
        self.session.update()
        self.sessionUpper.update()
        self.sessionLower.update()
        
        self.session.set_stop_all_clips_button(ButtonElement(is_momentary, MIDI_NOTE_TYPE, CHANNEL, 58))

        # You have to create the next button before the prev button!!!
        self.session.set_select_next_button(ButtonElement(is_momentary, MIDI_NOTE_TYPE, CHANNEL, 51))
        self.session.set_select_prev_button(ButtonElement(is_momentary, MIDI_NOTE_TYPE, CHANNEL, 49))
        self.session.scene(0).set_launch_button(ButtonElement(is_momentary, MIDI_NOTE_TYPE, CHANNEL, 60))
        self.sessionUpper.scene(0).set_launch_button(ButtonElement(is_momentary, MIDI_NOTE_TYPE, CHANNEL, 56))
        self.sessionLower.scene(0).set_launch_button(ButtonElement(is_momentary, MIDI_NOTE_TYPE, CHANNEL, 54))
        
        # Here we set up the clip launch assignments for the session  
        clip_launch_notes = [48, 50, 52, 53, 55, 57, 59] 
        for index in range(self.num_tracks):
            # Step through tracks and assign a note to each clip slot in the scene
            self.session.scene(0).clip_slot(index).set_launch_button(ButtonElement(is_momentary, MIDI_NOTE_TYPE, CHANNEL, clip_launch_notes[index]))

    def disconnect(self):
        # Clean things up on disconnect
        
        # Create entry in log file
        self.log_message(time.strftime("%d.%m.%Y %H:%M:%S", time.localtime()) + "----------12Step log closed----------")
        
        ControlSurface.disconnect(self)
        return None
