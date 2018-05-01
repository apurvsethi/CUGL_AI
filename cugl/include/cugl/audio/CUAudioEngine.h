//
//  CUAudioEngine.h
//  Cornell University Game Library (CUGL)
//
//  This module is a singleton providing a simple 2000-era audio engine.  Like
//  all engines of this era, it provides a flat channel structure for playing
//  sounds as well as a single channel for background music.  This is much
//  more primitive than modern sound engines, with the advantage that it is
//  much simpler to use.
//
//  Because this is a singleton, there are no publicly accessible constructors
//  or intializers.  Use the static methods instead.
//
//  We have factored out the platform-dependent code and have hidden it
//  behind this class as an abstraction.
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
#ifndef __CU_AUDIO_ENGINE_H__
#define __CU_AUDIO_ENGINE_H__
#include <cugl/audio/CUSound.h>
#include <cugl/audio/CUMusic.h>
#include <cugl/util/CUTimestamp.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include <deque>

/** The maximum number of buffer channels */
#define AUDIO_INPUT_CHANNELS  24
/** The number of output channels */
#define AUDIO_OUTPUT_CHANNELS 2
/** The default sampling frequency */
#define AUDIO_FREQUENCY 44100

/** Comment this out to use SDL sound on Mac/iOS (not recommended) */
#if defined (__MACOSX__) || defined (__IPHONEOS__)
    #define CU_AUDIO_AVFOUNDATION 1
#endif


namespace cugl {
    
/** Opaque reference to "hidden" package class for a sound effects channel */
class SoundChannel;
/** Opaque reference to "hidden" package class for the music queue */
class MusicQueue;
    
/**
 * Class provides a singleton audio engine
 *
 * This class is a simple (e.g. 2000-era) audio engine.  It exposes a flat 
 * channel structure and there is no mixer graph for advanced effects such
 * as 3D audio.  If you need these features, use {@link AudioEngineHD} instead.
 *
 * This class allows one music asset to be played at a time, though it does
 * allow you to queue up music asset.  All other sounds should be preloaded.
 * The user specifies sound instances (there may be may sound instances for
 * a single asset) by a predefined key.  This cuts down on the overhead of 
 * managing the sound identifier. 
 *
 * You cannot create new instances of this class.  Instead, you should access 
 * the singleton through the three static methods: start(), stop() and get().
 *
 * IMPORTANT: Like the OpenGL context, this class is not thread-safe.  It is
 * only safe to access this class in the main application thread.  This means
 * it should never be called in a call-back function as those are typically
 * executed in the host thread.  If you need to access the AudioEngine in a
 * callback function, you should use the {@link Application#schedule} method
 * to delay until the main thread is next available.
 */
class AudioEngine {
#pragma mark -
#pragma mark Sound State
public:
    /**
     * This enumeration provides a way to determine the state of a channel     
     */
    enum class State {
        /** This sound channel is not actually active */
        INACTIVE,
        /** This sound is active and currently playing */
        PLAYING,
        /** This sound is active but is currently paused */
        PAUSED
    };

    
private:
    /** Reference to the audio engine singleton */
    static AudioEngine* _gEngine;
    
    /** The queue for handling music assets */
    std::shared_ptr<MusicQueue> _mqueue;
    
    /** The number of supported audio channels */
    unsigned int _capacity;
    /** The channel objects for managing sounds */
    std::vector<std::shared_ptr<SoundChannel>> _channels;
    /** Map keys to identifiers */
    std::unordered_map<std::string,int> _effects;
    /** The queue for subsequent sound loops */
    std::deque<std::string> _equeue;
    
    /** 
     * Callback function for background music
     *
     * This function is called whenever a background music track completes.
     * It is called whether or not the music completed normally or if it
     * was terminated manually.  However, the second parameter can be used
     * to distinguish the two cases.
     *
     * @param asset     The music asset that just completed
     * @param status    True if the music terminated normally, false otherwise.
     */
    std::function<void(Music* asset, bool status)> _musicCB;

    /**
     * Callback function for the sound effects
     *
     * This function is called whenever a sound effect completes. It is called
     * whether or not the sound completed normally or if it was terminated 
     * manually.  However, the second parameter can be used to distinguish the 
     * two cases.
     *
     * @param key       The key identifying this sound effect
     * @param status    True if the music terminated normally, false otherwise.
     */
    std::function<void(const std::string& key , bool status)> _soundCB;
    
#pragma mark -
#pragma mark Constructors (Private)
    /**
     * Creates, but does not initialize the singleton audio engine
     *
     * The engine must be initialized before is can be used.
     */
    AudioEngine() : _capacity(0) {}
    
    /**
     * Disposes of the singleton audio engine.
     *
     * This destructor releases all of the resources associated with this 
     * audio engine.  Sounds and music assets can no longer be loaded.
     */
    ~AudioEngine() { dispose(); }
    
    /**
     * Initializes the audio engine.
     *
     * This method initializes the platform-specific audio engine and constructs
     * the mixer graph for the sound effect channels.  The provided parameter
     * indicates the number of simultaneously supported sounds.
     *
     * @param channels  The maximum number of sound effect channels to support
     *
     * @return true if the audio engine was successfully initialized.
     */
    bool init(unsigned int channels=AUDIO_INPUT_CHANNELS);
    
    /**
     * Releases all resources for this singleton audio engine.
     *
     * Sounds and music assets can no longer be loaded. If you need to use the 
     * engine again, you must call init().
     */
    void dispose();

    
#pragma mark -
#pragma mark Internal Helpers
    /**
     * Purges this key from the list of active effects.
     *
     * This method is not the same as stopping the channel. A channel may play a
     * little longer after the key is removed.  This is simply a clean-up method.
     *
     * @remove key  The key to purge from the list of active effects.
     */
    void removeKey(std::string key);
    
    
#pragma mark -
#pragma mark Static Accessors
public:
    /**
     * Returns the singleton instance of the audio engine.
     *
     * If the audio engine has not been started, then this method will return 
     * nullptr.
     *
     * @return the singleton instance of the audio engine.
     */
    static AudioEngine* get() { return _gEngine; }
    
    /**
     * Starts the singleton audio engine.
     *
     * Once this method is called, the method get() will no longer return
     * nullptr.  Calling the method multiple times (without calling stop) will 
     * have no effect.
     *
     * Sounds and music cannot be loaded until this engine is initialized.
     * They depend on this engine for asset management.
     * 
     * The provided parameter indicates the number of simultaneously supported 
     * sounds.
     *
     * @param channels  The maximum number of sound effect channels to support
     */
    static void start(unsigned int channels=AUDIO_INPUT_CHANNELS);
    
    /**
     * Stops the singleton audio engine, releasing all resources.
     *
     * Once this method is called, the method get() will return nullptr.
     * Calling the method multiple times (without calling stop) will have 
     * no effect.
     *
     * Sounds and music cannot be loaded until this engine is initialized
     * again. They depend on this engine for asset management.
     */
    static void stop();
    
    
#pragma mark -
#pragma mark Music Management
    /**
     * Plays given music asset as a background track.
     *
     * Music is handled differently from sound effects.  Music is streamed 
     * instead of being pre-loaded, so your music files can be much larger.
     * In addition, you can only play one music asset at a time.  However,
     * it is possible to queue music assets for immediate playback once the
     * active asset is finished.
     *
     * This method immediately plays the provided asset. Hence it overrides 
     * and clears the music queue. To safely play an asset without affecting 
     * the music queue, use the method {@link queueMusic} instead.
     *
     * When it begins playing, it will start at full volume unless you
     * provide a number of seconds to fade in. Note that looping a song will 
     * cause it to block the queue indefinitely until you turn off looping for 
     * that asset {@see setLoop}. This can be desired behavior, as it gives you
     * a way to control the speed of the queue processing.
     *
     * @param music     The music asset to play
     * @param loop      Whether to loop the music continuously
     * @param volume    The music volume (< 0 to use asset default volume)
     * @param fade      The number of seconds to fade in
     */
    void playMusic(const std::shared_ptr<Music>& music, bool loop=false, float volume=-1.0f, float fade=0.0f);
    
    /**
     * Returns the music asset currently playing
     *
     * If there is no active background music, this method returns nullptr.
     *
     * @return the music asset currently playing
     */
    const Music* currentMusic() const;
    
    /** 
     * Returns the current state of the background music
     *
     * @return the current state of the background music
     */
    State getMusicState() const;
    
    /**
     * Returns true if the background music is in a continuous loop.
     *
     * If there is no active background music, this method will return false.
     *
     * @return true if the background music is in a continuous loop.
     */
    bool isMusicLoop() const;
    
    /**
     * Sets whether the background music is on a continuous loop.
     *
     * If loop is true, this will block the queue until it is set to false 
     * again. This can be desired behavior, as it gives you a way to control 
     * the speed of the queue processing.
     *
     * If there is no active background music, this method will raise an error.
     *
     * @param  loop  whether the background music should be on a continuous loop
     */
    void setMusicLoop(bool loop);  // Clears the queue
    
    /**
     * Returns the volume of the background music
     *
     * If there is no active background music, this method will return 0.
     *
     * @return the volume of the background music
     */
    float getMusicVolume() const;
    
    /**
     * Sets the volume of the background music
     *
     * If there is no active background music, this method will raise an error.
     *
     * @param  volume   the volume of the background music
     */
    void setMusicVolume(float volume);
    
    /**
     * Returns the length of background music, in seconds.
     *
     * This is only the duration of the active background music.  All other 
     * music in the queue is ignored. If there is no active background music,
     * this method will return 0.
     *
     * This information is retrieved from the decoder. As the file is completely
     * decoded at load time, the result of this method is reasonably accurate.
     *
     * @return the length of background music, in seconds.
     */
    float getMusicDuration() const;
    
    /**
     * Returns the elapsed time of the background music, in seconds
     *
     * The elapsed time is the current position of the music from the beginning.
     * It does not include any time spent on a continuous loop. If there is no 
     * active background music, this method will return 0.
     *
     * This information is not guaranteed to be accurate.  Attempting to time
     * the playback of streaming data (as opposed to a fully in-memory PCM
     * buffer) is very difficult and not cross-platform.  We have tried to be
     * reasonably accurate, but from our tests we can only guarantee accuracy
     * within a 10th of a second.
     *
     * @return the elapsed time of the background music, in seconds
     */
    float getMusicElapsed() const;
    
    /**
     * Returns the time remaining for the background music, in seconds
     *
     * The time remaining is just duration-elapsed.  This method does not take 
     * into account whether the music is on a loop. It also does not include 
     * the duration of any music waiting in the queue. If there is no active 
     * background music, this method will return 0.
     *
     * This information is not guaranteed to be accurate.  Attempting to time
     * the playback of streaming data (as opposed to a fully in-memory PCM
     * buffer) is very difficult and not cross-platform.  We have tried to be
     * reasonably accurate, but from our tests we can only guarantee accuracy
     * within a 10th of a second.
     *
     * @return the time remaining for the background music, in seconds
     */
    float getMusicRemaining() const;
    
    /**
     * Sets the elapsed time of the background music, in seconds
     *
     * The elapsed time is the current position of the music from the beginning. 
     * It does not include any time spent on a continuous loop.
     *
     * This adjustment is not guaranteed to be accurate.  Attempting to time
     * the playback of streaming data (as opposed to a fully in-memory PCM
     * buffer) is very difficult and not cross-platform.  We have tried to be
     * reasonably accurate, but from our tests we can only guarantee accuracy
     * within a 10th of a second.
     *
     * If there is no active background music, this method will raise an error.
     *
     * @param  time  the new position of the background music
     */
    void setMusicElapsed(float time);
    
    /**
     * Sets the time remaining for the background music, in seconds
     *
     * The time remaining is just duration-elapsed.  It does not take into 
     * account whether the music is on a loop. It also does not include the
     * duration of any music waiting in the queue.
     *
     * If there is no active background music, this method will raise an error.
     *
     * This adjustment is not guaranteed to be accurate.  Attempting to time
     * the playback of streaming data (as opposed to a fully in-memory PCM
     * buffer) is very difficult and not cross-platform.  We have tried to be
     * reasonably accurate, but from our tests we can only guarantee accuracy
     * within a 10th of a second.
     *
     * @param  time  the new time remaining of the background music
     */
    void setMusicRemaining(float time);
    
    /**
     * Stops the background music and clears the entire queue.
     *
     * Before the music is stopped, this method gives the user an option to
     * fade out the music.  If the argument is 0, it will halt the music
     * immediately. Otherwise it will fade to completion over the given number
     * of seconds (or until the end of the song).
     *
     * This method also clears the queue of any further music.
     *
     * @param fade  The number of seconds to fade out
     */
    void stopMusic(float fade=0.0f);
    
    /**
     * Clears the music queue, but does not release any other resources.
     *
     * This method does not stop the current background music from playing. It
     * only clears pending music assets from the queue.
     */
    void clearMusicQueue();
    
    /**
     * Pauses the background music, allowing it to be resumed later.
     *
     * This method has no effect on the music queue.
     */
    void pauseMusic();
    
    /**
     * Resumes the background music assuming that it was paused previously.
     *
     * This method has no effect on the music queue.
     */
    void resumeMusic();
    
    /**
     * Sets the callback for background music
     *
     * This callback function is called whenever a background music track
     * completes. It is called whether or not the music completed normally or
     * if it was terminated manually.  However, the second parameter can be
     * used to distinguish the two cases.
     *
     * @param callback The callback for background music
     */
    void setMusicListener(std::function<void(Music*,bool)> callback) {
        _musicCB = callback;
    }
    
    /**
     * Returns the callback for background music
     *
     * This callback function is called whenever a background music track
     * completes. It is called whether or not the music completed normally or
     * if it was terminated manually.  However, the second parameter can be
     * used to distinguish the two cases.
     *
     * @return the callback for background music
     */
    std::function<void(Music*,bool)> getMusicListener() const {
        return _musicCB;
    }
    
    /**
     * Callback function for when a music channel finishes
     *
     * This method is called when the active music completes.  If there is any
     * music waiting in the queue, it plays it immediately (using the second
     * channel for a smooth transition).  Otherwise, it sets _musicData to
     * nullptr.
     *
     * This method is never intended to be accessed by general users.  It is
     * only publicly visible because this makes our cross-platform code cleaner.
     *
     * @param status    True if the music terminated normally, false otherwise.
     */
    void gcMusic(bool status);
    
    
#pragma mark -
#pragma mark Music Queue
    /**
     * Adds the given music asset to the background music queue
     *
     * Music is handled differently from sound effects.  Music is streamed
     * instead of being pre-loaded, so your music files can be much larger.
     * In addition, you can only play one music asset at a time.  However,
     * it is possible to queue music assets for immediate playback once the
     * active asset is finished.
     *
     * If the queue is empty and there is no active music, this method will 
     * play the music immediately.  Otherwise, it will add the music to the 
     * queue, and it will play as soon as it is removed from the queue.
     *
     * When it begins playing, it will start at full volume unless you
     * provide a number of seconds to fade in. Note that looping a song will
     * cause it to block the queue indefinitely until you turn off looping for
     * that asset {@see setLoop}. This can be desired behavior, as it gives you
     * a way to control the speed of the queue processing.
     *
     * @param music     The music asset to queue
     * @param loop      Whether to loop the music continuously
     * @param volume    The music volume (< 0 to use asset default volume)
     * @param fade      The number of seconds to fade in
     */
    void queueMusic(const std::shared_ptr<Music>& music, bool loop=false, float volume=-1.0f, float fade=0.0f);
    
    /**
     * Returns the list of assets for the music queue
     *
     * @return the list of assets for the music queue
     */
    const std::vector<const Music*> getMusicQueue() const;
    
    /**
     * Returns the size of the music queue
     *
     * @return the size of the music queue
     */
    size_t getMusicQueueSize() const;
    
    /**
     * Advances ahead in the music queue.
     *
     * The value step is the number of songs to skip over. A value of 0 will
     * simply skip over the active music to the next element of the queue. Each
     * value above 0 will skip over one more element in the queue.  If this
     * skipping empties the queue, no music will play.
     *
     * @param  steps    number of elements to skip in the queue
     */
    void advanceMusicQueue(unsigned int steps=0);
    
    
#pragma mark -
#pragma mark Sound Effect Management
    /**
     * Plays the given sound effect, and associates it with the specified key.
     *
     * Sound effects are associated with a reference key.  This allows the 
     * application to easily reference the sound state without having to 
     * internally manage pointers to the audio channel.
     *
     * If the key is already associated with an active sound channel, this 
     * method will stop the existing sound and replace it with this one.  It 
     * is the responsibility of the application layer to manage key usage.
     *
     * There are a limited number of channels available for sound effects.  If 
     * you go over the number available, the sound will not play unless force 
     * is true. In that case, it will grab the channel from the longest playing 
     * sound effect.
     *
     * @param  key      The reference key for the sound effect
     * @param  sound    The sound effect to play
     * @param  loop     Whether to loop the sound effect continuously
     * @param  volume   The sound effect (< 0 to use asset default volume)
     * @param  force    Whether to force another sound to stop.
     *
     * @return true if there was an available channel for the sound
     */
    bool playEffect(const std::string& key, const std::shared_ptr<Sound>& sound,
                    bool loop=false, float volume=-1.0f, bool force=false);

    /**
     * Plays the given sound effect, and associates it with the specified key.
     *
     * Sound effects are associated with a reference key.  This allows the
     * application to easily reference the sound state without having to
     * internally manage pointers to the audio channel.
     *
     * If the key is already associated with an active sound channel, this
     * method will stop the existing sound and replace it with this one.  It
     * is the responsibility of the application layer to manage key usage.
     *
     * There are a limited number of channels available for sound effects.  If
     * you go over the number available, the sound will not play unless force
     * is true. In that case, it will grab the channel from the longest playing
     * sound effect.
     *
     * @param  key      The reference key for the sound effect
     * @param  sound    The sound effect to play
     * @param  loop     Whether to loop the sound effect continuously
     * @param  volume   The sound effect (< 0 to use asset default volume)
     * @param  force    Whether to force another sound to stop.
     *
     * @return true if there was an available channel for the sound
     */
    bool playEffect(const char* key, const std::shared_ptr<Sound>& sound,
                    bool loop=false, float volume=-1.0f, bool force=false) {
        return playEffect(std::string(key),sound,loop,volume,force);
    }

    /**
     * Returns the number of channels available for sound effects.
     *
     * There are a limited number of channels available for sound effects.  If
     * all channels are in use, this method will return 0. If you go over the 
     * number available, you cannot play another sound unless you force it. In 
     * that case, it will grab the channel from the longest playing sound effect.
     *
     * @return the number of channels available for sound effects.
     */
    size_t getAvailableChannels() const {
        return (size_t)_capacity-_effects.size();
    }
    
    /**
     * Returns the current state of the sound effect for the given key.
     *
     * If there is no sound effect for the given key, it returns
     * State::INACTIVE.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the current state of the sound effect for the given key.
     */
    State getEffectState(const std::string& key) const;

    /**
     * Returns the current state of the sound effect for the given key.
     *
     * If there is no sound effect for the given key, it returns
     * State::INACTIVE.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the current state of the sound effect for the given key.
     */
    State getEffectState(const char* key) const {
        return getEffectState(std::string(key));
    }
    
    /**
     * Returns true if the key is associated with an active channel.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return true if the key is associated with an active channel.
     */
    bool isActiveEffect(const std::string& key) const {
        return _effects.find(key) != _effects.end();
    }

    /**
     * Returns true if the key is associated with an active channel.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return true if the key is associated with an active channel.
     */
    bool isActiveEffect(const char* key) const {
        return isActiveEffect(std::string(key));
    }
    
    /**
     * Returns the sound asset attached to the given key.
     *
     * If there is no active sound effect for the given key, this method
     * returns nullptr.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the sound asset attached to the given key.
     */
    const Sound* currentEffect(const std::string& key) const;

    /**
     * Returns the sound asset attached to the given key.
     *
     * If there is no active sound effect for the given key, this method
     * returns nullptr.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the sound asset attached to the given key.
     */
    const Sound* currentEffect(const char* key) const {
        return currentEffect(std::string(key));
    }

    /**
     * Returns true if the sound effect is in a continuous loop.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return true if the sound effect is in a continuous loop.
     */
    bool isEffectLoop(const std::string& key) const;

    /**
     * Returns true if the sound effect is in a continuous loop.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return true if the sound effect is in a continuous loop.
     */
    bool isEffectLoop(const char* key) const {
        return isEffectLoop(std::string(key));
    }
    
    /**
     * Sets whether the sound effect is in a continuous loop.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  loop     whether the sound effect is in a continuous loop
     */
    void setEffectLoop(const std::string& key, bool loop);

    /**
     * Sets whether the sound effect is in a continuous loop.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  loop     whether the sound effect is in a continuous loop
     */
    void setEffectLoop(const char* key, bool loop) {
        setEffectLoop(std::string(key),loop);
    }
    
    /**
     * Returns the current volume of the sound effect.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the current volume of the sound effect
     */
    float getEffectVolume(const std::string& key) const;
    
    /**
     * Returns the current volume of the sound effect.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the current volume of the sound effect
     */
    float getEffectVolume(const char* key) const {
        return getEffectVolume(std::string(key));
    }
    
    /**
     * Sets the current volume of the sound effect.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  volume   the current volume of the sound effect
     */
    void setEffectVolume(const std::string& key, float volume);

    /**
     * Sets the current volume of the sound effect.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  volume   the current volume of the sound effect
     */
    void setEffectVolume(const char* key, float volume) {
        setEffectVolume(std::string(key),volume);
    }

    /**
     * Returns the duration of the sound effect, in seconds.
     *
     * Because the asset is fully decompressed at load time, the result of this 
     * method is reasonably accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the duration of the sound effect, in seconds.
     */
    float getEffectDuration(const std::string& key) const;

    /**
     * Returns the duration of the sound effect, in seconds.
     *
     * Because the asset is fully decompressed at load time, the result of this
     * method is reasonably accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the duration of the sound effect, in seconds.
     */
    float getEffectDuration(const char* key) const {
        return getEffectDuration(std::string(key));
    }
    
    /**
     * Returns the elapsed time of the sound effect, in seconds
     *
     * The elapsed time is the current position of the sound from the beginning. 
     * It does not include any time spent on a continuous loop. Because the 
     * asset is fully decompressed at load time, the result of this method is 
     * reasonably accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the elapsed time of the sound effect, in seconds
     */
    float getEffectElapsed(const std::string& key) const;
    
    /**
     * Returns the elapsed time of the sound effect, in seconds
     *
     * The elapsed time is the current position of the sound from the beginning.
     * It does not include any time spent on a continuous loop. Because the
     * asset is fully decompressed at load time, the result of this method is
     * reasonably accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the elapsed time of the sound effect, in seconds
     */
    float getEffectElapsed(const char* key) const {
        return getEffectElapsed(std::string(key));
    }
    
    /**
     * Returns the time remaining for the sound effect, in seconds
     *
     * The time remaining is just duration-elapsed.  This method does not take 
     * into account whether the sound is on a loop. Because the asset is fully
     * decompressed at load time, the result of this method is reasonably 
     * accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the time remaining for the sound effect, in seconds
     */
    float getEffectRemaining(const std::string& key) const;

    /**
     * Returns the time remaining for the sound effect, in seconds
     *
     * The time remaining is just duration-elapsed.  This method does not take
     * into account whether the sound is on a loop. Because the asset is fully
     * decompressed at load time, the result of this method is reasonably
     * accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the time remaining for the sound effect
     */
    float getEffectRemaining(const char* key) const {
        return getEffectRemaining(std::string(key));
    }

    /**
     * Sets the elapsed time of the sound effect, in seconds
     *
     * The elapsed time is the current position of the sound from the beginning.  
     * It does not include any time spent on a continuous loop.  Because the
     * asset is fully decompressed at load time, the result of this method is
     * reasonably accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  time     the new position of the sound effect
     */
    void setEffectElapsed(const std::string& key, float time);

    /**
     * Sets the elapsed time of the sound effect, in seconds
     *
     * The elapsed time is the current position of the sound from the beginning.
     * It does not include any time spent on a continuous loop.  Because the
     * asset is fully decompressed at load time, the result of this method is
     * reasonably accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  time     the new position of the sound effect
     */
    void setEffectElapsed(const char* key, float time) {
        setEffectElapsed(std::string(key),time);
    }
    
    /**
     * Sets the time remaining for the sound effect, in seconds
     *
     * The time remaining is just duration-elapsed.  This method does not take 
     * into account whether the sound is on a loop. Because the asset is fully
     * decompressed at load time, the result of this method is reasonably
     * accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  time     the new time remaining for the sound effect
     */
    void setEffectRemaining(const std::string& key, float time);

    /**
     * Sets the time remaining for the sound effect, in seconds
     *
     * The time remaining is just duration-elapsed.  This method does not take
     * into account whether the sound is on a loop. Because the asset is fully
     * decompressed at load time, the result of this method is reasonably
     * accurate.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  time     the new time remaining for the sound effect
     */
    void setEffectRemaining(const char* key, float time) {
        setEffectRemaining(std::string(key),time);
    }
    
    /**
     * Stops the sound effect for the given key, removing it.
     *
     * If the key does not correspond to a channel, this method raises an error.
     * The effect will be removed from the audio engine entirely.  You will need
     * to add it again if you wish to replay it.
     *
     * @param  key      the reference key for the sound effect
     */
    void stopEffect(const std::string& key);
    
    /**
     * Stops the sound effect for the given key, removing it.
     *
     * If the key does not correspond to a channel, this method raises an error.
     * The effect will be removed from the audio engine entirely.  You will need
     * to add it again if you wish to replay it.
     *
     * @param  key      the reference key for the sound effect
     */
    void stopEffect(const char* key) {
        stopEffect(std::string(key));
    }
    
    /**
     * Pauses the sound effect for the given key.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     */
    void pauseEffect(const std::string& key);

    /**
     * Pauses the sound effect for the given key.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     */
    void pauseEffect(const char* key) {
        pauseEffect(std::string(key));
    }

    /**
     * Resumes the sound effect for the given key.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     */
    void resumeEffect(std::string key);

    /**
     * Resumes the sound effect for the given key.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     */
    void resumeEffect(const char* key) {
        resumeEffect(std::string(key));
    }
    
    /**
     * Stops all sound effects, removing them from the engine.
     *
     * You will need to add the effects again if you wish to replay them.
     */
    void stopAllEffects();
    
    /**
     * Pauses all sound effects, allowing them to be resumed later.
     *
     * Sound effects already paused will remain paused.
     */
    void pauseAllEffects();
    
    /**
     * Resumes all paused sound effects.
     */
    void resumeAllEffects();
    
    /**
     * Sets the callback for sound effects
     *
     * This cllabck function is called whenever a sound effect completes. It is
     * called whether or not the sound completed normally or if it was
     * terminated manually.  However, the second parameter can be used to 
     * distinguish the two cases.
     *
     * @param callback  The callback for sound effects
     */
    void setEffectListener(std::function<void(const std::string& key,bool)> callback) {
        _soundCB = callback;
    }
    
    /**
     * Returns the callback for sound effects
     *
     * This cllabck function is called whenever a sound effect completes. It is
     * called whether or not the sound completed normally or if it was
     * terminated manually.  However, the second parameter can be used to
     * distinguish the two cases.
     *
     * @return the callback for sound effects
     */
    std::function<void(const std::string& key,bool)> getEffectListener() const {
        return _soundCB;
    }

    /**
     * Callback function for when a sound effect channel finishes
     *
     * This method is called when the active sound effect completes. It garbage
     * collects the sound effect, allowing its key to be reused.
     *
     * This method is never intended to be accessed by general users.  It is
     * only publicly visible because this makes our cross-platform code cleaner.
     *
     * @param id        The sound channel being garbage collected
     * @param status    True if the music terminated normally, false otherwise.
     */
    void gcEffect(int id, bool status);
    
    
#pragma mark -
#pragma mark Global Management
    /**
     * Stops all sounds, both music and sound effects.
     *
     * This effectively clears the sound engine.
     */
    void stopAll();
    
    /**
     * Pauses all sounds, both music and sound effects.
     *
     * This method allows them to be resumed later. You should generally
     * call this method just before the app pages to the background.
     */
    void pauseAll();
    
    /**
     * Resumes all paused sounds, both music and sound effects.
     *
     * You should generally call this method right after the app returns
     * from the background.
     */
    void resumeAll();
};

}


#endif /* __CU_AUDIO_ENGINE_H__ */
