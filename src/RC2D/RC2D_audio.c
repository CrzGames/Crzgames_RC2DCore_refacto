#include <RC2D/RC2D_audio.h>
#include <RC2D/RC2D_logger.h>

/*RC2D_SourceAudio rc2d_audio_newSource(const char* filePath, const RC2D_FileAudioType fileType, const RC2D_AudioType audioType)
{
    if (filePath == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de charger la source audio : chemin de fichier invalide.\n");
        RC2D_SourceAudio sourceAudio;
        sourceAudio.music = NULL;
        sourceAudio.sound = NULL;
        sourceAudio.fileType = RC2D_FILE_TYPE_UNKNOWN;
        sourceAudio.audioType = RC2D_AUDIO_TYPE_UNKNOWN;
        return sourceAudio;
    }

    RC2D_SourceAudio sourceAudio;
    sourceAudio.music = NULL;
    sourceAudio.sound = NULL;
    sourceAudio.fileType = fileType;
    sourceAudio.audioType = audioType;

    // Chargement du son
    if (audioType == RC2D_AUDIO_TYPE_SOUND)
    {
        if (fileType == RC2D_FILE_TYPE_WAV)
        {
            Mix_Chunk* soundEffect = NULL;
            soundEffect = Mix_LoadWAV(filePath);

            if (soundEffect == NULL)
            {
                RC2D_log(RC2D_LOG_ERROR, "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
            }
            else
            {
                sourceAudio.sound = soundEffect;
                return sourceAudio;
            }
        }
        else if (fileType == RC2D_FILE_TYPE_MP3)
        {
            // Charger le fichier MP3
        }
    }

    // Chargement de la musique
    else if (audioType == RC2D_AUDIO_TYPE_MUSIC)
    {
        if (fileType == RC2D_FILE_TYPE_WAV)
        {
            Mix_Music* music = NULL;
            music = Mix_LoadMUS(filePath);

            if (music == NULL)
            {
                RC2D_log(RC2D_LOG_ERROR, "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
            }
            else
            {
                sourceAudio.music = music;
                return sourceAudio;
            }
        }
        else if (fileType == RC2D_FILE_TYPE_MP3)
        {
            // Charger le fichier MP3
        }
    }

    // Retourner la source audio vide en cas d'erreur
    return sourceAudio;
}

void rc2d_audio_freeSource(RC2D_SourceAudio* sourceAudio)
{
	if (sourceAudio->sound != NULL)
	{
		Mix_FreeChunk(sourceAudio->sound);
		sourceAudio->sound = NULL;
	}
	else if (sourceAudio->music != NULL)
	{
		Mix_FreeMusic(sourceAudio->music);
		sourceAudio->music = NULL;
	}
}

void rc2d_audio_play(const RC2D_SourceAudio* sourceAudio, const int loops)
{
	if (sourceAudio->sound != NULL)
		Mix_PlayChannel(-1, sourceAudio->sound, loops);
	else if (sourceAudio->music != NULL)
		Mix_PlayMusic(sourceAudio->music, loops);
}

void rc2d_audio_pause(const RC2D_SourceAudio* sourceAudio)
{

	if (sourceAudio->sound != NULL)
	{
		//Mix_Pause(int channel);
	}
	else if (sourceAudio->music != NULL)
	{
		//Mix_Paused(int channel);
	}
}

void rc2d_audio_stop(const RC2D_SourceAudio* sourceAudio)
{
	if (sourceAudio->sound != NULL)
	{
		//Mix_Pause(int channel);
	}
	else if (sourceAudio->music != NULL)
	{
		//Mix_Paused(int channel);
	}
}

void rc2d_audio_setVolume(const int volume)
{
	if (volume >= RC2D_AUDIO_MIN_VOLUME && volume <= RC2D_AUDIO_MAX_VOLUME)
	{
		Mix_Volume(-1, volume);
		Mix_VolumeMusic(volume);
	}
}*/