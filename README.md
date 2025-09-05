# Beat And Rhythm UE
An Unreal project where i experiment with beat and rhythm gameplay mecanics synced with audio.

## Mecanics

### Done 

#### Beat Subsystem

The world automatically play the first audio in its list of available tracks. Each world has defined beat intervals with a trigger event each which actors in the level and other objects can subscribe to for syncronization with the rhythm. The length between two beats depend on the BPM currently playing, the playback time and the settings of the interval they are linked to.  

An interval is a way to define a constant rhythm that follows the BPM. It includes the steps, the offset steps and bpm (if fixed), and whether or not those offsets must be used.  

For example, with a track at 60 BPM in a world with two intervals at 1 and 2 steps (no offset) respectivelly. The first will trigger a beat every second while the other will trigger twice per second (once every 0.5 sec).

### Incoming

- **In Rhythm Subsystem :** A subsystem linked to the beat subsystem. It allows to detect whether an action was performed on the beat. Action does not need to be frame prefect, the system will include a time margin (before and after the beat). Includes a streak system: Streak is broken if the action is not on the beat but does not break if no action was performed for a beat. (Might be definable with settings later) 

### Later

No other mecanics defined yet. Waiting for the base beat and rhythm subsystems to be finished before brainstorming for upgrades and other similar systems.

