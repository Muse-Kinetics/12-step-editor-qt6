from _Framework.SessionComponent import SessionComponent


class TwelveStepSessionComponent(SessionComponent):
	__module__ = __name__
  	""" Custom session component for 12Step that ties selceted scene to the redbox """

  	def __init__(self, num_tracks, num_scenes, box_number):
  		self._box_number = box_number
  		SessionComponent.__init__(self, num_tracks, num_scenes)
  		
  	def update(self):
  		SessionComponent.update(self)
  		self._reselect_track()
  		self._reselect_scene()

  	""" Because update doesn't get called in Live 9 when we change offsets 
      	We have to manually call reselect_track whenever we bank
  	"""
  	def set_offsets(self, track_offset, scene_offset):
  		SessionComponent.set_offsets(self, track_offset, scene_offset)
  		self._reselect_track()
  	
  	def _reselect_track(self):
  		tracks_to_use = self.tracks_to_use()
  		track = tracks_to_use[self._track_offset] 
  		self.song().view.selected_track = track
  	
  	def on_selected_scene_changed(self):
  		SessionComponent.on_selected_scene_changed(self)
  		self._reselect_scene()
  		
  	def _reselect_scene(self):
  		box_num = self._box_number
  		selected_scene = self.song().view.selected_scene
  		all_scenes = self.song().scenes
  		sceneIndex = list(all_scenes).index(selected_scene)
  		self.set_offsets(0, sceneIndex)
  		if (box_num == 0):
  			self.set_offsets(0, sceneIndex)
  		elif (box_num == 1):
  			self.set_offsets(0, sceneIndex + 1)
  		elif (box_num == 2):
  			if sceneIndex > 0:
  				self.set_offsets(0, sceneIndex - 1)
  			else: 
  				self.set_offsets(0, sceneIndex)