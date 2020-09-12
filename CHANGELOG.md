# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Implemented Lua Just-In-Time compilation (MoonJIT 2.2.0).

- Implemented PNG file loading and saving. PNGs still need to be indexed just like BMPs! Transparency (alpha) not supported (yet).

- New `Settings.ini` property `LoadingScreenReportPrecision = intValue` to control how accurately the module loading progress reports what line is currently being read.  
	Only relevant when `DisableLoadingScreen = 0`. Default value is 100, lower values increase loading times (especially if set to 1).  
	This should be used for debugging where you need to pinpoint the exact line that is crashing and the crash message isn't helping or doesn't exist at all.

- New `Settings.ini` property `MenuTransitionDuration = floatValue` to control how fast transitions between different menu screens happen (e.g main menu to activity selection screen and back).  
	This property is a multiplier, the default value is 1 (being the default hardcoded values), lower values decrease transition durations. 0 makes transitions instant.

- New `ADoor` sound properties: ([Issue #106](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/106))  
	```
	// Played when the door starts moving from fully open/closed position towards the opposite end.
	DoorMoveStartSound = SoundContainer
		AddSound = ContentFile
			FilePath = pathToFile

	// Played while the door is moving, between fully open/closed position.
	DoorMoveSound = SoundContainer
		AddSound = ContentFile
			FilePath = pathToFile
		LoopSetting = -1 // Doesn't have to loop indefinitely, but generally should.

	// Played when the door changes direction while moving between fully open/closed position.
	DoorDirectionChangeSound = SoundContainer
		AddSound = ContentFile
			FilePath = pathToFile

	// Played when the door stops moving and is at fully open/closed position.
	DoorMoveEndSound = SoundContainer
		AddSound = ContentFile
			FilePath = pathToFile
	```

- Exposed `Actor.StableVelocityThreshold` to lua. New bindings are: ([Issue #101](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/101))  
	`Actor:GetStableVelocityThreshold()` returns a `Vector` with the currently set stable velocity threshold.  
	`Actor:SetStableVelocityThreshold(xFloatValue, yFloatValue)` sets the stable velocity threshold to the passed in float values.  
	`Actor:SetStableVelocityThreshold(Vector)` sets the stable velocity threshold to the passed in `Vector`.

- New `Attachable` and `AEmitter` property `DeleteWithParent = 0/1`. If enabled the attachable/emitter will be deleted along with the parent if parent is deleted/gibbed/destroyed. ([Issue #97](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/97))

- New `Settings.ini` property `LaunchIntoActivity = 0/1`. With `PlayIntro` functionality changed to actually skip the intro and load into main menu, this flag exists to skip both the intro and main menu and load directly into the set default activity.

- Exposed `AHuman.ThrowPrepTime` to lua and ini: ([Issue #101](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/101))  
	`ThrowPrepTime = valueInMS` will set how long it takes the `AHuman` to fully charge a throw. Default value is 1000.  
	`AHuman.ThrowPrepTime` to get/set values via lua.

- Added new `SpriteAnimMode` modes:  
	```
	SpriteAnimMode = 7 // OVERLIFETIME
	```
	This mode handles exactly like (now removed) `MOSParticle.Framerate = 0` and will complete the sprite's animation cycle over the course of its existence. `SpriteAnimDuration` is inapplicable when using this mode and will do nothing.  
	For example, an object that has a sprite with 10 frames and a lifetime of 10 seconds will animate at a rate of 1 frame per second, finishing its animation cycle just before being deleted from the scene.  
	If this mode is used on an object that has `LifeTime = 0` (infinite) it will be overridden to `SpriteAnimMode = 1` (ALWAYSLOOP) otherwise it will never animate.  
	```
	SpriteAnimMode = 8 // ONCOLLIDE
	```
	This mode will drive the animation forward based on collisions this object has with other MOs or the terrain. `SpriteAnimDuration` is inapplicable when using this mode and will do nothing.  
	This mode is `MOSParticle` specific and used mainly for animating casings and small gibs. Using this mode on anything other than `MOSParticle` will do nothing.

- New `Settings.ini` properties `EnableCrabBombs = 0/1` and `CrabBombThreshold = intValue`.  
	When `EnableCrabBombs` is enabled, releasing a number of crabs equal to `CrabBombThreshold` or more at once will trigger the crab bomb effect.  
	If disabled releasing whatever number of crabs will do nothing except release whatever number of crabs.

- Doors can now be stopped at their exact position using `ADoor:StopDoor()` via lua. When stopped, doors will stop updating their sensors and will not try to reset to a default state.  
	If the door was stopped in a script, it needs to opened/closed by calling either `ADoor:OpenDoor()` or `ADoor:CloseDoor()` otherwise it will remain in the exact position it was stopped forever.  
	If either `DrawMaterialLayerWhenOpen` or `DrawMaterialLayerWhenClosed` properties are set true, a material layer will be drawn when the door is stopped. This is to prevent a situation where the material layer will be drawn only if the door is travelling in one direction, without adding an extra property.

- New value `STOPPED` (4) was to the `ADoor.DoorState` enumeration. `ADoor:GetDoorState` will return this if the door was stopped by the user via `ADoor:StopDoor`.

- New shortcut `ALT + W` to generate a detailed 140x55px mini `WorldDump` to be used for scene previews. No relying on `SceneEditor`, stretches over whole image, no ugly cyan bunkers, no actors or glows, has sky gradient, indexed to palette.

- All text in TextBox (any TextBox) can now be selected using `CTRL + A`.

- Console can now be resized using `CTRL + UP/DOWN` (arrow keys) while open.

- Added new lua function `UInputMan:GetInputDevice(playerNum)` to get a number value representing the input device used by the specified player. Should be useful for making custom key bindings compatible with different input devices.

- Scripts can now be attached to `ACrab.Turret` and `Leg`. Additionally, a binding to get the Foot of a Leg has been added.

- Added H/V flipping capabilities to Bitmap primitives.  New bindings with arguments for flip are:  
	`PrimitiveMan:DrawBitmapPrimitive(pos, entity, rotAngle, frame, bool hFlipped, bool vFlipped)`  
	`PrimitiveMan:DrawBitmapPrimitive(player, pos, entity, rotAngle, frame, bool hFlipped, bool vFlipped)`  
	Original bindings with no flip arguments are untouched and can be called as they were.

- Added new primitive drawing functions to `PrimitiveMan`:  
	```
	-- Arc
	PrimitiveMan:DrawArcPrimitive(Vector pos, startAngle, endAngle, radius, color)
	PrimitiveMan:DrawArcPrimitive(player, Vector pos, startAngle, endAngle, radius, color)

	PrimitiveMan:DrawArcPrimitive(Vector pos, startAngle, endAngle, radius, color, thickness)
	PrimitiveMan:DrawArcPrimitive(player, Vector pos, startAngle, endAngle, radius, color, thickness)

	-- Spline (Bézier Curve)
	PrimitiveMan:DrawSplinePrimitive(Vector start, Vector guideA, Vector guideB, Vector end, color)
	PrimitiveMan:DrawSplinePrimitive(player, Vector start, Vector guideA, Vector guideB, Vector end, color)

	-- Box with rounded corners
	PrimitiveMan:DrawRoundedBoxPrimitive(Vector upperLeftCorner, Vector bottomRightCorner, cornerRadius, color)
	PrimitiveMan:DrawRoundedBoxPrimitive(player, Vector upperLeftCorner, Vector bottomRightCorner, cornerRadius, color)

	PrimitiveMan:DrawRoundedBoxFillPrimitive(Vector upperLeftCorner, Vector bottomRightCorner, cornerRadius, color)
	PrimitiveMan:DrawRoundedBoxFillPrimitive(player, Vector upperLeftCorner, Vector bottomRightCorner, cornerRadius, color)

	-- Triangle
	PrimitiveMan:DrawTrianglePrimitive(Vector pointA, Vector pointB, Vector pointC, color)
	PrimitiveMan:DrawTrianglePrimitive(player, Vector pointA, Vector pointB, Vector pointC, color)

	PrimitiveMan:DrawTriangleFillPrimitive(Vector pointA, Vector pointB, Vector pointC, color)
	PrimitiveMan:DrawTriangleFillPrimitive(player, Vector pointA, Vector pointB, Vector pointC, color)

	-- Ellipse
	PrimitiveMan:DrawEllipsePrimitive(Vector pos, horizRadius, vertRadius, color)
	PrimitiveMan:DrawEllipsePrimitive(player, Vector pos, horizRadius, vertRadius, color)

	PrimitiveMan:DrawEllipseFillPrimitive(Vector pos, short horizRadius, short vertRadius, color)
	PrimitiveMan:DrawEllipseFillPrimitive(player, Vector pos, horizRadius, vertRadius, color)
	```

- Added log for non-fatal loading errors. This log will show image files that have been loaded with incorrect extensions (has no side effects but should be addressed) and audio files that failed loading entirely and will not be audible.  
	If errors are present the console will be forced open to notify the player (only when loading into main menu).  
	Log will be automatically deleted if warnings are no longer present to avoid polluting the root directory.

- Game window resolution can now be changed without restarting the game.

### Changed

- Codebase now uses the C++17 standard.

- Updated game framework from Allegro 4.2.3.1 to Allegro 4.4.3.1.

- Major cleanup and reformatting in the `Managers` folder.

- Lua error reporting has been improved so script errors will always show filename and line number.

- Ini error reporting has been improved so asset loading crash messages (image and audio files) will also display the ini file and line they are being referenced from and a better explanation why the crash occured. ([Issue #161](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/161))

- `Settings.ini` will now fully populate with all available settings (now also broken into sections) when being created (first time or after delete) rather than with just a limited set of defaults.

- Temporarily removed `PreciseCollisions` from `Settings.ini` due to bad things happening when disabled by user.

- `Settings.ini` property `PlayIntro` renamed to `SkipIntro` and functionality changed to actually skip the intro and load user directly into main menu, rather than into the set default activity.

- Lua calls for `GetParent` and `GetRootParent` can now be called by any `MovableObject` rather than being limited to `Attachable` only. ([Issue #102](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/102))  
	In some cases a cast to the appropriate type (`ToWhateverType`, e.g `ToMOSRotating`) will be needed when attempting to manipulate the object returned, because it will be returned as `MovableObject` if it is the root parent.  
	In cases where you need to iterate over a parent's attachable list the parent must be cast to the appropriate type that actually has an attachable list to iterate over.  
	For example:  
	```
	for attachable in ToMOSRotating(self:GetParent()).Attachables do
		...
	end
	```
	Or
	```
	local parent = ToMOSRotating(self:GetParent());
	for attachable in parent.Attachables do
		...
	end
	```

- Physics constants handling removed from `FrameMan` and now hardcoded in `Constants`. Lua bindings moved to `RTETools` and are now called without the `FrameMan` prefix like so:  
	`GetPPM()`, `GetMPP()`, `GetPPL()`, `GetLPP()`.

- Removed hardcoded 10 second `LifeTime` restriction for `MOPixel` and `MOSParticle`.

- `MOSParticle` animation can now be set with `SpriteAnimMode` and `SpriteAnimDuration`. If the property isn't defined it will default to `SpriteAnimMode = 7` (OVERLIFETIME).

- Reworked crab bombing behavior. When enabled through `Settings.ini` and triggered will gib all living actors on scene except brains and doors. Devices and non-actor MOs will remain untouched.

- `ADoor` properties `DrawWhenOpen` and `DrawWhenClosed` renamed to `DrawMaterialLayerWhenOpen` and `DrawMaterialLayerWhenClosed` so they are more clear on what they actually do.

- Specially handled Lua function `OnScriptRemoveOrDisable` has been changed to `OnScriptDisable`, and no longer has a parameter saying whether it was removed or disabled, since you can no longer remove scripts.

- When pasting multiple lines of code into the console all of them will be executed instead of the last line being pasted into the textbox and all before it executing.

- Input enums moved from `UInputMan` to `Constants` and are no longer accessed with the `UInputManager` prefix. These enums are now accessed with their own names as the prefix.  
	For example: `UInputManager.DEVICE_KEYB_ONLY` is now `InputDevice.DEVICE_KEYB_ONLY`, `UInputManager.INPUT_L_UP` is now `InputElements.INPUT_L_UP` and so on.

- `CraftsOrbitAtTheEdge` corrected to `CraftOrbitAtTheEdge`. Applies to both ini property and lua binding.

- Game will now Abort with an error message when trying to load a copy of a non-existent `AtomGroup`, `Attachable` or `AEmitter` preset.

- ComboBoxes (dropdown lists) can now also be closed by clicking on their top part.

- `Activity:IsPlayerTeam` renamed to `Activity:IsHumanTeam`.

- Screenshot functionality changed: ([Issue #162](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/162))  
	The `PrintScreen` button will now take a single screenshot on key release and will not take more until the key is pressed and released again.  
	The `Ctrl+S` key combination is unchanged and will take a single screenshot every frame while the keys are held.  
	The `Ctrl+W` and `Alt+W` key combinations will now take a single WorldDump/ScenePreview on `W` key release (while `Ctrl/Alt` are still held) and will not take more until the key is pressed and released again.

	Additionally, all screenshots (excluding abortscreen) will now be saved into a `_Screenshots` folder (`_` so it's on top and not hiding between module folders) to avoid polluting the root directory. ([Issue #163](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/163))  
	This folder will be created automatically after modules are loaded if it is missing.

- `ScreenDumps` and `WorldDumps` are now saved as compressed PNGs.

- Controller deadzone setting ignores more input. Previously setting it to the maximum was just enough to eliminate stick drift.

- `Arm.HandPos` will now get/set the hand position as relative to the arm's joint position, instead of relative to the arm's center of mass.

- Resolution settings in options screen changed:  
	Resolution multiplier button changed to `Fullscreen` button - this will set the game window resolution to match the desktop resolution. When resolution matches the desktop, this button will change to `Windowed` and will allow setting the game window resolution back to default (960x540).  
	Added `Upscaled Fullscreen` button - this will change the resolution to half of the desktop and the multiplier to 2. The `Fullscreen` button will change to `Windowed` in this mode to return to non-upscaled mode (960x540).  
	Selecting any resolution setting from the resolution combobox will immediately change to selected resolution. (Known bug: Clicking off the combobox without making a new selection while in `Upscaled Fullscreen` mode will change resolution to `Fullscreen`. This will be addressed later.)  

	**Note:** Changing the game window resolution while an Activity is active requires ending the Activity. A dialog box will appear asking to confirm the change.

- Moved from C-style random number generation to C++ standard. This includes usage of an mt19937 random number generator.  
	For C++ coders the functions SelectRand, PosRand and RangeRand have been replaced by the function template RandomNum() and its overload RandomNum(T min, T max). The function NormalRand has been replaced by the function template RandomNormalNum(). For lua coders there is no change.
	
- Resolution validation changed to support multiple screens. Incompatible/bad resolution settings will be overridden at startup with messages explaining the issue.  
	**Note:** For multi-screen to work properly, the left-most screen MUST be set as primary. Screens having different resolutions does not actually matter but different heights will still be warned about and overridden due to the likeliness of GUI elementes being cropped on the shortest screen.  
	Resolution validation can be disabled for multi-screen setups with `Settings.ini` property `DisableMultiScreenResolutionValidation`. Bad settings are likely to crash, use at own risk.  
	For setups with more than 3 screens `DisableMultiScreenResolutionValidation` must be set true.

- Damage to actors from impulses is now relative to their max health instead of being on a scale from 0 to 100.

### Fixed

- Fix crash when returning to `MetaGame` scenario screen after activity end.

- Control schemes will no longer get deleted when being configured. Resetting the control scheme will load a preset instead of leaving it blank. ([Issue #121](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/121))

- Fix glow effects being drawn one frame past `EffectStartTime` making objects that exist for a single frame not draw glows. ([Issue #67](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/67))

- Time scale can no longer be lowered to 0 through the performance stats interface.

- Actors now support their held devices identically while facing to either side. ([Issue #31](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/31))

- Fixed issue where clicking a ComboBox's scrollbar would release the mouse, thus causing unexpected behavior like not being able to close the list by clicking outside of it.

- Fixed issue where ComboBoxes did not save the current selection, thus if the ComboBox was deselected without making a selection then the selection would revert to the default value instead of the last selected value.

- Fixed issue with double clicks and missing clicks in menus (anything that uses AllegroInput).

- Fixed issue where OnPieMenu function wasn't working for `AHuman` equipped items, and made it work for `BGArm` equipped items as well as `FGArm` ones.

- The "woosh" sound played when switching actors from a distance will now take scene wrapping into account. Additionally, attempting to switch to previous or next actor with only one actor will play the more correct "error" sound.

### Removed

- Removed the ability to remove scripts from objects with Lua. This is no longer needed cause of code efficiency increases.

- Removed `Settings.ini` property `PixelsPerMeter`. Now hardcoded and cannot be changed by the user.

- Removed `MOSParticle` property `Framerate` and lua bindings. `MOSParticle` animation is now handled with `SpriteAnimMode` like everything else.

- Removed `ConsoleMan.ForceVisibility` and `ConsoleMan.ScreenSize` lua bindings.

- Removed `ActivityMan.PlayerCount` and `ActivityMan.TeamCount` setters lua bindings (obsolete and did nothing).

- Removed `Activity` properties `TeamCount` and `PlayerCount`. These are handled internally and do nothing when set in ini.

- Removed `Activity` property `FundsOfTeam#`, use `Team#Funds` instead.

***

## [0.1.0 pre-release 2][0.1.0-pre2] - 2020/05/08

### Added

- Lua binding for `Box::IntersectsBox(otherBox)`, that returns true if 2 boxes intersect.

- Command line arguments for launching directly into editors using `-editor "EditorName"`.  
	Valid editor names are: `ActorEditor`, `GibEditor`, `SceneEditor`, `AreaEditor` and `AssemblyEditor`.

- Added handling for custom number and string values in INI.
	```
	AddCustomValue = NumberValue
		YourKeyName = YourNumberValue // Integer or floating point number.

	AddCustomValue = StringValue
		YourKeyName = YourStringValue
	```
	`YourKeyName` is a string value and is not limited to just numbers.

- New `Settings.ini` property `AdvancedPerformanceStats = 0/1` to disable/enable the performance counter graphs (enabled by default).

- Added `PassengerSlots` INI and Lua property to Actors. This determines how many spaces in the buy menu an actor will take up (1 by default). It must be a whole number but can theoretically be 0 or less.

- Added Lua bindings for `IsInsideX` and `IsInsideY` to `Area`. These act similarly to the pre-existing `IsInside`, but allow you to check for the X and Y axes individually.

- Added the concept of `SoundSets`, which are collections of sounds inside a `SoundContainer`. This allows you to, for example, put multiple sounds for a given gunshot inside a `SoundSet` so they're played together.

- `SoundContainers` have been overhauled to allow for a lot more customization, including per-sound customization. The following INI example shows all currently available capabilities with explanatory comments:
	```
	AddSoundContainer = SoundContainer // Note that SoundContainers replace Sounds, so this can be used for things like FireSound = SoundContainer
		PresetName = Preset Name Here

		CycleMode = MODE_RANDOM (default) | MODE_FORWARDS // How the SoundContainer will cycle through its `SoundSets` whenever it's told to select a new one. The former is prior behaviour, the latter cycles through SoundSets in the order they were added.

		LoopSetting = -1 | 0 (default) | 1+ // How the SoundContainer loops its sounds. -1 means it loops forever, 0 means it plays once, any number > 0 means it plays once and loops that many times.

		Immobile = 0 (default) | 1 // Whether or not the SoundContainer's sounds should be treated as immobile. Immobile sounds are generally used for UI and system sounds; they will always play at full volume and will not be panned or affected by global pitch during game slowdown.

		AttenuationStartDistance = Number (default -1) // The distance at which the SoundContainer's sounds will start to attenuate out, any number < 0 set it to the game's default. Attenuation calculations follows FMOD's Inverse Rolloff model, which you can find linked below.

		Priority = 0 - 256 (default 128) // The priority at which the SoundContainer's sounds will be played, between 0 (highest priority) and 256 (lowest priority). Lower priority sounds are less likely to be played are a lot of sounds playing.

		AffectedByGlobalPitch = 0 | 1 (default) // Whether or not the SoundContainer's sounds will be affected by global pitch, or only change pitch when manually made to do so via Lua (note that pitch setting is done via AudioMan).

		AddSoundSet = SoundSet // This adds a SoundSet containing one or more sounds to the SoundContainer.

			AddSound = ContentFile // This adds a sound to the SoundSet, allowing it to be customized as shown.
				Filepath = "SomeRte.rte/Path/To/Sound.wav"

				Offset = Vector // This specifies where the sound plays with respect to its SoundContainer. This allows, for example, different sounds in a gun's reload to come from slightly different locations.
					X = Number
					Y = Number

				AttenuationStartDistance = Number // This functions identically to SoundContainer AttenuationStartDistance, allowing you to override it for specific sounds in the SoundContainer.

				MinimumAudibleDistance = Number (default 0) // This allows you to make a sound not play while the listener is within a certain distance, e.g. for gunshot echoes. It is automatically accounted for in sound attenuation.

			AddSound = "SomeRte.rte/Path/To/AnotherSound.wav" // This adds a sound to the SoundSet in oneline, allowing it to be compactly added (without customisation).

		AddSound = "SomeRte.rte/Path/To/YetAnotherSound.wav" // This adds a sound to the SoundContainer, creating a new SoundSet for it with just this sound.
	```
	NOTE: Here is a link to [FMOD's Inverse Rolloff Model.](https://fmod.com/resources/documentation-api?version=2.0&page=white-papers-3d-sounds.html#inverse)

- `SoundContainer` Lua controls have been overhauled, allowing for more control in playing and replaying them. The following Lua bindings are available:
	```
	soundContainer:HasAnySounds() - Returns whether or not the SoundContainer has any sounds in it. Returns True or false.
	```
	```
	soundContainer:IsBeingPlayed() - Returns whether or not any sounds in the SoundContainer are currently being played. Returns True or False.
	```
	```
	soundContainer:Play(optionalPosition, optionalPlayer) - Plays the sounds belonging to the SoundContainer's currently selected SoundSet. The sound will play at the position and for the player specified, or at (0, 0) for all players if parameters aren't specified.
	```
	```
	soundContainer:Stop(optionalPlayer) - Stops any playing sounds belonging to the SoundContainer, optionally only stopping them for a specified player.
	```
	```
	soundContainer:AddSound(filePath, optional soundSetToAddSoundTo, optionalSoundOffset, optionalAttenuationStartDistance, optionalAbortGameIfSoundIsInvalid) - Adds the sound at the given filepath to the SoundContainer. If a SoundSet index is specified it'll add it to that SoundSet. If an offset or attenuation start distance are specified they'll be set, as mentioned in the INI section above. If set to abort for invalid sounds, the game will error out if it can't load the sound, otherwise it'll show a console error.
	```
	```
	soundContainer:SetPosition(position) - Sets the position at which the SoundContainer's sounds will play.
	```
	```
	soundContainer:SelectNextSoundSet() - Selects the next SoundSet to play when soundContainer:Play(...) is called, according to the INI defined CycleMode.
	```
	```
	soundContainer.Loops - Set or get the number of loops for the SoundContainer, as mentioned in the INI section above.
	```
	```
	soundContainer.Priority - Set or get the priority of the SoundContainer, as mentioned in the INI section above.
	```
	```
	soundContainer.AffectedByGlobalPitch - Set or get whether the SoundContainer is affected by global pitch, as mentioned in the INI section above.
	```
- `MovableObjects` can now run multiple scripts by putting multiple `AddScript = FilePath.lua` lines in the INI definition. ([Issue #109](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/pull/109))  
	Scripts will have their appropriate functions run in the order they were added. Note that all scripts share the same `self`, so care must be taken when naming self variables.  
	Scripts can be checked for with `movableObject:HasScript(filePath);` and added and removed with `movableObject:AddScript(filePath);` and `movableObject:RemoveScript(filePath);`. They can also be enabled and disabled in Lua (preserving their ordering) with `movableObject:EnableScript(filePath);` and `movableObject:DisableScript(filePath);`.

- Scripts on `MovableObjects` and anything that extends them (i.e. most things) now support the following new functions (in addition to `Create`, `Update`, `Destroy` and `OnPieMenu`). They are added in the same way as the aforementioned scripts:  
	```
	OnScriptRemoveOrDisable(self, scriptWasRemoved) - This is run when the script is removed or disabled. The scriptWasRemoved parameter will be True if the script was removed and False if it was disabled.
	```
	```
	OnScriptEnable(self) - This is run when the script was disabled and has been enabled.
	```
	```
	OnCollideWithTerrain(self, terrainMaterial) - This is run when the MovableObject this script on is in contact with terrain. The terrainMaterial parameter gives you the material ID for the terrain collided with. It is suggested to disable this script when not needed to save on overhead, as it will be run a lot!
	```
	```
	OnCollideWithMO(self, collidedMO, collidedRootMO) - This is run when the MovableObject this script is on is in contact with another MovableObject. The collidedMO parameter gives you the MovableObject that was collided with, and the collidedRootMO parameter gives you the root MovableObject of that MovableObject (note that they may be the same). Collisions with MovableObjects that share the same root MovableObject will not call this function.
	```

- Scripts on `Attachables` now support the following new functions:  
	```
	OnAttach(self, newParent) - This is run when the Attachable this script is on is attached to a new parent object. The newParent parameter gives you the object the Attachable is now attached to.
	```
	```
	OnDetach(self, exParent) - This is run when the Attachable this script is on is detached from an object. The exParent gives you the object the Attachable was attached to.
	```

### Changed

- Codebase now uses the C++14 standard.

- Major cleanup and reformatting in the `System` folder.

- Upgraded to new, modern FMOD audio library. ([Issue #72](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/72)).  
	Sounds now play in 3D space, so they pan to the left and right, and attenuate automatically based on the player's viewpoint.

- `Sounds` have been renamed to `SoundContainers`, and are able to handle multiple sounds playing at once. INI definitions have changed accordingly.  
	They must be added using `... = SoundContainer`, and individual sounds for them must be added using `AddSound = ContentFile...`.

- Various lua bindings around audio have been upgraded, changed or fixed, giving modders a lot more control over sounds. See documentation for more details.

- Centered the loading splash screen image when `DisableLoadingScreen` is true.

- `Box:WithinBox` lua bindings have been renamed:  
	`Box:WithinBox` is now `Box:IsWithinBox`.  
	`Box:WithinBoxX` is now `Box:IsWithinBoxX`.  
	`Box:WithinBoxY` is now `Box:IsWithinBoxY`.

- Made `AHuman` show both weapon ammo states when 2 one-handed weapons are equipped.

- Added support for multiple lines in item descriptions ([Issue#58](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/58)). This is done as follows:
	```
	Description = MultiLineText
		AddLine = First line of text
		AddLine = Second line of text
		...
	```

- `FrameMan` broken down to 4 managers. New managers are:  
	`PerformanceMan` to handle all performance stats and measurements.  
	`PostProcessMan` to handle all post-processing (glows).  
	`PrimitiveMan` to handle all lua primitive drawing.

- Post-processing (glow effects) is now enabled at all times with no option to disable.

- All lua primitive draw calls are now called from `PrimitiveMan`.  
	For example: `FrameMan:DrawLinePrimitive()` is now `PrimitiveMan:DrawLinePrimitive()`.

- Resolution multiplier properties (`NxWindowed` and `NxFullscreen`) in settings merged into a single property `ResolutionMultiplier`.

- Incompatible/bad resolution settings will be overridden at startup with messages explaining the issue instead of multiple mode switches and eventually a reset to default VGA.  
	Reset to defaults (now 960x540) will happen only on horrible aspect ratio or if you managed to really destroy something.

- You can no longer toggle native fullscreen mode from the settings menu or ini. Instead, either select your desktop resolution at 1X mode or desktop resolution divided by 2 at 2X mode for borderless fullscreen windowed mode.  
	Due to limitations in Allegro 4, changing the actual resolution from within the game still requires a restart.

- If the current game resolution is half the desktop resolution or less, you will be able to instantly switch between 1X and 2X resolution multiplier modes in the settings without screen flicker or delay.  
	If the conditions are not met, the mode switch button will show `Unavailable`.

- `PieMenuActor` and `OrbitedCraft` have now been removed. They are instead replaced with parameters in their respective functions, i.e. `OnPieMenu(pieMenuActor);` and `CraftEnteredOrbit(orbitedCraft);`. Their use is otherwise unchanged.

### Fixed

- Fixed LuaBind being all sorts of messed up. All lua bindings now work properly like they were before updating to the v141 toolset.

- Explosives (and other thrown devices) will no longer reset their explosion triggering timer when they're picked up. ([Issue #71](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/71))

- Sprite Animation Mode `ALWAYSPINGPONG` now works properly. Sprite animation has also been moved to `MOSprite` instead of `MOSRotating`, they they'll be able to properly animate now. ([Issue#77](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/77))

- Fixed `BG Arm` flailing when reloading one-handed weapon, so shields are no longer so useless.

- Fixed crash when clearing an already empty preset list in the buy menu.

- Temporary fix for low mass attachables/emitters being thrown at ridiculous speeds when their parent is gibbed.

- The audio system now better supports splitscreen games, turning off sound panning for them and attenuating according to the nearest player.

- The audio system now better supports wrapping maps so sounds handle the seam better. Additionally, the game should be able to function if the audio system fails to start up.

- Scripts on attached attachables will only run if their parent exists in MovableMan. ([Issue #83](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/83))

### Removed

- Removed all Gorilla Audio and SDL Mixer related code and files.

- Removed all Steam Workshop and Achievement related code.

- Removed a bunch of outdated/unused sources in the repo.

- Removed all OSX/Linux related code and files because we don't care. See [Liberated Cortex](https://github.com/liberated-cortex) for working Linux port.

- Removed a bunch of low-level `FrameMan` lua bindings:  
	`FrameMan:ResetSplitScreens`, `FrameMan:PPM` setter, `FrameMan:ResX/Y`, `FrameMan:HSplit/VSplit`, `FrameMan:GetPlayerFrameBufferWidth/Height`, `FrameMan:IsFullscreen`, `FrameMan:ToggleFullScreen`, `FrameMan:ClearBackbuffer8/32`, `FrameMan:ClearPostEffects`, `FrameMan:ResetFrameTimer`, `FrameMan:ShowPerformanceStats`.

- Native fullscreen mode has been removed due to poor performance compared to windowed/borderless mode and various input device issues.  
	The version of Allegro we're running is pretty old now (released in 2007) and probably doesn't properly support/utilize newer features and APIs leading to these issues.  
	The minimal amount of hardware acceleration CC has is still retained through Windows' DWM and that evidently does a better job.

- Removed now obsolete `Settings.ini` properties:  
	**Post-processing:** `TrueColorMode`, `PostProcessing`, `PostPixelGlow`.   
	**Native fullscreen mode:** `Fullscreen`, `NxWindowed`, `NxFullscreen`, `ForceSoftwareGfxDriver`, `ForceSafeGfxDriver`.

***

## [0.1.0 pre-release 1][0.1.0-pre1] - 2020/01/27

### Added

- You can now run the game with command line parameters, including `-h` to see help and `-c` to send ingame console input to cout.

- `MOSprite` now has the `FlipFactor` property that returns -1 if the sprite is flipped and 1 if it's not.  
	Using any `nugNum` calculations based on `HFlipped` is now considered criminal activity.

- `TDExplosive` now has the `IsAnimatedManually` property that lets modders set its frames manually through lua.

- You can now add `AEmitters` to `MOSRotating` and have them function similarly to attachables.  
	**Addition:** `parent:AddEmitter(emitterToAdd)` or `parent:AddEmitter(emitterToAdd, parentOffsetVector)`  
	**Removal:** `parent:RemoveEmitter(emitterToRemove)` or `parent:RemoveEmitter(uniqueIdOfEmitterToRemove)`

- Attachables can now collide with terrain when attached.  
	**INI property:** `CollidesWithTerrainWhenAttached = 0/1`  
	**Check value:** `attachable.IsCollidingWithTerrainWhileAttached`  
	**Manipulate function:** `attachable:EnableTerrainCollisions(trueOrFalse)`  
	Collisions can be manipulated only if the attachable was set to `CollidesWithTerrainWhenAttached = 1` in ini.

- `Actor.DeathSound` is now accessible to lua using `Actor.DeathSound = "string pathToNewFile"` or `Actor.DeathSound = nil` for no DeathSound.

- `AHuman` Feet are now accessible to lua using `AHuman.FGFoot` and `AHuman.BGFoot`. Interaction with them may be wonky.

- Streamlined debug process and requirements so old Visual C++ Express edition is no longer needed for debugging.

- Added minimal debug configuration for quicker debug builds without visualization.

### Changed

- `ACrab` aim limits now adjust to crab body rotation.

- `ACrab.AimRange` can now be split into `AimRangeUpperLimit` and `AimRangeLowerLimit`, allowing asymmetric ranges.

- Objective arrows and Delivery arrows are now color coordinated to match their teams, instead of being only green or red.

- BuyMenu `Bombs` tab will now show all `ThrownDevices` instead of just `TDExplosives`.

- The list of `MOSRotating` attachables (`mosr.Attachables`) now includes hardcoded attachables like dropship engines, legs, etc.

- Attachable lua manipulation has been significantly revamped. The old method of doing `attachable:Attach(parent)` has been replaced with the following:  
	**Addition:** `parent:AddAttachable(attachableToAdd)` or `parent:AddAttachable(attachableToAdd, parentOffsetVector)`  
	**Removal:** `parent:RemoveAttachable(attachableToRemove)` or `parent:RemoveAttachable(uniqueIdOfAttachableToRemove)`

- Wounds have been separated internally from emitter attachables.  
	They can now be added with `parent:AddWound(woundEmitterToAdd)`.  
	Removing wounds remains the same as before.

- Built-in Actor angular velocity reduction on death has been lessened.

### Fixed

- SFX slider now works properly.

- BGM now loops properly.

- Sound pitching now respects sounds that are not supposed to be affected by pitch.

- Using `actor:Clone()` now works properly, there are no longer issues with controlling/selecting cloned actors.

- `TDExplosive.ActivatesWhenReleased` now works properly.

- Various bug fixed related to all the Attachable and Emitter changes, so they can now me affected reliably and safely with lua.

- Various minor other things that have gotten lost in the shuffle.

### Removed

- All licensing-related code has been removed since it's no longer needed.

- Wounds can no longer be added via ini, as used to be doable buggily through ini `AddEmitter`.

- All usage of the outdated Slick Profiler has been removed.

- `TDExplosive.ParticleNumberToAdd` property has been removed.

***

Note: For a log of changes made prior to the commencement of the open source community project, look [here.](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/wiki/Previous-Closed-Source-Changelog)


[unreleased]: https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/compare/master...cortex-command-community:development
[0.1.0-pre1]: https://github.com/cortex-command-community/Cortex-Command-Community-Project-Data/releases/tag/v0.1.0-pre1
[0.1.0-pre2]: https://github.com/cortex-command-community/Cortex-Command-Community-Project-Data/releases/tag/v0.1.0-pre2
