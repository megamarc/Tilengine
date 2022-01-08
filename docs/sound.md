# Sound

[TOC]

## Introduction

Engine provides functionality to play sound effects using SDL_mixer library.

## Usage

### Sounds
First you need to preload sound effects with `TLN_SoundInit(int nsounds, char **filenames)`:  

```c
char *sounds[] = {"1.wav", "2.wav"};
TLN_SoundInit(2, sounds);
```

Then sounds can be played this way:  

```c
// play assets/1.wav
TLN_PlaySound(0);
// play assets/2.wav
TLN_PlaySound(1);
```

### Music
First you need to preload background music with `TLN_MusicInit(char *filename)`:  

```c
TLN_MusicInit("music.mp3");
```

Then music can be played/stopped this way:  

```c
TLN_PlayMusic();
// ... do something
TLN_StopMusic();
```

Sound volume can be changed with `TLN_MusicVolume(int volume)`.  
`0` - minimum volume (mute), `128` - max volume.  

```c
TLN_MusicVolume(128);
// ... do somenthing
// mute
TLN_MusicVolume(0);
// ... do somenthing
// bring music back
TLN_MusicVolume(128);
```

### Deinit sound  
```c
TLN_SoundDeinit();
```


## Summary
This is a quick reference of related functions in this chapter:

|Function                        | Quick description
|--------------------------------|-------------------------------------
|\ref TLN_SoundInit              |Preload sounds
|\ref TLN_MusicInit              |Preload music
|\ref TLN_SoundDeinit            |Frees resurces occupied by sound subsys
|\ref TLN_PlaySound              |Plays preloaded sound effect
|\ref TLN_PlayMusic              |Plays preloaded music
|\ref TLN_StopMusic              |Stop playing preloaded music
|\ref TLN_MusicVolume            |Change music volume
