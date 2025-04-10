
# RC2D Audio API Documentation

Welcome to the RC2D Audio API documentation. This section covers the functionalities provided by the RC2D game engine for audio management, including playing sounds and music, pausing, stopping, and adjusting volume.

::: tip Note
This API utilizes SDL_mixer for managing audio in games. Ensure SDL_mixer is correctly setup in your project.
:::

## Structures

### `RC2D_SourceAudio`

Represents an audio source, which can be either a sound effect or music.

- **Fields:**
  - `Mix_Chunk* sound`: Pointer to the sound chunk data.
  - `Mix_Music* music`: Pointer to the music data.

```c
typedef struct RC2D_SourceAudio {
  Mix_Chunk* sound;
  Mix_Music* music;
} RC2D_SourceAudio;
```

<br /><br />

## API Functions

### Creating a New Audio Source

Creates a new audio source from a file. Supports two types of audio sources: sound and music.

- `fileName`: Path to the audio file.
- `typeFile`: Type of the file (unused in the current version).
- `typeSourceAudio`: Specifies the type of audio source ("sound" or "music").

```c
RC2D_SourceAudio rc2d_audio_newSource(const char* fileName, const char* typeFile, const char* typeSourceAudio);
```

### Freeing an Audio Source

Frees the resources associated with an audio source.

::: warning Important
Make sure to free audio sources when they are no longer needed to avoid memory leaks.
:::

- `sourceAudio`: The `RC2D_SourceAudio` to be freed.

```c
void rc2d_audio_freeSource(RC2D_SourceAudio* sourceAudio);
```

### Playing an Audio Source

Plays the specified audio source. Can play in loops.

- `sourceAudio`: The `RC2D_SourceAudio` to play.
- `loops`: Number of times to loop the audio (-1 for infinite looping).

```c
void rc2d_audio_play(RC2D_SourceAudio sourceAudio, int loops);
```

### Pausing an Audio Source

Pauses the specified audio source.

- `sourceAudio`: The `RC2D_SourceAudio` to pause.

```c
void rc2d_audio_pause(RC2D_SourceAudio sourceAudio);
```

### Stopping an Audio Source

Stops the specified audio source.

- `sourceAudio`: The `RC2D_SourceAudio` to stop.

```c
void rc2d_audio_stop(RC2D_SourceAudio sourceAudio);
```

### Setting the Master Volume

Sets the master volume for the audio system.

- `volume`: Volume level from 0 (min) to 128 (max).

```c
void rc2d_audio_setVolume(int volume);
```

## Conclusion

This documentation provides an overview of the audio functionalities available in the RC2D game engine. For more detailed information or assistance, please refer to the official SDL_mixer documentation or the RC2D game engine support resources.
