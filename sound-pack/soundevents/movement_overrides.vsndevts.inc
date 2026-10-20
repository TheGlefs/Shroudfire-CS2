// Full stock definitions are required: abbreviated duplicate events are discarded by
// CS2's compiler. The only intentional change is volume = 0.
// CT footsteps are role-specific because Invisants use CT. Landing and jumping are
// shared client-predicted events, so they are silent for both roles.

	CT_Concrete.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		pitch_random_max = 0.0
		pitch_random_min = 0.0
		pitch = 1.0
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		volume = 0
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 1.0
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/concrete_ct_01.vsnd",
			"sounds/player/footsteps/concrete_ct_02.vsnd",
			"sounds/player/footsteps/concrete_ct_03.vsnd",
			"sounds/player/footsteps/concrete_ct_04.vsnd",
			"sounds/player/footsteps/concrete_ct_05.vsnd",
			"sounds/player/footsteps/concrete_ct_06.vsnd",
			"sounds/player/footsteps/concrete_ct_07.vsnd",
			"sounds/player/footsteps/concrete_ct_08.vsnd",
			"sounds/player/footsteps/concrete_ct_09.vsnd",
			"sounds/player/footsteps/concrete_ct_10.vsnd",
			"sounds/player/footsteps/concrete_ct_11.vsnd",
			"sounds/player/footsteps/concrete_ct_12.vsnd",
			"sounds/player/footsteps/concrete_ct_13.vsnd",
			"sounds/player/footsteps/concrete_ct_14.vsnd",
			"sounds/player/footsteps/concrete_ct_15.vsnd",
			"sounds/player/footsteps/concrete_ct_16.vsnd",
			"sounds/player/footsteps/concrete_ct_17.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Ceiling_Tile.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		volume = 0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/tile_01.vsnd",
			"sounds/player/footsteps/tile_02.vsnd",
			"sounds/player/footsteps/tile_03.vsnd",
			"sounds/player/footsteps/tile_04.vsnd",
			"sounds/player/footsteps/tile_05.vsnd",
			"sounds/player/footsteps/tile_06.vsnd",
			"sounds/player/footsteps/tile_07.vsnd",
			"sounds/player/footsteps/tile_08.vsnd",
			"sounds/player/footsteps/tile_09.vsnd",
			"sounds/player/footsteps/tile_10.vsnd",
			"sounds/player/footsteps/tile_11.vsnd",
			"sounds/player/footsteps/tile_12.vsnd",
			"sounds/player/footsteps/tile_13.vsnd",
			"sounds/player/footsteps/tile_14.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Default.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = -0.1
		volume_random_max = 0.0
		volume = 0
		pitch = 1.0
		pitch_random_min = 0.0
		pitch_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 1.0
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/concrete_ct_01.vsnd",
			"sounds/player/footsteps/concrete_ct_02.vsnd",
			"sounds/player/footsteps/concrete_ct_03.vsnd",
			"sounds/player/footsteps/concrete_ct_04.vsnd",
			"sounds/player/footsteps/concrete_ct_05.vsnd",
			"sounds/player/footsteps/concrete_ct_06.vsnd",
			"sounds/player/footsteps/concrete_ct_07.vsnd",
			"sounds/player/footsteps/concrete_ct_08.vsnd",
			"sounds/player/footsteps/concrete_ct_09.vsnd",
			"sounds/player/footsteps/concrete_ct_10.vsnd",
			"sounds/player/footsteps/concrete_ct_11.vsnd",
			"sounds/player/footsteps/concrete_ct_12.vsnd",
			"sounds/player/footsteps/concrete_ct_13.vsnd",
			"sounds/player/footsteps/concrete_ct_14.vsnd",
			"sounds/player/footsteps/concrete_ct_15.vsnd",
			"sounds/player/footsteps/concrete_ct_16.vsnd",
			"sounds/player/footsteps/concrete_ct_17.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Wood.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Wood_Solid.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_SolidMetal.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_solid_35.vsnd",
			"sounds/player/footsteps/metal_solid_36.vsnd",
			"sounds/player/footsteps/metal_solid_37.vsnd",
			"sounds/player/footsteps/metal_solid_38.vsnd",
			"sounds/player/footsteps/metal_solid_39.vsnd",
			"sounds/player/footsteps/metal_solid_40.vsnd",
			"sounds/player/footsteps/metal_solid_41.vsnd",
			"sounds/player/footsteps/metal_solid_42.vsnd",
			"sounds/player/footsteps/metal_solid_43.vsnd",
			"sounds/player/footsteps/metal_solid_44.vsnd",
			"sounds/player/footsteps/metal_solid_45.vsnd",
			"sounds/player/footsteps/metal_solid_46.vsnd",
			"sounds/player/footsteps/metal_solid_47.vsnd",
			"sounds/player/footsteps/metal_solid_48.vsnd",
			"sounds/player/footsteps/metal_solid_49.vsnd",
			"sounds/player/footsteps/metal_solid_50.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_ChainLink.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		volume = 0
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_chainlink_01.vsnd",
			"sounds/player/footsteps/metal_chainlink_02.vsnd",
			"sounds/player/footsteps/metal_chainlink_03.vsnd",
			"sounds/player/footsteps/metal_chainlink_04.vsnd",
			"sounds/player/footsteps/metal_chainlink_05.vsnd",
			"sounds/player/footsteps/metal_chainlink_06.vsnd",
			"sounds/player/footsteps/metal_chainlink_07.vsnd",
			"sounds/player/footsteps/metal_chainlink_09.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_MetalGrate.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_grate_01.vsnd",
			"sounds/player/footsteps/metal_grate_02.vsnd",
			"sounds/player/footsteps/metal_grate_03.vsnd",
			"sounds/player/footsteps/metal_grate_04.vsnd",
			"sounds/player/footsteps/metal_grate_05.vsnd",
			"sounds/player/footsteps/metal_grate_06.vsnd",
			"sounds/player/footsteps/metal_grate_07.vsnd",
			"sounds/player/footsteps/metal_grate_08.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Ladder.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.4
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.8
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/ladder_01.vsnd",
			"sounds/player/footsteps/ladder_02.vsnd",
			"sounds/player/footsteps/ladder_03.vsnd",
			"sounds/player/footsteps/ladder_04.vsnd",
			"sounds/player/footsteps/ladder_05.vsnd",
			"sounds/player/footsteps/ladder_06.vsnd",
			"sounds/player/footsteps/ladder_08.vsnd",
			"sounds/player/footsteps/ladder_09.vsnd",
			"sounds/player/footsteps/ladder_10.vsnd",
			"sounds/player/footsteps/ladder_11.vsnd",
			"sounds/player/footsteps/ladder_12.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_MetalVent.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 0.3
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.97
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_auto_01.vsnd",
			"sounds/player/footsteps/metal_auto_02.vsnd",
			"sounds/player/footsteps/metal_auto_03.vsnd",
			"sounds/player/footsteps/metal_auto_04.vsnd",
			"sounds/player/footsteps/metal_auto_05.vsnd",
			"sounds/player/footsteps/metal_auto_06.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_MetalVehicle.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_auto_01.vsnd",
			"sounds/player/footsteps/metal_auto_02.vsnd",
			"sounds/player/footsteps/metal_auto_03.vsnd",
			"sounds/player/footsteps/metal_auto_04.vsnd",
			"sounds/player/footsteps/metal_auto_05.vsnd",
			"sounds/player/footsteps/metal_auto_06.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_MetalBarrel.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_vent_01.vsnd",
			"sounds/player/footsteps/metal_vent_02.vsnd",
			"sounds/player/footsteps/metal_vent_03.vsnd",
			"sounds/player/footsteps/metal_vent_04.vsnd",
			"sounds/player/footsteps/metal_vent_05.vsnd",
			"sounds/player/footsteps/metal_vent_06.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Snow.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		use_time_volume_mapping_curve = false
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/snow_01.vsnd",
			"sounds/player/footsteps/snow_02.vsnd",
			"sounds/player/footsteps/snow_03.vsnd",
			"sounds/player/footsteps/snow_04.vsnd",
			"sounds/player/footsteps/snow_05.vsnd",
			"sounds/player/footsteps/snow_06.vsnd",
			"sounds/player/footsteps/snow_07.vsnd",
			"sounds/player/footsteps/snow_08.vsnd",
			"sounds/player/footsteps/snow_09.vsnd",
			"sounds/player/footsteps/snow_10.vsnd",
			"sounds/player/footsteps/snow_11.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		time_volume_mapping_curve = 
		[
			[
				0.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				0.162857, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Dirt.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 1.0
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/dirt_01.vsnd",
			"sounds/player/footsteps/dirt_02.vsnd",
			"sounds/player/footsteps/dirt_03.vsnd",
			"sounds/player/footsteps/dirt_04.vsnd",
			"sounds/player/footsteps/dirt_05.vsnd",
			"sounds/player/footsteps/dirt_06.vsnd",
			"sounds/player/footsteps/dirt_07.vsnd",
			"sounds/player/footsteps/dirt_08.vsnd",
			"sounds/player/footsteps/dirt_09.vsnd",
			"sounds/player/footsteps/dirt_10.vsnd",
			"sounds/player/footsteps/dirt_11.vsnd",
			"sounds/player/footsteps/dirt_12.vsnd",
			"sounds/player/footsteps/dirt_13.vsnd",
			"sounds/player/footsteps/dirt_14.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Mud.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 12.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.5, 0.007466, 0.007466,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/mud_01.vsnd",
			"sounds/player/footsteps/mud_02.vsnd",
			"sounds/player/footsteps/mud_03.vsnd",
			"sounds/player/footsteps/mud_04.vsnd",
			"sounds/player/footsteps/mud_05.vsnd",
			"sounds/player/footsteps/mud_06.vsnd",
			"sounds/player/footsteps/mud_07.vsnd",
			"sounds/player/footsteps/mud_08.vsnd",
			"sounds/player/footsteps/mud_09.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Slime.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.5, 0.007466, 0.007466,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/mud_01.vsnd",
			"sounds/player/footsteps/mud_02.vsnd",
			"sounds/player/footsteps/mud_03.vsnd",
			"sounds/player/footsteps/mud_04.vsnd",
			"sounds/player/footsteps/mud_05.vsnd",
			"sounds/player/footsteps/mud_06.vsnd",
			"sounds/player/footsteps/mud_07.vsnd",
			"sounds/player/footsteps/mud_08.vsnd",
			"sounds/player/footsteps/mud_09.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Grass.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		volume = 0
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		pitch = 1.0
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/grass_01.vsnd",
			"sounds/player/footsteps/grass_02.vsnd",
			"sounds/player/footsteps/grass_03.vsnd",
			"sounds/player/footsteps/grass_04.vsnd",
			"sounds/player/footsteps/grass_05.vsnd",
			"sounds/player/footsteps/grass_06.vsnd",
			"sounds/player/footsteps/grass_07.vsnd",
		]
		metadata = "quietfs 600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Tile.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/tile_01.vsnd",
			"sounds/player/footsteps/tile_02.vsnd",
			"sounds/player/footsteps/tile_03.vsnd",
			"sounds/player/footsteps/tile_04.vsnd",
			"sounds/player/footsteps/tile_05.vsnd",
			"sounds/player/footsteps/tile_06.vsnd",
			"sounds/player/footsteps/tile_07.vsnd",
			"sounds/player/footsteps/tile_08.vsnd",
			"sounds/player/footsteps/tile_09.vsnd",
			"sounds/player/footsteps/tile_10.vsnd",
			"sounds/player/footsteps/tile_11.vsnd",
			"sounds/player/footsteps/tile_12.vsnd",
			"sounds/player/footsteps/tile_13.vsnd",
			"sounds/player/footsteps/tile_14.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Water.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 3.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/puddle_01.vsnd",
			"sounds/player/footsteps/puddle_02.vsnd",
			"sounds/player/footsteps/puddle_03.vsnd",
			"sounds/player/footsteps/puddle_04.vsnd",
			"sounds/player/footsteps/puddle_05.vsnd",
			"sounds/player/footsteps/puddle_06.vsnd",
			"sounds/player/footsteps/puddle_07.vsnd",
			"sounds/player/footsteps/puddle_08.vsnd",
			"sounds/player/footsteps/puddle_09.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				68.285713, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				150.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Wood_Basket.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Glass.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/glass_01.vsnd",
			"sounds/player/footsteps/glass_07.vsnd",
			"sounds/player/footsteps/glass_08.vsnd",
			"sounds/player/footsteps/glass_09.vsnd",
			"sounds/player/footsteps/glass_10.vsnd",
			"sounds/player/footsteps/glass_11.vsnd",
			"sounds/player/footsteps/glass_12.vsnd",
			"sounds/player/footsteps/glass_13.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Gravel.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.07
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/gravel_01.vsnd",
			"sounds/player/footsteps/gravel_02.vsnd",
			"sounds/player/footsteps/gravel_03.vsnd",
			"sounds/player/footsteps/gravel_04.vsnd",
			"sounds/player/footsteps/gravel_05.vsnd",
			"sounds/player/footsteps/gravel_06.vsnd",
			"sounds/player/footsteps/gravel_07.vsnd",
			"sounds/player/footsteps/gravel_08.vsnd",
			"sounds/player/footsteps/gravel_09.vsnd",
			"sounds/player/footsteps/gravel_10.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Flesh.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/concrete_ct_01.vsnd",
			"sounds/player/footsteps/concrete_ct_02.vsnd",
			"sounds/player/footsteps/concrete_ct_03.vsnd",
			"sounds/player/footsteps/concrete_ct_04.vsnd",
			"sounds/player/footsteps/concrete_ct_05.vsnd",
			"sounds/player/footsteps/concrete_ct_06.vsnd",
			"sounds/player/footsteps/concrete_ct_07.vsnd",
			"sounds/player/footsteps/concrete_ct_08.vsnd",
			"sounds/player/footsteps/concrete_ct_09.vsnd",
			"sounds/player/footsteps/concrete_ct_10.vsnd",
			"sounds/player/footsteps/concrete_ct_11.vsnd",
			"sounds/player/footsteps/concrete_ct_12.vsnd",
			"sounds/player/footsteps/concrete_ct_13.vsnd",
			"sounds/player/footsteps/concrete_ct_14.vsnd",
			"sounds/player/footsteps/concrete_ct_15.vsnd",
			"sounds/player/footsteps/concrete_ct_16.vsnd",
			"sounds/player/footsteps/concrete_ct_17.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Sand.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 1.0
		pitch_random_min = -0.07
		pitch_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/sand_01.vsnd",
			"sounds/player/footsteps/sand_02.vsnd",
			"sounds/player/footsteps/sand_03.vsnd",
			"sounds/player/footsteps/sand_04.vsnd",
			"sounds/player/footsteps/sand_05.vsnd",
			"sounds/player/footsteps/sand_06.vsnd",
			"sounds/player/footsteps/sand_07.vsnd",
			"sounds/player/footsteps/sand_08.vsnd",
			"sounds/player/footsteps/sand_09.vsnd",
			"sounds/player/footsteps/sand_10.vsnd",
			"sounds/player/footsteps/sand_11.vsnd",
			"sounds/player/footsteps/sand_12.vsnd",
		]
		metadata = "quietfs 600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_GlassBottle.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/physics/glass/glass_bottle_impact_hard1.vsnd",
			"sounds/physics/glass/glass_bottle_impact_hard2.vsnd",
			"sounds/physics/glass/glass_bottle_impact_hard3.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Drywall.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 1.0
		pitch_random_min = 0.0
		pitch_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/concrete_ct_01.vsnd",
			"sounds/player/footsteps/concrete_ct_02.vsnd",
			"sounds/player/footsteps/concrete_ct_03.vsnd",
			"sounds/player/footsteps/concrete_ct_04.vsnd",
			"sounds/player/footsteps/concrete_ct_05.vsnd",
			"sounds/player/footsteps/concrete_ct_06.vsnd",
			"sounds/player/footsteps/concrete_ct_07.vsnd",
			"sounds/player/footsteps/concrete_ct_08.vsnd",
			"sounds/player/footsteps/concrete_ct_09.vsnd",
			"sounds/player/footsteps/concrete_ct_10.vsnd",
			"sounds/player/footsteps/concrete_ct_11.vsnd",
			"sounds/player/footsteps/concrete_ct_12.vsnd",
			"sounds/player/footsteps/concrete_ct_13.vsnd",
			"sounds/player/footsteps/concrete_ct_14.vsnd",
			"sounds/player/footsteps/concrete_ct_15.vsnd",
			"sounds/player/footsteps/concrete_ct_16.vsnd",
			"sounds/player/footsteps/concrete_ct_17.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Cardboard.StepLeft = 
	{
		base = "Base.Footstep"
		block_match_entity = true
		block_matching_events = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		volume = 0
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.5, 0.007466, 0.007466,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/physics/cardboard/cardboard_box_impact_soft1.vsnd",
			"sounds/physics/cardboard/cardboard_box_impact_soft2.vsnd",
			"sounds/physics/cardboard/cardboard_box_impact_soft3.vsnd",
			"sounds/physics/cardboard/cardboard_box_impact_soft4.vsnd",
			"sounds/physics/cardboard/cardboard_box_impact_soft5.vsnd",
			"sounds/physics/cardboard/cardboard_box_impact_soft6.vsnd",
			"sounds/physics/cardboard/cardboard_box_impact_soft7.vsnd",
		]
		metadata = "quietfs 600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Plastic_Barrel.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.4, 0.008959, 0.008959,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/plastic_barrel_01.vsnd",
			"sounds/player/footsteps/plastic_barrel_02.vsnd",
			"sounds/player/footsteps/plastic_barrel_03.vsnd",
			"sounds/player/footsteps/plastic_barrel_04.vsnd",
			"sounds/player/footsteps/plastic_barrel_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Plastic_Box.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.4, 0.008959, 0.008959,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/plastic_barrel_01.vsnd",
			"sounds/player/footsteps/plastic_barrel_02.vsnd",
			"sounds/player/footsteps/plastic_barrel_03.vsnd",
			"sounds/player/footsteps/plastic_barrel_04.vsnd",
			"sounds/player/footsteps/plastic_barrel_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Rubber.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/rubber_03.vsnd",
			"sounds/player/footsteps/rubber_04.vsnd",
			"sounds/player/footsteps/rubber_05.vsnd",
			"sounds/player/footsteps/rubber_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Grenade.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/concrete_ct_01.vsnd",
			"sounds/player/footsteps/concrete_ct_02.vsnd",
			"sounds/player/footsteps/concrete_ct_03.vsnd",
			"sounds/player/footsteps/concrete_ct_04.vsnd",
			"sounds/player/footsteps/concrete_ct_05.vsnd",
			"sounds/player/footsteps/concrete_ct_06.vsnd",
			"sounds/player/footsteps/concrete_ct_07.vsnd",
			"sounds/player/footsteps/concrete_ct_08.vsnd",
			"sounds/player/footsteps/concrete_ct_09.vsnd",
			"sounds/player/footsteps/concrete_ct_10.vsnd",
			"sounds/player/footsteps/concrete_ct_11.vsnd",
			"sounds/player/footsteps/concrete_ct_12.vsnd",
			"sounds/player/footsteps/concrete_ct_13.vsnd",
			"sounds/player/footsteps/concrete_ct_14.vsnd",
			"sounds/player/footsteps/concrete_ct_15.vsnd",
			"sounds/player/footsteps/concrete_ct_16.vsnd",
			"sounds/player/footsteps/concrete_ct_17.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_WoodBox.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Wood_Crate.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Wood_Panel.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Weapon.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/physics/weapons/weapon_smg_impact_13.vsnd",
			"sounds/physics/weapons/weapon_smg_impact_16.vsnd",
			"sounds/physics/weapons/weapon_smg_impact_18.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.022076, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				58.948574, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Foliage.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		volume = 0
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		pitch = 1.0
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/grass_01.vsnd",
			"sounds/player/footsteps/grass_02.vsnd",
			"sounds/player/footsteps/grass_03.vsnd",
			"sounds/player/footsteps/grass_04.vsnd",
			"sounds/player/footsteps/grass_05.vsnd",
			"sounds/player/footsteps/grass_06.vsnd",
			"sounds/player/footsteps/grass_07.vsnd",
			"sounds/player/footsteps/grass_08.vsnd",
			"sounds/player/footsteps/grass_09.vsnd",
			"sounds/player/footsteps/grass_10.vsnd",
			"sounds/player/footsteps/grass_11.vsnd",
			"sounds/player/footsteps/grass_12.vsnd",
			"sounds/player/footsteps/grass_13.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Carpet.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		volume = 0
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.5, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/carpet_01.vsnd",
			"sounds/player/footsteps/carpet_03.vsnd",
			"sounds/player/footsteps/carpet_04.vsnd",
			"sounds/player/footsteps/carpet_08.vsnd",
			"sounds/player/footsteps/carpet_09.vsnd",
			"sounds/player/footsteps/carpet_11.vsnd",
			"sounds/player/footsteps/carpet_14.vsnd",
			"sounds/player/footsteps/carpet_15.vsnd",
			"sounds/player/footsteps/carpet_17.vsnd",
			"sounds/player/footsteps/carpet_18.vsnd",
			"sounds/player/footsteps/carpet_19.vsnd",
			"sounds/player/footsteps/carpet_20.vsnd",
		]
		metadata = "quietfs 600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_SlipperySlime.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.5, 0.007466, 0.007466,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/mud_01.vsnd",
			"sounds/player/footsteps/mud_02.vsnd",
			"sounds/player/footsteps/mud_03.vsnd",
			"sounds/player/footsteps/mud_04.vsnd",
			"sounds/player/footsteps/mud_05.vsnd",
			"sounds/player/footsteps/mud_06.vsnd",
			"sounds/player/footsteps/mud_07.vsnd",
			"sounds/player/footsteps/mud_08.vsnd",
			"sounds/player/footsteps/mud_09.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Silent.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "silentfs"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_WetTile.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = -0.1
		volume_random_max = 0.0
		volume = 0
		pitch = 1.1
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = true
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		set_child_position = true
		use_baked_occlusion = true
		soundevent_01 = "WetTile.Splash"
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/tile_01.vsnd",
			"sounds/player/footsteps/tile_02.vsnd",
			"sounds/player/footsteps/tile_03.vsnd",
			"sounds/player/footsteps/tile_04.vsnd",
			"sounds/player/footsteps/tile_05.vsnd",
			"sounds/player/footsteps/tile_06.vsnd",
			"sounds/player/footsteps/tile_07.vsnd",
			"sounds/player/footsteps/tile_08.vsnd",
			"sounds/player/footsteps/tile_09.vsnd",
			"sounds/player/footsteps/tile_10.vsnd",
			"sounds/player/footsteps/tile_11.vsnd",
			"sounds/player/footsteps/tile_12.vsnd",
			"sounds/player/footsteps/tile_13.vsnd",
			"sounds/player/footsteps/tile_14.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.022076, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				58.948574, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Wood_Ladder.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/ladder_wood_01.vsnd",
			"sounds/player/footsteps/ladder_wood_02.vsnd",
			"sounds/player/footsteps/ladder_wood_03.vsnd",
			"sounds/player/footsteps/ladder_wood_04.vsnd",
			"sounds/player/footsteps/ladder_wood_05.vsnd",
			"sounds/player/footsteps/ladder_wood_06.vsnd",
			"sounds/player/footsteps/ladder_wood_07.vsnd",
			"sounds/player/footsteps/ladder_wood_08.vsnd",
			"sounds/player/footsteps/ladder_wood_09.vsnd",
			"sounds/player/footsteps/ladder_wood_10.vsnd",
			"sounds/player/footsteps/ladder_wood_11.vsnd",
			"sounds/player/footsteps/ladder_wood_12.vsnd",
			"sounds/player/footsteps/ladder_wood_13.vsnd",
			"sounds/player/footsteps/ladder_wood_14.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Wet_Sand.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		pitch_random_min = -0.07
		pitch_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		volume = 0
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		pitch = 1.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/puddle_small_01.vsnd",
			"sounds/player/footsteps/puddle_small_02.vsnd",
			"sounds/player/footsteps/puddle_small_03.vsnd",
			"sounds/player/footsteps/puddle_small_04.vsnd",
			"sounds/player/footsteps/puddle_small_05.vsnd",
			"sounds/player/footsteps/puddle_small_06.vsnd",
			"sounds/player/footsteps/puddle_small_07.vsnd",
		]
		metadata = "quietfs 600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Plastic_milkCrate.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/rubber_03.vsnd",
			"sounds/player/footsteps/rubber_04.vsnd",
			"sounds/player/footsteps/rubber_05.vsnd",
			"sounds/player/footsteps/rubber_06.vsnd",
			"sounds/player/footsteps/rubber_07.vsnd",
			"sounds/player/footsteps/rubber_08.vsnd",
			"sounds/player/footsteps/rubber_11.vsnd",
			"sounds/player/footsteps/rubber_12.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_Plastic_autoCover.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 0.98
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_auto_01.vsnd",
			"sounds/player/footsteps/metal_auto_02.vsnd",
			"sounds/player/footsteps/metal_auto_03.vsnd",
			"sounds/player/footsteps/metal_auto_04.vsnd",
			"sounds/player/footsteps/metal_auto_05.vsnd",
			"sounds/player/footsteps/metal_auto_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_MetalRailing.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		volume = 0
		pitch = 1.0
		pitch_random_min = -0.02
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = false
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.35
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		soundevent_01 = ""
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.3, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_railing_a_02.vsnd",
			"sounds/player/footsteps/metal_railing_a_03.vsnd",
			"sounds/player/footsteps/metal_railing_a_05.vsnd",
			"sounds/player/footsteps/metal_railing_a_06.vsnd",
			"sounds/player/footsteps/metal_railing_a_07.vsnd",
			"sounds/player/footsteps/metal_railing_a_08.vsnd",
			"sounds/player/footsteps/metal_railing_a_09.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				60.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_WetConcrete.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = -0.1
		volume_random_max = 0.0
		volume = 0
		pitch = 1.1
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = true
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		set_child_position = true
		use_baked_occlusion = true
		soundevent_01 = "WetTile.Splash"
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/concrete_ct_01.vsnd",
			"sounds/player/footsteps/concrete_ct_02.vsnd",
			"sounds/player/footsteps/concrete_ct_03.vsnd",
			"sounds/player/footsteps/concrete_ct_04.vsnd",
			"sounds/player/footsteps/concrete_ct_05.vsnd",
			"sounds/player/footsteps/concrete_ct_06.vsnd",
			"sounds/player/footsteps/concrete_ct_07.vsnd",
			"sounds/player/footsteps/concrete_ct_08.vsnd",
			"sounds/player/footsteps/concrete_ct_09.vsnd",
			"sounds/player/footsteps/concrete_ct_10.vsnd",
			"sounds/player/footsteps/concrete_ct_11.vsnd",
			"sounds/player/footsteps/concrete_ct_12.vsnd",
			"sounds/player/footsteps/concrete_ct_13.vsnd",
			"sounds/player/footsteps/concrete_ct_14.vsnd",
			"sounds/player/footsteps/concrete_ct_15.vsnd",
			"sounds/player/footsteps/concrete_ct_16.vsnd",
			"sounds/player/footsteps/concrete_ct_17.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.022076, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				58.948574, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	CT_WetMud.StepLeft = 
	{
		base = "Base.Footstep"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = -0.1
		volume_random_max = 0.0
		volume = 0
		pitch = 1.1
		pitch_random_min = -0.07
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		block_distance = 0.0
		enable_child_events = true
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		use_distance_unfiltered_stereo_mapping_curve = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		set_child_position = true
		use_baked_occlusion = true
		soundevent_01 = "WetTile.Splash"
		distance_volume_mapping_curve = 
		[
			[
				49.591427, 0.45, 0.010452, 0.010452,
				3.0, 1.0,
			],
			[
				116.563492, 1.0, -0.001429, -0.001429,
				2.0, 3.0,
			],
			[
				402.285736, 0.488971, -0.000991, -0.000991,
				3.0, 1.0,
			],
			[
				1095.0, 0.03, -0.000701, -0.000701,
				1.0, 1.0,
			],
			[
				1100.0, 0.0, -0.000476, -0.000476,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/mud_01.vsnd",
			"sounds/player/footsteps/mud_02.vsnd",
			"sounds/player/footsteps/mud_03.vsnd",
			"sounds/player/footsteps/mud_04.vsnd",
			"sounds/player/footsteps/mud_05.vsnd",
			"sounds/player/footsteps/mud_06.vsnd",
			"sounds/player/footsteps/mud_07.vsnd",
			"sounds/player/footsteps/mud_08.vsnd",
			"sounds/player/footsteps/mud_09.vsnd",
		]
		metadata = "loudfs 1600"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				50.022076, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				58.948574, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Concrete.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		pitch = 1.06
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/land2.vsnd",
			"sounds/player/land3.vsnd",
			"sounds/player/land4.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Ceiling_Tile.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_tile_01.vsnd",
			"sounds/player/footsteps/land_tile_02.vsnd",
			"sounds/player/footsteps/land_tile_03.vsnd",
			"sounds/player/footsteps/land_tile_04.vsnd",
			"sounds/player/footsteps/land_tile_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Default.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		enable_child_events = true
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/land.vsnd",
			"sounds/player/land2.vsnd",
			"sounds/player/land3.vsnd",
			"sounds/player/land4.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		soundevent_01 = "Land.Thud"
	}

	Land_Wood.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Wood_Solid.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_SolidMetal.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_metal_solid_01.vsnd",
			"sounds/player/footsteps/land_metal_solid_02.vsnd",
			"sounds/player/footsteps/land_metal_solid_03.vsnd",
			"sounds/player/footsteps/land_metal_solid_04.vsnd",
			"sounds/player/footsteps/land_metal_solid_05.vsnd",
			"sounds/player/footsteps/land_metal_solid_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_ChainLink.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/metal_chainlink_01.vsnd",
			"sounds/player/footsteps/metal_chainlink_02.vsnd",
			"sounds/player/footsteps/metal_chainlink_03.vsnd",
			"sounds/player/footsteps/metal_chainlink_04.vsnd",
			"sounds/player/footsteps/metal_chainlink_05.vsnd",
			"sounds/player/footsteps/metal_chainlink_06.vsnd",
			"sounds/player/footsteps/metal_chainlink_07.vsnd",
			"sounds/player/footsteps/metal_chainlink_09.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_MetalGrate.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_metal_grate_01.vsnd",
			"sounds/player/footsteps/land_metal_grate_02.vsnd",
			"sounds/player/footsteps/land_metal_grate_03.vsnd",
			"sounds/player/footsteps/land_metal_grate_04.vsnd",
			"sounds/player/footsteps/land_metal_grate_05.vsnd",
			"sounds/player/footsteps/land_metal_grate_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Ladder.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.8
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/ladder_01.vsnd",
			"sounds/player/footsteps/ladder_02.vsnd",
			"sounds/player/footsteps/ladder_03.vsnd",
			"sounds/player/footsteps/ladder_04.vsnd",
			"sounds/player/footsteps/ladder_05.vsnd",
			"sounds/player/footsteps/ladder_06.vsnd",
			"sounds/player/footsteps/ladder_08.vsnd",
			"sounds/player/footsteps/ladder_09.vsnd",
			"sounds/player/footsteps/ladder_10.vsnd",
			"sounds/player/footsteps/ladder_11.vsnd",
			"sounds/player/footsteps/ladder_12.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_MetalVent.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 0.3
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.4
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_metal_vent_01.vsnd",
			"sounds/player/footsteps/land_metal_vent_02.vsnd",
			"sounds/player/footsteps/land_metal_vent_03.vsnd",
			"sounds/player/footsteps/land_metal_vent_04.vsnd",
			"sounds/player/footsteps/land_metal_vent_05.vsnd",
			"sounds/player/footsteps/land_metal_vent_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_MetalVehicle.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_auto_01.vsnd",
			"sounds/player/footsteps/land_auto_02.vsnd",
			"sounds/player/footsteps/land_auto_03.vsnd",
			"sounds/player/footsteps/land_auto_04.vsnd",
			"sounds/player/footsteps/land_auto_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_MetalBarrel.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_metal_barrel_01.vsnd",
			"sounds/player/footsteps/land_metal_barrel_02.vsnd",
			"sounds/player/footsteps/land_metal_barrel_03.vsnd",
			"sounds/player/footsteps/land_metal_barrel_04.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Snow.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_snow_01.vsnd",
			"sounds/player/footsteps/land_snow_02.vsnd",
			"sounds/player/footsteps/land_snow_03.vsnd",
			"sounds/player/footsteps/land_snow_04.vsnd",
			"sounds/player/footsteps/land_snow_05.vsnd",
			"sounds/player/footsteps/land_snow_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Dirt.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_dirt_01.vsnd",
			"sounds/player/footsteps/land_dirt_02.vsnd",
			"sounds/player/footsteps/land_dirt_03.vsnd",
			"sounds/player/footsteps/land_dirt_04.vsnd",
			"sounds/player/footsteps/land_dirt_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Mud.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_mud_01.vsnd",
			"sounds/player/footsteps/land_mud_02.vsnd",
			"sounds/player/footsteps/land_mud_03.vsnd",
			"sounds/player/footsteps/land_mud_04.vsnd",
			"sounds/player/footsteps/land_mud_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Slime.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_mud_01.vsnd",
			"sounds/player/footsteps/land_mud_02.vsnd",
			"sounds/player/footsteps/land_mud_03.vsnd",
			"sounds/player/footsteps/land_mud_04.vsnd",
			"sounds/player/footsteps/land_mud_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Grass.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_grass_01.vsnd",
			"sounds/player/footsteps/land_grass_02.vsnd",
			"sounds/player/footsteps/land_grass_03.vsnd",
			"sounds/player/footsteps/land_grass_04.vsnd",
			"sounds/player/footsteps/land_grass_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "quietfs 600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Tile.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_tile_01.vsnd",
			"sounds/player/footsteps/land_tile_02.vsnd",
			"sounds/player/footsteps/land_tile_03.vsnd",
			"sounds/player/footsteps/land_tile_04.vsnd",
			"sounds/player/footsteps/land_tile_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Wood_Basket.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		pitch = 1.06
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_basket_01.vsnd",
			"sounds/player/footsteps/land_basket_02.vsnd",
			"sounds/player/footsteps/land_basket_03.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Glass.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_glass_01.vsnd",
			"sounds/player/footsteps/land_glass_02.vsnd",
			"sounds/player/footsteps/land_glass_03.vsnd",
			"sounds/player/footsteps/land_glass_04.vsnd",
			"sounds/player/footsteps/land_glass_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Gravel.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_gravel_01.vsnd",
			"sounds/player/footsteps/land_gravel_02.vsnd",
			"sounds/player/footsteps/land_gravel_03.vsnd",
			"sounds/player/footsteps/land_gravel_04.vsnd",
			"sounds/player/footsteps/land_gravel_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Flesh.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_body_01.vsnd",
			"sounds/player/footsteps/land_body_02.vsnd",
			"sounds/player/footsteps/land_body_03.vsnd",
			"sounds/player/footsteps/land_body_04.vsnd",
			"sounds/player/footsteps/land_body_05.vsnd",
			"sounds/player/footsteps/land_body_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Heavy.Step"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Sand.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_sand_01.vsnd",
			"sounds/player/footsteps/land_sand_02.vsnd",
			"sounds/player/footsteps/land_sand_03.vsnd",
			"sounds/player/footsteps/land_sand_04.vsnd",
			"sounds/player/footsteps/land_sand_05.vsnd",
			"sounds/player/footsteps/land_sand_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "quietfs 600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_GlassBottle.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/physics/glass/glass_bottle_impact_hard1.vsnd",
			"sounds/physics/glass/glass_bottle_impact_hard2.vsnd",
			"sounds/physics/glass/glass_bottle_impact_hard3.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Drywall.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/land2.vsnd",
			"sounds/player/land3.vsnd",
			"sounds/player/land4.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Cardboard.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_cardboard_01.vsnd",
			"sounds/player/footsteps/land_cardboard_02.vsnd",
			"sounds/player/footsteps/land_cardboard_03.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "quietfs 600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Plastic_Box.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_auto_01.vsnd",
			"sounds/player/footsteps/land_auto_02.vsnd",
			"sounds/player/footsteps/land_auto_03.vsnd",
			"sounds/player/footsteps/land_auto_04.vsnd",
			"sounds/player/footsteps/land_auto_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Rubber.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_rubber_01.vsnd",
			"sounds/player/footsteps/land_rubber_02.vsnd",
			"sounds/player/footsteps/land_rubber_03.vsnd",
			"sounds/player/footsteps/land_rubber_04.vsnd",
			"sounds/player/footsteps/land_rubber_05.vsnd",
			"sounds/player/footsteps/land_rubber_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Grenade.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/physics/metal/metal_grenade_impact_hard1.vsnd",
			"sounds/physics/metal/metal_grenade_impact_hard2.vsnd",
			"sounds/physics/metal/metal_grenade_impact_hard3.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_WoodBox.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
			"sounds/player/footsteps/wood_13.vsnd",
			"sounds/player/footsteps/wood_14.vsnd",
			"sounds/player/footsteps/wood_15.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Wood_Crate.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Wood_Panel.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wood_01.vsnd",
			"sounds/player/footsteps/wood_02.vsnd",
			"sounds/player/footsteps/wood_03.vsnd",
			"sounds/player/footsteps/wood_04.vsnd",
			"sounds/player/footsteps/wood_05.vsnd",
			"sounds/player/footsteps/wood_06.vsnd",
			"sounds/player/footsteps/wood_07.vsnd",
			"sounds/player/footsteps/wood_08.vsnd",
			"sounds/player/footsteps/wood_09.vsnd",
			"sounds/player/footsteps/wood_10.vsnd",
			"sounds/player/footsteps/wood_11.vsnd",
			"sounds/player/footsteps/wood_12.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Weapon.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		enable_child_events = true
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/physics/weapons/weapon_smg_impact_07.vsnd",
			"sounds/physics/weapons/weapon_smg_impact_08.vsnd",
			"sounds/physics/weapons/weapon_smg_impact_12.vsnd",
			"sounds/physics/weapons/weapon_smg_impact_13.vsnd",
			"sounds/physics/weapons/weapon_smg_impact_16.vsnd",
			"sounds/physics/weapons/weapon_smg_impact_18.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		soundevent_01 = "Land.Thud"
	}

	Land_Foliage.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_grass_01.vsnd",
			"sounds/player/footsteps/land_grass_02.vsnd",
			"sounds/player/footsteps/land_grass_03.vsnd",
			"sounds/player/footsteps/land_grass_04.vsnd",
			"sounds/player/footsteps/land_grass_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_Carpet.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_carpet_01.vsnd",
			"sounds/player/footsteps/land_carpet_02.vsnd",
			"sounds/player/footsteps/land_carpet_03.vsnd",
			"sounds/player/footsteps/land_carpet_04.vsnd",
			"sounds/player/footsteps/land_carpet_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "quietfs 600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		enable_child_events = true
		soundevent_01 = 
		[
			"Land.Thud",
		]
	}

	Land_SlipperySlime.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_mud_01.vsnd",
			"sounds/player/footsteps/land_mud_02.vsnd",
			"sounds/player/footsteps/land_mud_03.vsnd",
			"sounds/player/footsteps/land_mud_04.vsnd",
			"sounds/player/footsteps/land_mud_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Silent.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = 
		[
			"meta",
			"silentfs",
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Wet.Splash = 
	{
		type = "csgo_mega"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.01
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 12.0
		reverb_wet = 0.5
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 1.0
		enable_child_events = false
		set_child_position = false
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.97
		delay = 0.0
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				547.056152, 0.430612, -0.000709, -0.000709,
				3.0, 1.0,
			],
			[
				1300.418213, 0.104898, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1310.0, 0.0, -0.010948, -0.010948,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/wet_01.vsnd",
			"sounds/player/footsteps/wet_02.vsnd",
			"sounds/player/footsteps/wet_03.vsnd",
			"sounds/player/footsteps/wet_04.vsnd",
			"sounds/player/footsteps/wet_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				75.022087, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_WetTile.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		enable_child_events = true
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_tile_01.vsnd",
			"sounds/player/footsteps/land_tile_02.vsnd",
			"sounds/player/footsteps/land_tile_03.vsnd",
			"sounds/player/footsteps/land_tile_04.vsnd",
			"sounds/player/footsteps/land_tile_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		soundevent_01 = 
		[
			"Land_Wet.Splash",
			"Land.Thud",
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_PlayerFlesh.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_body_01.vsnd",
			"sounds/player/footsteps/land_body_02.vsnd",
			"sounds/player/footsteps/land_body_03.vsnd",
			"sounds/player/footsteps/land_body_04.vsnd",
			"sounds/player/footsteps/land_body_05.vsnd",
			"sounds/player/footsteps/land_body_06.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "quietfs 600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Wood_Ladder.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = true
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/ladder_wood_01.vsnd",
			"sounds/player/footsteps/ladder_wood_02.vsnd",
			"sounds/player/footsteps/ladder_wood_03.vsnd",
			"sounds/player/footsteps/ladder_wood_04.vsnd",
			"sounds/player/footsteps/ladder_wood_05.vsnd",
			"sounds/player/footsteps/ladder_wood_06.vsnd",
			"sounds/player/footsteps/ladder_wood_07.vsnd",
			"sounds/player/footsteps/ladder_wood_08.vsnd",
			"sounds/player/footsteps/ladder_wood_09.vsnd",
			"sounds/player/footsteps/ladder_wood_10.vsnd",
			"sounds/player/footsteps/ladder_wood_11.vsnd",
			"sounds/player/footsteps/ladder_wood_12.vsnd",
			"sounds/player/footsteps/ladder_wood_13.vsnd",
			"sounds/player/footsteps/ladder_wood_14.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		soundevent_01 = "Land.Thud"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_WaterVol.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/physics/water/water_splash_01.vsnd",
			"sounds/physics/water/water_splash_02.vsnd",
			"sounds/physics/water/water_splash_03.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				74.064003, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				200.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "quietfs 600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Plastic_Barrel.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_auto_01.vsnd",
			"sounds/player/footsteps/land_auto_02.vsnd",
			"sounds/player/footsteps/land_auto_03.vsnd",
			"sounds/player/footsteps/land_auto_04.vsnd",
			"sounds/player/footsteps/land_auto_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Plastic_milkCrate.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		pitch = 1.0
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_plastic_milkcrates_01.vsnd",
			"sounds/player/footsteps/land_plastic_milkcrates_02.vsnd",
			"sounds/player/footsteps/land_plastic_milkcrates_03.vsnd",
			"sounds/player/footsteps/land_plastic_milkcrates_04.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 0.246154, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_Plastic_autoCover.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_auto_01.vsnd",
			"sounds/player/footsteps/land_auto_02.vsnd",
			"sounds/player/footsteps/land_auto_03.vsnd",
			"sounds/player/footsteps/land_auto_04.vsnd",
			"sounds/player/footsteps/land_auto_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_MetalRailing.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.02
		pitch_random_max = 0.02
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		enable_child_events = true
		pitch = 1.0
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_metal_railing_01.vsnd",
			"sounds/player/footsteps/land_metal_railing_02.vsnd",
			"sounds/player/footsteps/land_metal_railing_03.vsnd",
			"sounds/player/footsteps/land_metal_railing_04.vsnd",
			"sounds/player/footsteps/land_metal_railing_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		soundevent_01 = "Land.Thud"
	}

	Land_WetConcrete.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		enable_child_events = true
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_tile_01.vsnd",
			"sounds/player/footsteps/land_tile_02.vsnd",
			"sounds/player/footsteps/land_tile_03.vsnd",
			"sounds/player/footsteps/land_tile_04.vsnd",
			"sounds/player/footsteps/land_tile_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		soundevent_01 = 
		[
			"Land_Wet.Splash",
			"Land.Thud",
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	Land_WetMud.StepLeft = 
	{
		base = "Base.Land"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		enable_child_events = true
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		occlusion_frequency_scale = 0.998
		distance_effect_mix = 0.6
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/land_mud_01.vsnd",
			"sounds/player/footsteps/land_mud_02.vsnd",
			"sounds/player/footsteps/land_mud_03.vsnd",
			"sounds/player/footsteps/land_mud_04.vsnd",
			"sounds/player/footsteps/land_mud_05.vsnd",
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		soundevent_01 = 
		[
			"Land_Wet.Splash",
			"Land.Thud",
		]
		metadata = "loudfs 1600"
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -1.377315, -1.377315,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
	}

	// Every surface landing also plays this shared impact child.
	Land.Thud = 
	{
		type = "csgo_mega"
		block_matching_events = true
		block_match_entity = true
		mixgroup = "Footsteps"
		block_duration = 0.15
		use_distance_volume_mapping_curve = true
		volume_random_min = 0.0
		volume_random_max = 0.0
		use_fadetime_volume_mapping_curve = false
		use_distance_unfiltered_stereo_mapping_curve = true
		pitch_random_min = -0.06
		pitch_random_max = 0.06
		volume = 0
		block_distance = 0.0
		reverb_wet = 1.0
		use_world_position = true
		use_entity_position_if_local_player = true
		occlusion_intensity = 0.375
		position_offset = [ 0.0, 0.0, 20.0 ]
		display_broadcast = true
		instance_limit = 0.0
		enable_child_events = false
		occlusion_frequency_scale = 0.998
		set_child_position = true
		use_baked_occlusion = true
		distance_volume_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				3.0, 1.0,
			],
			[
				37.42857, 1.0, -0.002865, -0.002865,
				2.0, 3.0,
			],
			[
				429.10892, 0.445141, -0.000903, -0.000903,
				3.0, 1.0,
			],
			[
				1090.0, 0.05, -0.000476, -0.000476,
				2.0, 3.0,
			],
			[
				1100.0, 0.0, -0.005, -0.005,
				0.0, 1.0,
			],
		]
		vsnd_files_track_01 = "sounds/player/footsteps/land_concrete_01.vsnd"
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				50.657143, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				90.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		metadata = "mediumfs 1000"
	}

// Generic local landing playback, independent of the surface landing event.
	Default.Land =
	{
		type = "csgo_mega"
		volume = 0
		pitch = 1.0
		block_duration = 0.4
		mixgroup = "Physics"
		use_distance_volume_mapping_curve = true
		pitch_random_min = -0.01
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		reverb_wet = 0.5
		instance_limit = 2.0
		occlusion_intensity = 0.8
		occlusion_frequency_scale = 0.96
		restrict_source_reverb = true
		vsnd_files_track_01 =
		[
			"sounds/player/land.vsnd",
			"sounds/player/land2.vsnd",
			"sounds/player/land3.vsnd",
		]
		distance_volume_mapping_curve =
		[
			[
				40.0, 1.0, 0.0, -0.003722,
				0.0, 0.0,
			],
			[
				219.428574, 0.332226, -0.003722, -0.001786,
				0.0, 1.0,
			],
			[
				600.0, 0.0, -0.000873, 0.0,
				0.0, 0.0,
			],
		]
		fadetime_volume_mapping_curve =
		[
			[
				0.0, 1.0, 0.0, -10.0,
				0.0, 0.0,
			],
			[
				0.1, 0.0, -10.0, 0.0,
				0.0, 0.0,
			],
		]
	}

Default.WalkJump = 
	{
		type = "csgo_mega"
		volume = 0
		pitch = 1.1
		mixgroup = "Footsteps"
		block_duration = 0.4
		use_distance_volume_mapping_curve = true
		pitch_random_min = -0.01
		pitch_random_max = 0.01
		use_fadetime_volume_mapping_curve = false
		reverb_wet = 0.5
		instance_limit = 3.0
		display_broadcast = true
		position_offset = [ 0.0, 0.0, 30.0 ]
		occlusion_intensity = 0.0
		use_distance_unfiltered_stereo_mapping_curve = true
		broadcast_distance_override = 98.0
		distance_volume_mapping_curve = 
		[
			[
				40.0, 1.0, 0.0, -0.006361,
				0.0, 0.0,
			],
			[
				155.714294, 0.263889, -0.002204, -0.002204,
				1.0, 1.0,
			],
			[
				493.714294, 0.0, -0.000781, 0.0,
				0.0, 0.0,
			],
		]
		fadetime_volume_mapping_curve = 
		[
			[
				0.0, 1.0, -2.108865, -2.108865,
				2.0, 3.0,
			],
			[
				1.0, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		distance_unfiltered_stereo_mapping_curve = 
		[
			[
				0.0, 1.0, 0.0, 0.0,
				2.0, 3.0,
			],
			[
				49.823318, 1.003497, -0.010143, -0.010143,
				3.0, 1.0,
			],
			[
				98.586571, 0.0, 0.0, 0.0,
				2.0, 3.0,
			],
		]
		vsnd_files_track_01 = 
		[
			"sounds/player/footsteps/jump_launch_01.vsnd",
		]
	}
