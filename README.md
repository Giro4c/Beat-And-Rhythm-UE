# Beat And Rhythm UE
An Unreal project where i experiment with beat and rhythm gameplay mechanics synced with audio.

## Mecanics

### Done 

#### Beat Subsystem

The world automatically play the first audio in its list of available tracks. Each world has defined beat intervals with a trigger event each which actors in the level and other objects can subscribe to for synchronization with the rhythm. The length between two beats depend on the BPM currently playing, the playback time and the settings of the interval they are linked to.  

An interval is a way to define a constant rhythm that follows the BPM. It includes the steps, the offset steps and bpm (if fixed), and whether or not those offsets must be used.  

For example, with a track at 60 BPM in a world with two intervals at 1 and 2 steps (no offset) respectively. The first will trigger a beat every second while the other will trigger twice per second (once every 0.5 sec).

#### Rhythm Subsystem

A subsystem linked to the beat subsystem. It allows to detect whether an action was performed on the beat. Actions don't need to be frame prefect, includes a time margin (before and after the beat) editable in the global settings "Beat & Rhythm". 

There is also a streak system: 
- Streak is increased (+1) if the action is on the beat. (Within the margin)
- Streak is reset to 0 if the action is not on the beat. (Outside the margin)
- Streak is reset to 0 if another action is performed before the previous one was validated. (No double actions)
- Streak is reset to 0 if an action was already performed for the current beat. (No double actions)
- Streak is NOT reset to 0 if no action was performed for the current beat. (Missing a beat is not a mistake)

The system follows a single beat pattern that can be defined in the world settings. The rhythm subsystem will use the beat events from that interval to validate actions if it exists otherwise it will use the default one.  

### Incoming

- **Plugin Version:** Make the Beat & Rhythm system a plugin so it can be used in any project. Also allow for multiple version to coexist (one for simple rhythm, one for complex music-sheet-like rhythm, etc).

### Later

- **Changing Tracks in a Level at Runtime:** Being able to change the current track for the background music and keeping the sync with the BPM. Maybe add a transition sound effect if track wasn't finished when the switch was triggered.

- **In Rhythm Subsystem :** Improve the audio play system (especially for looping) so as to only play the next audio on the next beat of the current track despite it not playing anymore. This will avoid having abnormal gap between beats when the track is looping but stopped after its last beat.  
  

- **Track Partition System:** Literally a track's music sheet but without the tone. Kinda like other click-based rhythm games like Osu.  
    _Bellow are two possible versions i brainstormed for the system._

_Track Partition System V1:_  

A track partition is made of segments whose count is defined by the maximum steps for an interval (fastest rhythm possible). Each segment can have a beat or not (1 or 0).  
When a track with a partition is played in a world, the beat subsystem will use the partition to trigger beats only when there is one in the current segment. The rhythm subsystem will then use those beats to validate actions.  

The partition created for a track is editable in a segmented line-like editor made with IMGUI. The user can choose the linked track, the maximum number of steps, save the partition where they want and click on the segments to add or remove a beat. When saved, the partition is serialized in a text or binary file that can be loaded into a partition class instance thanks to a utility function dedicated to serialization/deserialization.
This is an example of a partition file without the track reference (maybe store sheet without track for easy reuse ?):  
```
4
101010001010
```
Since the maximum steps will be a float (e.g 4 bytes = 32 bits) and each segment will be a bit (1 or 0), the partition could be stored in a binary file but not sure how to get the content of a binary file as a bit array in C++ (maybe read byte per byte and convert each byte to its bit representation ? Might need an additional value to define the exact number of segments since last byte may actually contain less than 8 segments).

_Track Partition System V2:_  

A track partition is made of segments that each have a beat or not (1 or 0) and a step value (float). The step value will define the length of the segment. This will allow for true musical rhythm like what can be found on music sheets and the system can also be expanded to support extended beat (e.g long notes in a rhythm game).  
When a track with a partition is played in a world, the beat subsystem will use the partition to trigger beats only when there is one in the current segment. The rhythm subsystem will then use those beats to validate actions.  

The partition created for a track is editable in a segmented line-like editor made with IMGUI. The user can choose the linked track, the scale for the grid, save the partition where they want and click on the segments to add, remove a beat or change their length. When saved, the partition is serialized in a text or binary file that can be loaded into a partition class instance thanks to a utility function dedicated to serialization/deserialization. 
This is an example of a partition file without the track reference (maybe store sheet without track for easy reuse ?):  
```
4 (scale)
1 0.5 1 0.5 1 2 1 0.5 1 0.5 (no space)
```
Since the scale will be a float (e.g 4 bytes = 32 bits) and each segment will be a bit (1 or 0) followed by a float (4 bytes = 32 bits), the partition could be stored in a binary file but not sure how to get the content of a binary file as a bit array in C++ since bytes are not possible (the presence of a bit between every float prevents the use of byte array).  
If the number of segments is known then maybe all floats can be serialized one after another and the bits can be stored after with the special treatment of the last byte (if the number of segments is not a multiple of 8).  
