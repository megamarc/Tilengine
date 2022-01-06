# Sound

[TOC]

## Introduction

Engine provides functionality to play sound effects using SDL_mixer library.

## Usage
First you need to initialize sound subsystem and preload sound effects with `TLN_SoundInit(int nsounds, char **filenames)`:  

```c
    char *sounds[] = {"assets/1.wav", "assets/2.wav"};
	TLN_SoundInit(2, sounds);
```

Then sounds can be played this way:  

```c
// play assets/1.wav
TLN_PlaySound(0);
// play assets/2.wav
TLN_PlaySound(1);
```

Deinit sound:  
```c
TLN_SoundDeinit();
```
## Summary
This is a quick reference of related functions in this chapter:

|Function                        | Quick description
|--------------------------------|-------------------------------------
|\ref TLN_SoundInit              |Initializes audio subsys and preload sounds
|\ref TLN_SoundDeinit            |Frees resurces occupied by sound subsys
|\ref TLN_PlaySound              |Plays preloaded sound effect
