//
//  CUAudioEngine-impl.h
//  Cornell University Game Library (CUGL)
//
//  This module provides platform specific support for AudioEngine.  We have
//  two options.  The first is to use SDL Mixer, which works on all platforms
//  but is ancient.  It also gives major deprecation errors for iOS and OS X
//  (and is not safe to use on those platforms).  For those platforms, we use
//  the Apple-specific AVFoundation library.
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
#ifndef __CU_AUDIO_ENGINE_IMPL_H__
#define __CU_AUDIO_ENGINE_IMPL_H__
#include <cugl/base/CUBase.h>
#include <cugl/audio/CUMusic.h>

namespace cugl {
namespace impl {

#pragma mark -
#pragma mark AudioEngine
    /**
     * Reference to an platform-specific audio format for PCM data
     *
     * Regardless of the platform, this is usually a data type storing
     * a byte-buffer of PCM data.
     *
     * In most cases, the decode is attached to an audio engine, and it is 
     * not safe to use it when the engine is not initialized or active.
     */
    struct AudioBuffer;
    
    /**
     * Reference to an platform-specific music format.
     *
     * Regardless of platform, this is usually a file handle with functions
     * extract pages of PCM data from the file.
     *
     * In most cases, the decode is attached to an audio engine, and it is
     * not safe to use it when the engine is not initialized or active.
     */
    struct AudioStream;

    /**
     * Reference to an platform-specific audio player for PCM Channels.
     *
     * This play is attached to an audio engine, it is not safe to use it 
     * when the engine is not initialized or active.
     */
    struct AudioChannel;

    /**
     * Reference to an platform-specific music player for streaming audio.
     *
     * This play is attached to an audio engine, it is not safe to use it
     * when the engine is not initialized or active.
     */
    struct AudioPlayer;
    
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
    bool AudioStart(int frequency, int input, int output);
    
    /**
     * Stops the audio engine preventing it from further use.
     *
     * This should be the very last thing called at application shutdown.  All
     * sound and music assets should be unloaded before this function is called.
     */
    void AudioStop();
    
    
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
    AudioBuffer* AudioLoadBuffer(const char* file);
    
    /**
     * Frees the given PCM buffer, releasing all resources
     *
     * @param source    The PCM buffer to free
     */
    void AudioFreeBuffer(AudioBuffer* source);
    
    /**
     * Returns the number of audio frames for the given PCM buffer
     *
     * @param source    The PCM buffer
     *
     * @return the number of audio frames for the given PCM buffer
     */
    Uint64 AudioGetBufferFrames(AudioBuffer* source);
    
    /**
     * Returns the number of audio channels for the given PCM buffer
     *
     * @param source    The PCM buffer
     *
     * @return the number of audio channels for the given PCM buffer
     */
    Uint32 AudioGetBufferChannels(AudioBuffer* source);
    
    /**
     * Returns the number of sample rate (in HZ) for the given PCM buffer
     *
     * @param source    The PCM buffer
     *
     * @return the number of sample rate (in HZ) for the given PCM buffer
     */
    double AudioGetBufferSampleRate(AudioBuffer* source);

    
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
    AudioStream* AudioLoadStream(const char* file);
    
    /**
     * Frees the given audio stream, releasing all resources
     *
     * @param source    The audio stream to free
     */
    void AudioFreeStream(AudioStream* source);
    
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
    double AudioGetStreamDuration(AudioStream* source);
    
    /**
     * Returns the music type of this audio stream
     *
     * @param source    The audio stream
     *
     * @return the music type of this audio stream
     */
    cugl::Music::Type AudioGetStreamType(AudioStream* source);
    
    
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
    AudioChannel* AudioAllocChannel(int channel);
    
    /**
     * Frees the sound channel, releasing all resources
     *
     * Once this method is called, it is unsafe to play audio on this channel
     * any longer.
     *
     * @param player   The sound channel to free
     */
    void AudioFreeChannel(AudioChannel* player);
    
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
    void AudioPlayChannel(AudioChannel* player, AudioBuffer* source, bool loop, Uint32 start=0);
    
    /**
     * Halts the sound channel, garbage collecting the attached sound asset.
     *
     * This function will call the gcEffect() method in AudioEngine.
     *
     * @param player    The sound channel
     */
    void AudioHaltChannel(AudioChannel* player);

    /**
     * Halts the sound channel after the given number of milliseconds.
     *
     * Once this sound is complete, this function will call the gcEffect() 
     * method in AudioEngine.
     *
     * @param player    The sound channel
     * @param millis    The number of millisecond before halting the asset
     */
    void AudioExpireChannel(AudioChannel* player, Uint32 millis);
    
    /**
     * Pauses the sound channel
     *
     * The sound asset remains attached to the channel and will continue to
     * play once it resumes.  If this channel is already paused, this function
     * does nothing.
     *
     * @param player    The sound channel
     */
    void AudioPauseChannel(AudioChannel* player);

    /**
     * Resumes the sound channel
     *
     * Playback begins from the audio frame it was paused at.  If this channel 
     * is paused, this function does nothing.
     *
     * @param player    The sound channel
     */
    void AudioResumeChannel(AudioChannel* player);
    
    /**
     * Returns true if this channel is actively playing.
     *
     * This function does not check if the channel is paused.
     *
     * @param player    The sound channel
     *
     * @return true if this channel is actively playing.
     */
    bool AudioChannelPlaying(AudioChannel* player);

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
    bool AudioChannelPaused(AudioChannel* player);

    /**
     * Sets the volume for this sound channel
     *
     * The value set should be 0 to 1 where 0 is no volume and 1 is maximum
     * volume.  Any value outside of this range has undefined behavior.
     *
     * @param player    The sound channel
     * @param volume   The volume (0 to 1) to play the asset
     */
    void AudioSetChannelVolume(AudioChannel* player, float volume);

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
    void AudioSetChannelLoop(AudioChannel* player, bool loop);
    
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
    Uint64 AudioGetChannelFrame(AudioChannel* player);
    
    /**
     * Sets the current audio frame of the given sound channel
     *
     * This function will fast-forward or rewind the sound asset to the
     * given position.  This function will not pause or halt playback.
     *
     * @param player    The sound channel
     * @param frame     The audio frame to jump to
     */
    void   AudioSetChannelFrame(AudioChannel* player, Uint64 frame);
    
    
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
    AudioPlayer* AudioAllocBackground();
    
    /**
     * Frees the music player, releasing all resources
     *
     * Once this method is called, it is unsafe to play music on this player
     * any longer.
     *
     * @param player    The music player to free
     */
    void AudioFreeBackground(AudioPlayer* player);
    
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
    void AudioPlayBackground(AudioPlayer* player, AudioStream* source, bool loop, Uint32 start=0);
    
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
     * @param fade      The time (in milliseconds) to fade in playback
     */
    void AudioFadeInBackground(AudioPlayer* player, AudioStream* source, bool loop, Uint32 start=0, Uint32 fade=0);

    /**
     * Halts the background music, garbage collecting the attached music asset.
     *
     * This function will call the gcMusic() method in AudioEngine.
     *
     * @param player    The music player
     */
    void AudioHaltBackground(AudioPlayer* player);
    
    /**
     * Fades out the background music over given number of milliseconds.
     *
     * After the fade out is complete, the song will be halted and the player
     * will return to normal volume.  In addition, this function will then
     * call the gcMusic() method in AudioEngine.
     *
     * @param player    The music player
     * @param fade      The number of millisecond before halting the asset
     */
    void AudioFadeOutBackground(AudioPlayer* player, Uint32 fade);
    
    /**
     * Pauses the music player
     *
     * The music asset remains attached to the player and will continue to
     * play once it resumes.  If this player is already paused, this function
     * does nothing.
     *
     * @param player    The music player
     */
    void AudioPauseBackground(AudioPlayer* player);
    
    /**
     * Resumes the music player
     *
     * Playback begins from the second it was paused at.  If this channel
     * is paused, this function does nothing.
     *
     * @param player    The music player
     */
    void AudioResumeBackground(AudioPlayer* player);
    
    /**
     * Returns true if the background music is actively playing.
     *
     * This function does not check if the player is paused.
     *
     * @param player    The music player
     *
     * @return true if the background music is actively playing.
     */
    bool AudioBackgroundPlaying(AudioPlayer* player);
    
    /**
     * Returns true if the background music is actively paused.
     *
     * This function will also return false if there is no music playing at all.
     *
     * @param player    The music player
     *
     * @return true if the background music is actively paused.
     */
    bool AudioBackgroundPaused(AudioPlayer* player);

    /**
     * Sets the volume for the background music
     *
     * The value set should be 0 to 1 where 0 is no volume and 1 is maximum
     * volume.  Any value outside of this range has undefined behavior.
     *
     * @param player    The music player
     * @param volume    The volume (0 to 1) to play the asset
     */
    void AudioSetBackgroundVolume(AudioPlayer* player, float volume);

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
    void AudioSetBackgroundLoop(AudioPlayer* player, bool loop);
    
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
    double AudioGetBackgroundTime(AudioPlayer* player);
    
    /**
     * Sets the elapsed number of seconds of the audio stream
     *
     * This function will fast-forward or rewind the sound asset to the
     * given position.  This function will not pause or halt playback.
     *
     * @param player    The sound channel
     * @param time      The position to jump to
     */
    void   AudioSetBackgroundTime(AudioPlayer* player, double time);

}
}

#endif /* __CU_AUDIO_ENGINE_IMPL_H__ */
