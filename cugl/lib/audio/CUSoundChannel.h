//
//  CUSoundChannel.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for playing PCM buffers (e.g. WAV files).
//  This class provides a simple channel abstraction that hides the platform
//  specific details.  Channels are not meant to be accessed directly by the
//  user.  All interactions should go through the AudioEngine.
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
#ifndef __CU_SOUND_CHANNEL_H__
#define __CU_SOUND_CHANNEL_H__
#include <cugl/audio/CUSound.h>

namespace cugl {
   
// We use the impl namespace for platform-dependent data.
namespace impl {
    /**
     * Reference to an platform-specific audio player for PCM Channels.
     *
     * We present this as a struct, as it has more type information than a void
     * pointer, and is equally safe across C++ and Objective-C boundaries.  The
     * exact representation of the struct is platform specific.
     */
    struct AudioChannel;
}

    
#pragma mark -
#pragma mark Sound Channel Class
/**
 * Player for a single sound effect channel.
 *
 * A sound channel can only play one sound at a time.  The audio engine is hence
 * a collection of multiple channels. The engine attaches a sound asset to a
 * channel, and that channel is responsible for playing that sound until done,
 * or it is stopped manually. The sound asset is then detached, and the player
 * is available for another sound.
 *
 * In both SDL and AVFoundation, there appears to be an issue where a hard stop
 * can produce a clipping sound.  StackOverflow is littered with this issue and
 * no good solutions.  This class provides a workable, but imperfect solution:
 * sounds only stop at the end of the buffer, unless completely necessary.
 * That is, if the sound stops on its own, there is no problem. However, if the
 * sound is stopped manually, then the player simply turns the volume to 0 and
 * allows it to play to completion while marking the buffer for deletion in a
 * subsequent animation frame.
 *
 * This solution has an unfortunate side effect: the engine may have no
 * available players even though all sounds are "stopped".  This is the purpose
 * of the shadow asset.  If the original asset is marked for deletion, then
 * the engine can attach a second sound asset to the player.  The original
 * asset will be detached the next animation frame and the shadow asset will
 * immediately start playing.  This allows the engine to fool the user that a
 * player is immediately available, even when it is not.
 *
 * IMPORTANT: For best performance, it is absolutely crucial that all sounds
 * have exactly the same format. The same file format, the same sampling rate,
 * the same number of channels.  Any change in format requires a reconfiguration
 * of the audio engine, and this can cause distortion for rapid-fire sound
 * effects.
 */
class SoundChannel {
private:
    /** The unique identifier for this channel */
    Sint32 _channel;
    /** A reference to the (platform-specific) player for this channel */
    impl::AudioChannel* _player;
    
    /** Whether the player is actively playing */
    bool _playing;
    /** Whether the player is paused (but may still be active) */
    bool _paused;
    
    /** The primary asset currently attached to this player for use */
    std::shared_ptr<Sound> _primary;
    /** The key associated with the primary asset */
    std::string _primaryKey;
    /** Whether to loop the primary asset */
    bool  _primaryLoop;
    /** The volume for the primary asset */
    float _primaryVolume;
    /** The sample frame (in the audio file) to resume the sound after a pause */
    Uint64 _primaryTime;
    /** The sample frame (in the audio file) at which the sound was paused */
    Uint64 _pauseTime;
    
    /** A queued asset to play immediately once the current one is detached */
    std::shared_ptr<Sound> _shadow;
    /** The key associated with the shadow asset */
    std::string _shadowKey;
    /** Whether to loop the shadow asset */
    bool  _shadowLoop;
    /** The volume for the shadow asset */
    float _shadowVolume;
    /** The sample frame (in the audio file) to resume the sound after a pause */
    Uint64 _shadowTime;
    
    /** Mark whether or not we deleted ourself (to prevent double cleanup) */
    bool _selfdelete;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new SoundChannel
     *
     * This method simply initializes the default values of the attributes.
     * It does not allocate any buffers for processing audio.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
     */
    SoundChannel();
    
    /**
     * Disposes of this player, detaching it from the audio engine.
     */
    ~SoundChannel() { dispose(); }
    
    /**
     * Removes this player from the audio engine.
     *
     * This method differs from the destructor in that the player can be
     * reattached with a subsequent call to init().
     */
    void dispose();
    
    /**
     * Initializes this player as a channel in the audio engine.
     *
     * This method allocates the necessary buffers for sound processing.
     * However, it does not attach this channel to the audio engine. That
     * should be done externally.
     *
     * @param  id   the channel identifier
     *
     * @return true if the channel was initialized successfully
     */
    bool init(int id);
    
    
#pragma mark -
#pragma mark Static Constructor
    /**
     * Returns a newly allocated player for a channel in the audio engine.
     *
     * This method allocates the necessary buffers for sound processing.
     * However, it does not attach this channel to the audio engine.  That
     * should be done externally.
     *
     * @param  id   the channel identifier
     *
     * @return a newly allocated player for a channel in the audio engine.
     */
    static std::shared_ptr<SoundChannel> alloc(int id) {
        std::shared_ptr<SoundChannel> result = std::make_shared<SoundChannel>();
        return (result->init(id) ? result : nullptr);
    }
    
    
#pragma mark -
#pragma mark Asset Management
    /**
     * Attaches an asset and readies it with the given volume and loop setting.
     *
     * A channel may have at most two assets attached: the primary and the
     * shadow asset.  The primary asset may be played immediately.  The shadow
     * asset cannot be played until the {@link advance} method is called.
     *
     * If this channel has no attached assets, then the attached asset will
     * become the primary one.  If it already has an asset, it will become the
     * shadow asset.  Otherwise, it will raise an error.
     *
     * The channels keeps track of the key for asset management.  This is
     * used by the {@link AudioEngine} for garbage collection.
     *
     * @param  key      the key for this playback instance
     * @param  asset    the sound asset to play
     * @param  volume   the volume ot play the sound
     * @param  loop     whether to loop the sound indefinitely
     */
    void attach(const std::string& key, const std::shared_ptr<Sound>& asset, float volume=1.0, bool loop=false);
    
    /**
     * Attaches an asset and readies it with the given volume and loop setting.
     *
     * A channel may have at most two assets attached: the primary and the
     * shadow asset.  The primary asset may be played immediately.  The shadow
     * asset cannot be played until the {@link advance} method is called.
     *
     * If this channel has no attached assets, then the attached asset will
     * become the primary one.  If it already has an asset, it will become the
     * shadow asset.  Otherwise, it will raise an error.
     *
     * The channels keeps track of the key for asset management.  This is
     * used by the {@link AudioEngine} for garbage collection.
     *
     * @param  key      the key for this playback instance
     * @param  asset    the sound asset to play
     * @param  volume   the volume ot play the sound
     * @param  loop     whether to loop the sound indefinitely
     */
    void attach(const char* key, const std::shared_ptr<Sound>& asset, float volume=1.0, bool loop=false) {
        attach(std::string(key),asset,volume,loop);
    }
    
    /**
     * Swaps in the shadow asset, provided that there is one.
     *
     * The shadow asset begins playing immediately.  If there is no shadow
     * asset, this method does nothing.
     */
    void advance();
    
    /**
     * Clears both the primary and the shadow asset.
     *
     * If the channel is still playing, it will perform a hard stop immediately.
     */
    void clear();
    
    /**
     * Returns the number (0, 1, or 2) of assets attached to this channel
     *
     * A channel with both a primary and shadow asset will return a value of 2.
     * Any attempt to add more assets to such a channel will result in an error.
     *
     * @return the number (0, 1, or 2) of assets attached to this channel
     */
    unsigned int attached() const {
        return _primary != nullptr ? (_shadow != nullptr ? 2 : 1) : 0;
    }
    
    /**
     * Returns the key identifier for the primary asset
     *
     * @return the key identifier for the primary asset
     */
    const std::string& getPrimaryKey() const { return _primaryKey; }
    
    /**
     * Returns a reference to the primary asset
     *
     * @return a reference to the primary asset
     */
    const std::shared_ptr<Sound> getPrimary() const { return _primary; }
    
    /**
     * Returns a reference to the primary asset
     *
     * @return a reference to the primary asset
     */
    std::shared_ptr<Sound> getPrimary() { return _primary; }
    
    /**
     * Returns the key identifier for the shadow asset
     *
     * @return the key identifier for the shadow asset
     */
    const std::string& getShadowKey() const { return _shadowKey; }
    
    /**
     * Returns a reference to the shadow asset
     *
     * @return a reference to the shadow asset
     */
    const std::shared_ptr<Sound> getShadow() const { return _shadow; }
    
    /**
     * Returns a reference to the shadow asset
     *
     * @return a reference to the shadow asset
     */
    std::shared_ptr<Sound> getShadow() { return _shadow; }
    
    
#pragma mark -
#pragma mark Memory Management
    /**
     * Resets the deletion status after a self-deletion.
     *
     * This method is called by {@link AudioEngine} in the garbage collection
     * method for clean-up.
     */
    void resetDelete()      { _selfdelete = false; }
    
    /**
     * Returns true if this player was deleted recently.
     *
     * This method is called by {@link AudioEngine} in the garbage collection
     * method to check whether clean-up is necessary.
     */
    bool isDeleted() const  { return _selfdelete;  }
    
    
#pragma mark -
#pragma mark Playback Control
    /**
     * Plays the current asset immediately.
     */
    void play();
    
    /**
     * Pauses the current asset.
     *
     * The asset is not marked for deletion and will pick up from where it
     * stopped when the sound is resumed.  If the sound is already paused, this
     * method will fail.
     *
     * @return true if the sound is successfully paused
     */
    bool pause();
    
    /**
     * Resumes the current asset.
     *
     * If the sound was previously paused, this pick up from where it stopped.
     * If the sound is not paused, this method will fail.
     *
     * @return true if the sound is successfully resumed
     */
    bool resume();
    
    /**
     * Stops the current asset, marking it for deletion.
     *
     * The player will not be available for a new sound immediately.  The asset
     * will not detach until the next animation frame.  If the player is needed
     * immediately, the sound should be attached as a shadow asset.
     */
    void stop();
    
    
#pragma mark -
#pragma mark Playback Attributes
    /**
     * Returns the identifier for this channel
     *
     * @return the identifier for this channel
     */
    Sint32  getId() const { return _channel; }
    
    /**
     * Returns true if this player is currently paused
     *
     * @return true if this player is currently paused
     */
    bool  isPaused() const { return _playing && _paused; }
    
    /**
     * Returns true if this player has been stopped
     *
     * @return true if this player has been stopped
     */
    bool  isStopped() const { return !_playing; }
    
    /**
     * Returns the length of the asset being played, in seconds.
     *
     * Because the asset is fully decompressed at load time, the result of
     * this method is reasonably accurate.
     *
     * If there is a shadow asset present, this method will apply to the shadow
     * asset instead.
     *
     * @return the length of the asset being played, in seconds.
     */
    float getDuration() const;
    
    /**
     * Returns the current position of the asset being played, in seconds.
     *
     * Because the asset is fully decompressed at load time, the result of
     * this method is reasonably accurate.
     *
     * If there is a shadow asset present, this method will apply to the shadow
     * asset instead.
     *
     * @return the current position of the asset being played, in seconds.
     */
    float getCurrentTime() const;
    
    /**
     * Sets the current posiiton of the asset being played, in seconds.
     *
     * Because the asset is fully decompressed at load time, the result of
     * this method is reasonably accurate. If the sound is paused, this will 
     * do nothing until the player is resumed. Otherwise, this will stop and 
     * restart the sound at the new position.
     *
     * If there is a shadow asset present, this method will apply to the shadow
     * asset instead.
     *
     * @param  time     the new position of the player in the audio source
     * @param  force    whether to force the player to play, even if paused
     */
    void setCurrentTime(float time, bool force=false);
    
    /**
     * Returns the volume (0 to 1) of the asset being played.
     *
     * If there is a shadow asset present, this method will apply to the shadow
     * asset instead.
     *
     * @return the volume (0 to 1) of the asset being played.
     */
    float getVolume() const { return (_shadow == nullptr ? _primaryVolume : _shadowVolume); }
    
    /**
     * Sets the volume (0 to 1) of the asset being played.
     *
     * If there is a shadow asset present, this method will apply to the shadow
     * asset instead.
     *
     * @param  volume   the volume (0 to 1) to play the asset.
     */
    void setVolume(float volume);
    
    /**
     * Returns true if the current sound is in an indefinite loop.
     *
     * If there is a shadow asset present, this method will apply to the shadow
     * asset instead.
     *
     * @return true if the current sound is in an indefinite loop.
     */
    bool  getLoop() const   { return (_shadow == nullptr ? _primaryLoop : _shadowLoop); }
    
    /**
     * Sets whether the current sound should play in an indefinite loop.
     *
     * If loop is false, then the sound will stop at its natural loop point.
     *
     * If there is a shadow asset present, this method will apply to the shadow
     * asset instead.
     *
     * @param  loop whether the current sound should play in an indefinite loop
     */
    void setLoop(bool loop);
    
    /** Allow the AudioEngine access to the player */
    friend class AudioEngine;
};

}
#endif /* __CU_SOUND_CHANNEL_H__ */
