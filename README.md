# Beat And Rhythm UE
An Unreal project where i experiment with beat and rhythm gameplay mechanics synced with audio.

## Mecanics

### Done 

#### Beat Subsystem

The world automatically play the first audio in its list of available tracks. Each world has defined beat intervals with a trigger event each which actors in the level and other objects can subscribe to for synchronization with the rhythm. The length between two beats depend on the BPM currently playing, the playback time and the settings of the interval they are linked to.  

An interval is a way to define a constant rhythm that follows the BPM. It includes the steps, the offset steps and bpm (if fixed), and whether or not those offsets must be used.  

For example, with a track at 60 BPM in a world with two intervals at 1 and 2 steps (no offset) respectively. The first will trigger a beat every second while the other will trigger twice per second (once every 0.5 sec).

#### Rhythm Subsystem

A subsystem linked to the beat subsystem. It allows to detect whether an action was performed on the beat. Actions don't need to be frame prefect, includes a time margin (before and after the beat). 

There is also a streak system: 
- Streak is increased (+1) if the action is on the beat. (Within the margin)
- Streak is reset to 0 if the action is not on the beat. (Outside the margin)
- Streak is reset to 0 if another action is performed before the previous one was validated. (No double actions)
- Streak is reset to 0 if an action was already performed for the current beat. (No double actions)
- Streak is NOT reset to 0 if no action was performed for the current beat. (Missing a beat is not a mistake)

For now, only one interval can be used for the rhythm subsystem. The followed interval and the time margin are both hard coded the class for now.

### Incoming

- **In Rhythm Subsystem :** (Update) Allow to change the followed interval and the time margin at runtime and with editor settings (engine & world).

### Later

- **Changing Tracks in a Level:** Being able to change the current track for the background music and keeping the sync with the BPM. Track currently only loops with itself -> add read settings like in stream playlists: Looping, Play Next, Random. Maybe add a transition sound effect if track wasn't finished when the switch was triggered.

- **Track Partition System:** Literally a track's music sheet but without the tone. Kinda like other click-based rhythm games like Osu.  
*MAYBE ?? Really not sure how to do that without having to use level sequencer. Don't want to make a level per track -> more like a music sheet asset so a track can have more than one (difficulty levels why not?).*  

