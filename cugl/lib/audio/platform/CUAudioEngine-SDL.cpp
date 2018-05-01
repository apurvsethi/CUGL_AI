//
//  CUAudioEngine-impl.h
//  Cornell University Game Library (CUGL)
//
//  This module provides the SDL Mixer support for AudioEngine. This solution
//  works on all platforms.  However, it gives major deprecation errors for
//  iOS and OS X, and is not safe to use on those platforms.
//
//  On Apple platforms, you can switch between solutions by defining/undefining
//  the CU_AUDIO_AVFOUNDATION compiler variable.
//
//  CUGL zlib License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 12/10/16
//
#include "CUAudioEngine-impl.h"
#include <cugl/audio/CUMusic.h>
#include <cugl/audio/CUAudioEngine.h>
#include <cugl/util/CUDebug.h>
#include <SDL/SDL_mixer.h>
#include <vector>

/** The mixer block size */
// SMALLER = LESS LAG, BUT MORE CPU LOAD
#define MIX_BLOCK_SIZE  1024

namespace cugl {
namespace impl {
    
#pragma mark -
#pragma mark Basic Data Types
/**
 * Reference to the SDL mixer audio format.
 *
 * We need this struct because Mix_Chunk does not carry along is audio format
 * information (which we need for duration calculations).  In addition, this 
 * allows us to unify SDL mixer with AVFoundation.
 */
typedef struct AudioBuffer {
    /** The SDL mixer representation of a PCM Buffer */
    Mix_Chunk* chunk;
    /** The data format (e.g. bytes) of a single audio frame */
    Uint16 format;
    /** The number of audio frames in the buffer */
    Uint64 frames;
    /** The number of audio channels (1, 2, 4 or 6) */
    Uint32 channels;
    /** The audio sample rate in HZ */
    double bitrate;
} AudioBuffer;

/**
 * Reference to the SDL mixer music format.
 *
 * This struct is really nothing more than Mix_Music.  We only need this to
 * unify SDL mixer with AVFoundation.
 */
typedef struct AudioStream {
    Mix_Music* music;
} AudioStream;

/**
 * Reference to the SDL implementation of a sound channel.
 *
 * As with sound assets, SDL mixer does not carry along the audio format info
 * for each sound channel.  Therefore, we have added that in addition to the
 * channel id.  This allows us to unify SDL mixer with AVFoundation.
 */
typedef struct AudioChannel {
    /** The id for this player channel */
    Uint32 channel;
    /** The data format (e.g. bytes) of a single audio frame */
    Uint16 format;
    /** The number of audio channels (1, 2, 4 or 6) */
    Uint32 channels;
    /** The audio sample rate in HZ */
    double bitrate;
    /** Whether or not this channel was terminated manually */
    bool manual;
} AudioChannel;

/**
 * Reference to the SDL implementation of the music player.
 *
 * For the most part, we do not need any special information for a music
 * player, as it is bolted into the mixer.  However, we do need support
 * for rudimentary timing data, and that is included here.
 */
typedef struct AudioPlayer {
    /** The currently attached music asset */
    Mix_Music* music;
    /** The SDL time stamp at which the music was paused */
    Uint32 pauseTick;
    /** The SDL time stamp at which the music was started */
    Uint32 startTick;
    /** The position (in milliseconds) in the song where it was last started */
    Uint32 startTime;
    /** Whether or not this channel was terminated manually */
    bool manual;
} AudioPlayer;

/**
 * Reference to the SDL mixer audio engine
 *
 * This class stores the primitive audio "graph", tracking the players and
 * channels that have been allocated so far.
 */
typedef struct AudioMixer {
    AudioPlayer* background;
    std::vector< AudioChannel * > channels;
} AudioMixer;

/** The pointer to the engine root */
static AudioMixer* _engine;


#pragma mark -
#pragma mark Internal Helpers

/**
 * The SDL_Mixer callback parent for sound effects.
 *
 * We needed the SDL_Mixer callback as a regular C function as SDL does not
 * play well with C++ closures.
 */
void InternalChannelDone(int channel) {
    if (cugl::AudioEngine::get()) {
        AudioChannel* player = _engine->channels[channel];
        bool manual = player->manual;
        player->manual = false;
        cugl::AudioEngine::get()->gcEffect(channel,!manual);
    }
}

/**
 * The SDL_Mixer callback parent for music.
 *
 * We needed the SDL_Mixer callback as a regular C function as SDL does not
 * play well with C++ closures.
 */
void InternalMusicFinished() {
    if (cugl::AudioEngine::get()) {
        AudioPlayer* player = _engine->background;
        bool manual = player->manual;
        player->manual = false;
        player->music  = nullptr;
        cugl::AudioEngine::get()->gcMusic(!manual);
    }
}

/**
 * Initializes the audio engine for use.
 *
 * If you are using sound, this function should be one of the very first
 * things your application should call.  You cannot load sound or music
 * assets until this function is called. Once the audio engine is started,
 * it will continue running until stopped. It should be stopped on
 * application shutdown to prevent memory leaks.
 *
 * The audio engine should be defined with a default sampling frequency.
 * This is the ideal sampling frequency for sound and music assets. It is
 * not a good idea to the use assets with a different sampling frequency.
 *
 * While the sound engine can specify the number of output channels, the
 * only cross-platforms options are 1 (Mono) and 2 (Stereo). Cross-platform
 * 5.1 or 7.1 sound is not supported.
 *
 * @param frequency The default sampling frequency
 * @param input     The number of sound effect channels
 * @param output    The number of output channels
 */
bool AudioStart(int frequency, int input, int output) {
    CUAssertLog(!_engine, "Audio engine has already been started");
    if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, output, MIX_BLOCK_SIZE) == -1) {
        return false;
    }
    _engine = new AudioMixer();
    _engine->channels.resize(input, nullptr);
    Mix_AllocateChannels(input);
    Mix_ChannelFinished(InternalChannelDone);
    Mix_HookMusicFinished(InternalMusicFinished);
    return true;
}

/**
 * Stops the audio engine, preventing it from further use.
 *
 * This should be the very last thing called at application shutdown.  All
 * sound and music assets should be unloaded before this function is called.
 */
void AudioStop() {
    CUAssertLog(_engine, "Audio engine is not currently active");
    if (_engine->background) {
        AudioFreeBackground(_engine->background);
    }
    for(auto it = _engine->channels.begin(); it != _engine->channels.end(); ++it) {
        if (*it) {
            AudioFreeChannel(*it);
        }
    }
    
    delete _engine;
    _engine = nullptr;
    Mix_CloseAudio();
}

#pragma mark -
#pragma mark Sound Assets
/**
 * Returns an in-memory PCM buffer for the given audio asset
 *
 * This function will attempt to read the sound asset file.  If file is
 * a relative path, it will search in the asset directory.  Otherwise, it
 * will use the full path specified.
 *
 * The success of this function may depend on the platform.  Only WAV,
 * MP3, and OGG Vorbis files are cross-platform. Everything else (AAC,
 * M4A, FLAC) is platform-dependent.  If the function cannot decode the
 * file, it will return nullptr.
 *
 * @param file  The path (absolute or relative) for the sound asset
 *
 * @return an in-memory PCM buffer for the given audio asset
 */
AudioBuffer* AudioLoadBuffer(const char* file) {
    Mix_Chunk* data = Mix_LoadWAV(file);
    if (!data) {
        return nullptr;
    }
    
    AudioBuffer* buffer = new AudioBuffer();
    buffer->chunk = data;
    
    // Get the auxiliary information.
    int freq = 0;
    Uint16 fmt = 0;
    int chans = 0;
    
    // Chunks are converted to audio device format...
    if (!Mix_QuerySpec(&freq, &fmt, &chans)) {
        // never called Mix_OpenAudio()?!
        buffer->format = fmt;
        buffer->channels = 0;
        buffer->frames = 0;
        buffer->bitrate = 0.0;
    } else {
        buffer->format = fmt;
        buffer->channels = chans;
        buffer->bitrate  = freq;
        Uint32 points = (buffer->chunk->alen / ((fmt & 0xFF)/8));
        buffer->frames = (points / chans);
    }
    
    return buffer;
}

/**
 * Frees the given PCM buffer, releasing all resources
 *
 * @param source    The PCM buffer to free
 */
void AudioFreeBuffer(AudioBuffer* source) {
    if (source) {
        Mix_FreeChunk(source->chunk);
        source->chunk = nullptr;
        delete source;
    }
}

/**
 * Returns the number of audio frames for the given PCM buffer
 *
 * @param source    The PCM buffer
 *
 * @return the number of audio frames for the given PCM buffer
 */
Uint64 AudioGetBufferFrames(AudioBuffer* source) {
    return source->frames;
}

/**
 * Returns the number of audio channels for the given PCM buffer
 *
 * @param source    The PCM buffer
 *
 * @return the number of audio channels for the given PCM buffer
 */
Uint32 AudioGetBufferChannels(AudioBuffer* source) {
    return source->channels;
}

/**
 * Returns the number of sample rate (in HZ) for the given PCM buffer
 *
 * @param source    The PCM buffer
 *
 * @return the number of sample rate (in HZ) for the given PCM buffer
 */
double AudioGetBufferSampleRate(AudioBuffer* source) {
    return source->bitrate;
}

#pragma mark -
#pragma mark Music Assets
/**
 * Returns an audio stream for the given music asset
 *
 * This function will attempt to decode the music asset file.  If file is
 * a relative path, it will search in the asset directory.  Otherwise, it
 * will use the full path specified.
 *
 * The success of this function may depend on the platform.  Only WAV,
 * MP3, and OGG Vorbis files are cross-platform. Everything else (AAC,
 * M4A, FLAC) is platform-dependent.  If the function cannot decode the
 * file, it will return nullptr.
 *
 * @param file  The path (absolute or relative) for the sound asset
 *
 * @return an audio stream for the given music asset
 */
AudioStream* AudioLoadStream(const char* file) {
    Mix_Music* data = Mix_LoadMUS(file);
    if (!data) {
        return nullptr;
    }
    
    AudioStream* buffer = new AudioStream();
    buffer->music = data;
    return buffer;
}

/**
 * Frees the given audio stream, releasing all resources
 *
 * @param source    The audio stream to free
 */
void AudioFreeStream(AudioStream* source) {
    if (source) {
        Mix_FreeMusic(source->music);
        source->music = nullptr;
        delete source;
    }
}

/**
 * Returns the duration of the music asset in seconds
 *
 * This value is computed from the sample rate which is deliberately
 * hidden from the user, as that is not important for tream assets.
 *
 * @param source    The audio stream
 *
 * @return the duration of the music asset in seconds
 */
double AudioGetStreamDuration(AudioStream* source) {
    return Mix_GetMusicDuration(source->music);
}

/**
 * Returns the music type of this audio stream
 *
 * @param source    The audio stream
 *
 * @return the music type of this audio stream
 */
cugl::Music::Type AudioGetStreamType(AudioStream* source) {
    cugl::Music::Type result = cugl::Music::Type::UNSUPPORTED;
    switch (Mix_GetMusicType(source->music)) {
        case MUS_MP3:
            result = cugl::Music::Type::MP3;
            break;
        case MUS_WAV:
            result = cugl::Music::Type::WAV;
            break;
        case MUS_OGG:
            result = cugl::Music::Type::OGG;
            break;
        case MUS_FLAC:
            result = cugl::Music::Type::FLAC;
            break;
        default:
            result = cugl::Music::Type::UNSUPPORTED;
            break;
    }
    return result;
}


#pragma mark -
#pragma mark Sound Channels

/**
 * Returns a sound channel allocated for use with the audio engine
 *
 * The audio engine must be initialized for this function to succeed.  In
 * addtion, channel must be non-negative and less than the number of input
 * channels of the audio engine
 *
 * If the function cannot allocate a channel, it returns nullptr.  If this
 * channel has already been allocated, this returns a reference to the
 * previously allocated channel.  As a result, it is a bad idea to reallocate
 * a channel with the same id.
 *
 * @param channel   The id of the sound channel to allocate
 *
 * @return a sound channel allocated for use with the audio engine
 */
AudioChannel* AudioAllocChannel(int channel) {
    CUAssertLog(channel >= 0 && channel < Mix_AllocateChannels(-1),
                "Channel %d is not a valid channel",channel);
    CUAssertLog(!_engine->channels[channel], "Audio channel is already allocated");
    AudioChannel* player = new AudioChannel();
    if (player) {
        player->channel = channel;
        player->manual = false;
    }
    _engine->channels[channel] = player;
    return player;
}

/**
 * Frees the sound channel, releasing all resources
 *
 * Once this method is called, it is unsafe to play audio on this channel
 * any longer.
 *
 * @param channel   The sound channel to free
 */
void AudioFreeChannel(AudioChannel* player) {
    _engine->channels[player->channel] = nullptr;
    delete player;
}

/**
 * Plays the sound asset on the given channel
 *
 * The sound will play until it is finished.  If loop is true, it will
 * play indefinitely until halted explicilty.  When the audio halts it
 * will call the gcEffect() method in AudioEngine.
 *
 * If the asset is set to loop, it will loop back to the beginning, not
 * the beginning audio frame.
 *
 * @param player    The sound channel
 * @param source    The (PCM) sound asset
 * @param loop      Whether to loop the given asset
 * @param start     The audio frame to start playback
 */
void AudioPlayChannel(AudioChannel* player, AudioBuffer* source, bool loop, Uint32 start) {
    player->format   = source->format;
    player->channels = source->channels;
    player->bitrate  = source->bitrate;
    
    Mix_PlayChannel(player->channel, source->chunk, loop ? -1 : 0);
    
    // Adjust the byte offset if not at start
    if (start > 0) {
        // Convert frame to offset
        Uint64 offset = start*((source->format & 0xFF)/8)*source->channels;
        Mix_SetOffset(player->channel, (int)offset);
    }
}

/**
 * Halts the sound channel, garbage collecting the attached sound asset.
 *
 * This function will call the gcEffect() method in AudioEngine.
 *
 * @param player    The sound channel
 */
void AudioHaltChannel(AudioChannel* player) {
    player->manual = true;
    Mix_HaltChannel(player->channel);
}

/**
 * Halts the sound channel after the given number of milliseconds.
 *
 * Once this sound is complete, this function will call the gcEffect()
 * method in AudioEngine.
 *
 * @param player    The sound channel
 * @param millis    The number of millisecond before halting the asset
 */
void AudioExpireChannel(AudioChannel* player, Uint32 millis) {
    Mix_ExpireChannel(player->channel,millis);
}

/**
 * Pauses the sound channel
 *
 * The sound asset remains attached to the channel and will continue to
 * play once it resumes.  If this channel is already paused, this function
 * does nothing.
 *
 * @param player    The sound channel
 */
void AudioPauseChannel(AudioChannel* player) {
    Mix_Pause(player->channel);
}

/**
 * Resumes the sound channel
 *
 * Playback begins from the audio frame it was paused at.  If this channel
 * is paused, this function does nothing.
 *
 * @param player    The sound channel
 */
void AudioResumeChannel(AudioChannel* player) {
    Mix_Resume(player->channel);
}

/**
 * Returns true if this channel is actively playing.
 *
 * This function does not check if the channel is paused.
 *
 * @param player    The sound channel
 *
 * @return true if this channel is actively playing.
 */
bool AudioChannelPlaying(AudioChannel* player) {
    return (bool)Mix_Playing(player->channel);
}

/**
 * Returns true if this channel is actively paused.
 *
 * This function will also return false if the channel is not playing
 * anything at all.
 *
 * @param player    The sound channel
 *
 * @return true if this channel is actively paused.
 */
bool AudioChannelPaused(AudioChannel* player) {
    return (bool)Mix_Paused(player->channel);
}

/**
 * Sets the volume for this sound channel
 *
 * The value set should be 0 to 1 where 0 is no volume and 1 is maximum
 * volume.  Any value outside of this range has undefined behavior.
 *
 * @param player    The sound channel
 * @param volume   The volume (0 to 1) to play the asset
 */
void AudioSetChannelVolume(AudioChannel* player, float volume) {
    Mix_Volume(player->channel,(int)(MIX_MAX_VOLUME*volume));
}

/**
 * Sets the loop option for this sound channel
 *
 * This setting only affects the currently attached sound asset.  If loop
 * is true, the sound asset will continue to play indefinitely unless
 * stopped manually.  If it is set to false, it will terminate the asset
 * at the end of the natural loop point.
 *
 * @param player    The sound channel
 * @param loop      Whether to loop the current attached asset
 */
void AudioSetChannelLoop(AudioChannel* player, bool loop) {
    Mix_SetLoop(player->channel, loop ? -1 : 0);
}

/**
 * Returns the current audio frame of the given sound channel
 *
 * Together with the sampling rate, this can be used to compute the
 * number of seconds the player is into the sound asset.
 *
 * @param player    The sound channel
 *
 * @return the current audio frame of the given sound channel
 */
Uint64 AudioGetChannelFrame(AudioChannel* player) {
    return Mix_GetOffset(player->channel);
}

/**
 * Sets the current audio frame of the given sound channel
 *
 * This function will fast-forward or rewind the sound asset to the
 * given position.  This function will not pause or halt playback.
 *
 * @param player    The sound channel
 * @param frame     The audio frame to jump to
 */
void AudioSetChannelFrame(AudioChannel* player, Uint64 frame) {
    Uint64 offset = frame*((player->format & 0xFF)/8)*player->channels;
    Mix_SetOffset(player->channel,(int)offset);
}


#pragma mark -
#pragma mark Background Music
/**
 * Returns a music player allocated for use with the audio engine
 *
 * The audio engine must be initialized for this function to succeed.  If
 * the function cannot allocate the player, it returns nullptr.  If a
 * music playerhas already been allocated, this returns a reference to the
 * previously allocated player.  As a result, it is a bad idea to call this
 * function more than once before freeing the player.
 *
 * @return a music player allocated for use with the audio engine
 */
AudioPlayer* AudioAllocBackground() {
    if (!_engine->background) {
        AudioPlayer* player = new AudioPlayer();
        if (player) {
            player->music = nullptr;
            player->pauseTick = 0;
            player->startTick = 0;
        }
        _engine->background = player;
    }
    return _engine->background;
}

/**
 * Frees the music player, releasing all resources
 *
 * Once this method is called, it is unsafe to play music on this player
 * any longer.
 *
 * @param channel   The music player to free
 */
void AudioFreeBackground(AudioPlayer* player) {
    _engine->background = nullptr;
    delete player;
}

/**
 * Plays the music asset in the background
 *
 * The song will play until it is finished.  If loop is true, it will
 * play indefinitely until halted explicilty.  When the audio halts it
 * will call the gcMusic() method in AudioEngine.
 *
 * If the asset is set to loop, it will loop back to the beginning, not
 * the start position.
 *
 * @param player    The music player
 * @param source    The (streaming) audio asset
 * @param loop      Whether to loop the given asset
 * @param start     The position (in milliseconds) to start playback
 */
void AudioPlayBackground(AudioPlayer* player, AudioStream* source, bool loop, Uint32 start) {
    // This is the only way we might still be playing.
    if (Mix_FadingMusic()) {
        player->manual = true;
        Mix_HaltMusic();
    }
    
    player->startTick = SDL_GetTicks();
    player->pauseTick = 0;
    player->startTime = start;
    player->music = source->music;
    
    // Do this quietly
    int volume = Mix_VolumeMusic(0);
    Mix_PlayMusic(source->music,(loop ? -1 : 0));
    Mix_SetMusicPosition(start/1000.0);
    Mix_VolumeMusic(volume);
}

/**
 * Plays the music asset in the background, fading from 0 volume.
 *
 * The song will play until it is finished.  If loop is true, it will
 * play indefinitely until halted explicilty.  When the audio halts it
 * will call the gcMusic() method in AudioEngine.
 *
 * The song will start out at 0 volume, rising the current player volume
 * after fade milliseconds.  If the volume is changed before then (or
 * the song finishes early), the fade effect will be canceled, and the
 * player will restore to its natural volume.
 *
 * If the asset is set to loop, it will loop back to the beginning, not
 * the start position.
 *
 * @param player    The music player
 * @param source    The (streaming) audio asset
 * @param loop      Whether to loop the given asset
 * @param start     The position (in milliseconds) to start playback
 * @param start     The time (in milliseconds) to fade in playback
 */
void AudioFadeInBackground(AudioPlayer* player, AudioStream* source, bool loop, Uint32 start, Uint32 fade) {
    player->startTick = SDL_GetTicks();
    player->pauseTick = 0;
    player->startTime = start;
    player->music = source->music;

    Mix_FadeInMusic(source->music,(loop ? -1 : 0),fade);
    Mix_SetMusicPosition(start/1000.0);
}

/**
 * Halts the background music, garbage collecting the attached music asset.
 *
 * This function will call the gcMusic() method in AudioEngine.
 *
 * @param player    The music player
 */
void AudioHaltBackground(AudioPlayer* player) {
    player->manual = true;
    Mix_HaltMusic();
}

/**
 * Fades out the background music over given number of milliseconds.
 *
 * After the fade out is complete, the song will be halted and the player
 * will return to normal volume.  In addition, this function will then
 * call the gcMusic() method in AudioEngine.
 *
 * @param player    The music player
 * @param millis    The number of millisecond before halting the asset
 */
void AudioFadeOutBackground(AudioPlayer* player, Uint32 millis) {
    Mix_FadeOutMusic(millis);
}

/**
 * Pauses the music player
 *
 * The music asset remains attached to the player and will continue to
 * play once it resumes.  If this player is already paused, this function
 * does nothing.
 *
 * @param player    The music player
 */
void AudioPauseBackground(AudioPlayer* player) {
    player->pauseTick = SDL_GetTicks();
    Mix_PauseMusic();
}

/**
 * Resumes the music player
 *
 * Playback begins from the second it was paused at.  If this channel
 * is paused, this function does nothing.
 *
 * @param player    The music player
 */
void AudioResumeBackground(AudioPlayer* player) {
    player->startTime = (Uint32)AudioGetBackgroundTime(player);
    player->startTick = SDL_GetTicks();
    Mix_ResumeMusic();
}

/**
 * Returns true if the background music is actively playing.
 *
 * This function does not check if the player is paused.
 *
 * @param player    The music player
 *
 * @return true if the background music is actively playing.
 */
bool AudioBackgroundPlaying(AudioPlayer* player) {
    return (bool)Mix_PlayingMusic();
}


/**
 * Returns true if the background music is actively paused.
 *
 * This function will also return false if there is no music playing at all.
 *
 * @param player    The music player
 *
 * @return true if the background music is actively paused.
 */
bool AudioBackgroundPaused(AudioPlayer* player) {
    return (bool)Mix_PausedMusic();
}

/**
 * Sets the volume for the background music
 *
 * The value set should be 0 to 1 where 0 is no volume and 1 is maximum
 * volume.  Any value outside of this range has undefined behavior.
 *
 * @param player    The music player
 * @param volume    The volume (0 to 1) to play the asset
 */
void AudioSetBackgroundVolume(AudioPlayer* player, float volume) {
    Mix_VolumeMusic((int)(volume*MIX_MAX_VOLUME));
}

/**
 * Sets the loop option for this music player
 *
 * This setting only affects the currently attached music asset.  If loop
 * is true, the music asset will continue to play indefinitely unless
 * stopped manually.  If it is set to false, it will terminate the asset
 * at the end of the natural loop point.
 *
 * @param player    The music player
 * @param loop      Whether to loop the current attached asset
 */
void AudioSetBackgroundLoop(AudioPlayer* player, bool loop) {
    Mix_SetMusicLoop(loop ? -1 : 0);
}

/**
 * Returns the elapsed number of seconds of the audio stream
 *
 * If the music asset is playing in a loop, this function returns the
 * elapsed time since the beginning of the song.
 *
 * @param player    The music player
 *
 * @return the elapsed number of seconds of the audio stream
 */
double AudioGetBackgroundTime(AudioPlayer* player) {
    int millis = (int)(Mix_GetMusicDuration(player->music)*1000);

    int offset = 0;
    if (player->pauseTick >= player->startTick) {
        offset = (player->pauseTick - player->startTick);
    } else {
        offset = (SDL_GetTicks()-player->startTick);
    }
    
    // This handles loops properly
    int result = (offset+player->startTime) % millis;
    return result/1000.0;
}

/**
 * Sets the loop option for this music player
 *
 * This setting only affects the currently attached music asset.  If loop
 * is true, the music asset will continue to play indefinitely unless
 * stopped manually.  If it is set to false, it will terminate the asset
 * at the end of the natural loop point.
 *
 * @param player    The music player
 * @param loop      Whether to loop the current attached asset
 */
void  AudioSetBackgroundTime(AudioPlayer* player, double time) {
    if (Mix_SetMusicPosition(time) != -1) {
        player->startTick = SDL_GetTicks();
        player->startTime = (Uint32)(time*1000);
        if (Mix_PausedMusic()) {
            player->pauseTick = player->startTick;
        }
        return;
    }
    CULogError("Failed to set music position to time %3.f",time);
}


}
}



