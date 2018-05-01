//
//  CUMusicQueue.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for playing streaming music from MP3, OGG, WAV
//  or other files.  This class provides a simple channel abstraction that
//  hides the platform specific details.  Background music is not meant to be
//  accessed directly by the user.  All interactions should go through the
//  AudioEngine.
//
//  This file is an internal header.  It is not accessible by general users
//  of the CUGL API.
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
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
#ifndef __CU_MUSIC_QUEUE_H__
#define __CU_MUSIC_QUEUE_H__
#include <cugl/audio/CUMusic.h>
#include <deque>
#include <vector>

namespace cugl {
    
// We use the impl namespace for platform-dependent data.
namespace impl {
    /**
     * Reference to an platform-specific music player.
     *
     * We present this as a struct, as it has more type information than a void
     * pointer, and is equally safe across C++ and Objective-C boundaries.  The
     * exact representation of the struct is platform specific.
     */
    struct AudioPlayer;
}

/**
 * Struct to store playback information in the waiting queue.
 *
 * This data is independent of the asset.  In order to cut down on the number
 * of queues to track this data, we package it as a single struct.
 */
typedef struct {
    /** The volume to play the music */
    float volume;
    /** The number of seconds to fade IN the music */
    float fade;
    /** Whether to loop the music */
    bool  loop;
} MusicSettings;

    
#pragma mark -
#pragma mark Music Queue
/**
 * Sequential playback queue for streamin audio
 *
 * A music queue is similar to a sound channel in that it can only play one 
 * asset at a time. The difference is that music queues can support streaming
 * data while sound channels need in-memory PCM (WAV) data.
 *
 * In general, there is usually only one music queue at a time, representing 
 * the application background music.  The queue allows the user to queue up
 * additional tracks ahead of time so that transition from one song to another
 * is seamless.
 *
 * IMPORTANT: For best performance, it is absolutely crucial that all music
 * have exactly the same format. The same file format, the same sampling rate,
 * the same number of channels.  Any change in format requires a reconfiguration
 * of the audio engine, and this can cause gaps between songs.
 * effects.
 */
class MusicQueue {
private:
    /** A reference to the (platform-specific) music player */
    impl::AudioPlayer* _player;
    /** The music asset for the active music channel */
    std::shared_ptr<Music> _music;
    /** A copy of the music for garbage collection */
    std::shared_ptr<Music> _backgd;
    /** The playback settings for the active asset */
    MusicSettings _settings;

    /** Whether the player is actively playing */
    bool  _playing;
    /** Whether the player is paused (but may still be active) */
    bool  _paused;

    /** The queue for subsequent music loops */
    std::deque<std::shared_ptr<Music>> _mqueue;
    /** The playback settings for the subsequent music loops */
    std::deque<MusicSettings> _squeue;

    
#pragma mark -
#pragma mark Constructor
public:
    /**
     * Creates a new MusicQueue
     *
     * This method simply initializes the default values of the attributes.
     * It does not allocate any buffers for processing audio.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
     */
    MusicQueue() : _player(nullptr) {}
    
    /**
     * Disposes of this playback queue, detaching it from the audio engine.
     */
    ~MusicQueue() { dispose(); }
    
    /**
     * Removes this playback queue from the audio engine.
     *
     * This method differs from the destructor in that the player can be
     * reattached with a subsequent call to init().
     */
    void dispose();
    
    /**
     * Initializes this playback queue for the audio engine.
     *
     * This method allocates the necessary buffers for stream processing.
     * However, it does not attach this queue to the audio engine. That
     * should be done externally.
     *
     * @return true if the queue was initialized successfully
     */
    bool init();
    
    
#pragma mark -
#pragma mark Static Constructor
    /**
     * Returns a newly allocated playback queue for the audio engine.
     *
     * This method allocates the necessary buffers for stream processing.
     * However, it does not attach this queue to the audio engine. That
     * should be done externally.
     *
     * @return a newly allocated playback queue for the audio engine.
     */
    static std::shared_ptr<MusicQueue> alloc() {
        std::shared_ptr<MusicQueue> result = std::make_shared<MusicQueue>();
        return (result->init() ? result : nullptr);
    }
    
    
#pragma mark -
#pragma mark Asset Management
    /**
     * Adds the given music asset to the music queue
     *
     * Music is handled differently from sound effects.  Music is streamed
     * instead of being pre-loaded, so your music files can be much larger.
     * The drawback is that you can only play one music asset at a time.
     * However, it is possible to queue music assets for immediate playback
     * once the active asset is finished.
     *
     * If the queue is empty and there is no active music, this method will
     * play the music immediately.  Otherwise, it will add the music to the
     * queue, and it will play as soon as it is removed from the queue.
     * When it begins playing, it will start at full volume unless you
     * provide a number of seconds to fade in.
     *
     * Note that looping a song will cause it to block the queue indefinitely
     * until you turn off looping for that asset {@see setLoop}. This can be
     * desired behavior, as it gives you a way to control the speed of the
     * queue processing.
     *
     * @param music     The music asset to play
     * @param volume    The music volume
     * @param loop      Whether to loop the music continuously
     * @param fade      The number of seconds to fade in
     */
    void enqueue(const std::shared_ptr<Music>& music, float volume=1.0f, bool loop=false, float fade=0.0f);
    
    /**
     * Returns the size of the music queue
     *
     * @return the size of the music queue
     */
    size_t size() const { return _mqueue.size(); }

    /**
     * Returns the current playing asset for this music queue
     *
     * @return the current playing asset for this music queue
     */
    const std::shared_ptr<Music> getCurrent() const { return _backgd; }
    
    /**
     * Returns the current playing asset for this music queue
     *
     * @return the current playing asset for this music queue
     */
    std::shared_ptr<Music> getCurrent() { return _backgd; }
    
    /**
     * Returns the (weak) list of assets for the music queue
     *
     * @return the (weak) list of assets for the music queue
     */
    const std::vector<const Music*> getQueue() const;
    
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
    void advance(unsigned int steps=0);
    
    /**
     * Clears the music queue, but does not release any other resources.
     *
     * This method does not stop the current music asset from playing. It 
     * only clears pending music assets from the queue.
     */
    void clear();
    
    
#pragma mark -
#pragma mark Playback Control
    /**
     * Plays the first element in the queue immediately.
     */
    void play();
    
    /**
     * Pauses the current music asset.
     *
     * The asset is not marked for deletion and will pick up from where it
     * stopped when the music is resumed.  If the music is already paused, this
     * method will fail.
     *
     * @return true if the music is successfully paused
     */
    bool pause();
    
    /**
     * Resumes the current music asset.
     *
     * If the music was previously paused, this pick up from where it stopped.
     * If the music is not paused, this method will fail.
     *
     * @return true if the music is successfully resumed
     */
    bool resume();
    
    /**
     * Stops the current music asset and clears the entire queue.
     *
     * Before the music is stopped, this method gives the user an option to
     * fade out the music.  If the argument is 0, it will halt the music
     * immediately. Otherwise it will fade to completion over the given number
     * of seconds.
     *
     * @param fade  The number of seconds to fade out
     */
    void stop(float fade=0.0f);
    
    
#pragma mark -
#pragma mark Playback Attributes
    /**
     * Returns true if this queue is currently paused
     *
     * @return true if this queue is currently paused
     */
    bool  isPaused() const { return _paused && _playing; }
    
    /**
     * Returns true if this queue has been stopped
     *
     * @return true if this queue has been stopped
     */
    bool  isStopped() const { return !_playing; }
    
    /**
     * Returns the length of the asset being played, in seconds.
     *
     * This only returns the length of the asset at the head of the queue.
     * All other music assets in the queue are ignored.
     *
     * This information is retrieved from the decoder. As the file is completely
     * decoded at load time, the result of this method is reasonably accurate.
     *
     * @return the length of the asset being played, in seconds.
     */
    float getDuration() const;
    
    /**
     * Returns the current position of the asset being played, in seconds.
     *
     * This only returns the position for the asset at the head of the queue.
     * All other music assets in the queue are ignored.
     *
     * This information is not guaranteed to be accurate.  Attempting to time
     * the playback of streaming data (as opposed to a fully in-memory PCM
     * buffer) is very difficult and not cross-platform.  We have tried to be
     * reasonably accurate, but from our tests we can only guarantee accuracy
     * within a 10th of a second.
     *
     * @return the current position of the asset being played, in seconds.
     */
    float getCurrentTime() const;
    
    /**
     * Sets the current posiiton of the asset being played, in seconds.
     *
     * If the sound is paused, this will do nothing until the player is resumed.
     * Otherwise, this will stop and restart the sound at the new position.
     *
     * This only assigns the position for the asset at the head of the queue.
     * All other music assets in the queue are ignored.
     *
     * This information is not guaranteed to be accurate.  Attempting to time
     * the playback of streaming data (as opposed to a fully in-memory PCM
     * buffer) is very difficult and not cross-platform.  We have tried to be
     * reasonably accurate, but from our tests we can only guarantee accuracy
     * within a 10th of a second.
     *
     * @param  time     the new position of the player in the audio source
     * @param  force    whether to force the player to play, even if paused
     */
    void setCurrentTime(float time, bool force=false);
    
    /**
     * Returns the volume (0 to 1) of the asset being played.
     *
     * This only returns the volume for the asset at the head of the queue.
     * All other music assets in the queue are ignored.
     *
     * @return the volume (0 to 1) of the asset being played.
     */
    float getVolume() const { return (_music == nullptr ? 0 : _settings.volume); }
    
    /**
     * Sets the volume (0 to 1) of the asset being played.
     *
     * This only sets the volume for the asset at the head of the queue.
     * All other music assets in the queue are ignored.
     *
     * @param  volume   the volume (0 to 1) to play the asset.
     */
    void setVolume(float volume);
    
    /**
     * Returns true if the current sound is in an indefinite loop.
     *
     * If the value is false, the music will stop at its natural loop point.
     *
     * This only returns the status for the asset at the head of the queue.
     * All other music assets in the queue are ignored.
     *
     * @return true if the current sound is in an indefinite loop.
     */
    bool  getLoop() const   { return (_music == nullptr ? false : _settings.loop); }
    
    /**
     * Sets whether the current sound should play in an indefinite loop.
     *
     * If loop is false, the music will stop at its natural loop point.
     *
     * This only assigns the status for the asset at the head of the queue.
     * All other music assets in the queue are ignored.  Of course, this means
     * that the queue may become blocked if the current asset is set to loop
     * indefinitely. This can be desired behavior, as it gives you a way to 
     * control the speed of the queue processing.
     *
     * @param  loop whether the current sound should play in an indefinite loop
     */
    void setLoop(bool loop);
    
};

}

#endif /* __CU_MUSIC_QUEUE_H__ */
