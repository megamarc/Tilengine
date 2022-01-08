/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* Sound module created by Aleksey Malyshenko <https://github.com/lxxxxl>
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include "Tilengine.h"
#include "SDL2/SDL_mixer.h"

#ifdef _MSC_VER
#define inline __inline
#endif

#define MAX_PATH	300
Mix_Chunk* _sample[8];
Mix_Music* _music;

bool _mixAudioOpened = false;
bool _mixChannelsAllocated = false;


/*!
 * \brief
 * Initialize audio subsys 
 */
bool TLN_AudioSybsysInit()
{
	// Set up the audio stream
	if(!_mixAudioOpened && Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0)
		return false;
	_mixAudioOpened = true;

	if(!_mixChannelsAllocated && Mix_AllocateChannels(MIX_DEFAULT_CHANNELS) < 0)
			return false;
	_mixChannelsAllocated = true;

	return true;
}

/*!
 * \brief
 * Preload sounds
 * 
 * \param nsounds
 * Count of preloaded sound files, max 8
 * 
 * \param filenames
 * Array of filenames
 * 
 */
bool TLN_SoundInit(int nsounds, char **filenames)
{
	memset(_sample, 0, sizeof(Mix_Chunk*) * 8);
	if (!TLN_AudioSybsysInit())
		return false;

	// Load waveforms
	for(int i = 0; i < nsounds; i++)
	{
		_sample[i] = Mix_LoadWAV(filenames[i]);
		if( _sample[i] == NULL )
		{
			return false;
		}
	}

	return true;
}

/*!
 * \brief
 * Preload music
 * 
 * \param filename
 * Filename of melody
 * 
 */
bool TLN_MusicInit(char *filename)
{
	if(!TLN_AudioSybsysInit())
		return false;

	_music = Mix_LoadMUS(filename);
	if (_music==NULL)
		return false;

	return true;
}

/*!
 * \brief
 * Free resurces occupied by sound subsys
 * 
 */
void TLN_SoundDeinit()
{
	for(int i = 0; i < 8; i++)
	{
		if (_sample[i] != NULL)
			Mix_FreeChunk(_sample[i]);
	}
	Mix_HaltMusic();
	Mix_FreeMusic(_music);
	Mix_CloseAudio();
}

/*!
 * \brief
 * Play preloaded sound
 * 
 * \param nsound
 * Id of the sound previously preloaded
 * 
 */
void TLN_PlaySound(int nsound)
{
	if (_sample[nsound] != NULL)
		Mix_PlayChannel(-1, _sample[nsound], 0);
}

/*!
 * \brief
 * Play preloaded music
 * 
 */
void TLN_PlayMusic()
{
	if (_music == NULL)
		return;

	if (Mix_PlayingMusic())
		Mix_HaltMusic();

	Mix_PlayMusic(_music, -1);
}

/*!
 * \brief
 * Stop playing preloaded music
 * 
 */
void TLN_StopMusic()
{
	Mix_HaltMusic();
}