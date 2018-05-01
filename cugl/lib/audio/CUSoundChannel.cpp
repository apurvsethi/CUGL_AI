//
//  CUSoundChannel.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for playing PCM buffers (e.g. WAV files).
//  This class provides a simple channel abstraction that hides the platform
//  specific details.  Channels are not meant to be accessed directly by the
//  user.  All interactions should go through the AudioEngine.
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
#include <cugl/util/CUDebug.h>
#include <cugl/base/CUApplication.h>
#include "CUSoundChannel.h"
#include "platform/CUAudioEngine-impl.h"

/** The timeout period for aging out sounds */
#define CLIPPING_TIMEOUT 100

/** Some Android workarounds */
#define ANDROID_FADEVOL  8/128.0f
#define ANDROID_TIMEOUT  10

using namespace cugl;

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
SoundChannel::SoundChannel() :
_channel(-1),
_player(nullptr),
_playing(false),
_paused(false),
_primaryLoop(false),
_primaryVolume(0.0f),
_primaryTime(0),
_shadowLoop(false),
_shadowVolume(0.0f),
_shadowTime(0),
_selfdelete(false) {
}

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
bool SoundChannel::init(int id) {
    
    _player = impl::AudioAllocChannel(id);
    if (_player) {
        _channel = id;
        _playing = false;
        _paused  = false;
        return true;
    }
    
    return false;
}

/**
 * Removes this player from the audio engine.
 *
 * This method differs from the destructor in that the player can be
 * reattached with a subsequent call to init().
 */
void SoundChannel::dispose() {
    if (_player) {
        _selfdelete = true;
        if (impl::AudioChannelPlaying(_player)) {
            impl::AudioHaltChannel(_player);
        }
        impl::AudioFreeChannel(_player);
        _player = nullptr;
    }
    _channel = -1;
    _primary = nullptr;
    _shadow  = nullptr;
    _playing = false;
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
void SoundChannel::attach(const std::string& key, const std::shared_ptr<Sound>& asset, float volume, bool loop) {
    CUAssertLog(_primary == nullptr || _shadow == nullptr, "Attaching to an occupied audio channel");
    
    if (_primary == nullptr) {
        _playing = false;
        _paused  = false;
        _primary = asset;
        _primaryKey = key;
        _primaryLoop = loop;
        _primaryVolume = volume;
        _primaryTime = 0;
    } else {
        _shadow = asset;
        _shadowKey = key;
        _shadowLoop = loop;
        _shadowVolume = volume;
        _shadowTime = 0;
    }
    
}

/**
 * Swaps in the shadow asset, provided that there is one.
 *
 * The shadow asset begins playing immediately.  If there is no shadow
 * asset, this method does nothing.
 */
void SoundChannel::advance() {
    CUAssertLog(_primary != nullptr, "Advancing an empty audio channel");
    
    // Swap in shadow buffer if appropriate
    if (_shadow != nullptr) {
        _playing = false;
        _paused  = false;
        _primary = _shadow;
        _primaryKey = _shadowKey;
        _primaryLoop   = _shadowLoop;
        _primaryVolume = _shadowVolume;
        _primaryTime   = _shadowTime;
        
        _shadow = nullptr;
        _shadowKey = "";
        _shadowLoop = false;
        _shadowVolume = 0.0;
        _shadowTime = 0;
        
        play();
    }
    
}

/**
 * Clears both the primary and the shadow asset.
 *
 * If the channel is still playing, it will perform a hard stop immediately.
 */
void SoundChannel::clear() {
    if (impl::AudioChannelPlaying(_player)) {
        _selfdelete = true;
        impl::AudioHaltChannel(_player);
    }
    
    _playing = false;
    _paused  = false;
    _primary = nullptr;
    _primaryKey = "";
    _primaryLoop = false;
    _primaryVolume = 0.0;
    _primaryTime = 0;
    
    _shadow = nullptr;
    _shadowKey = "";
    _shadowLoop = false;
    _shadowVolume = 0.0;
    _shadowTime = 0;
}



#pragma mark -
#pragma mark Playback Control
/**
 * Plays the current asset immediately.
 */
void SoundChannel::play() {
    CUAssertLog(_primary != nullptr, "Attempt to play without an attached asset");
    
    // We may still be playing with volume down (CLIPPING WORKAROUND)
    if (impl::AudioChannelPlaying(_player)) {
        _selfdelete = true;
        impl::AudioHaltChannel(_player);
    }
    
    _playing = true;
    impl::AudioSetChannelVolume(_player,_primaryVolume);
    impl::AudioPlayChannel(_player,_primary->_buffer,_primaryLoop,(Uint32)_primaryTime);
}

/**
 * Pauses the current asset.
 *
 * The asset is not marked for deletion and will pick up from where it
 * stopped when the sound is resumed.  If the sound is already paused, this
 * method will fail.
 *
 * @return true if the sound is successfully paused
 */
bool SoundChannel::pause() {
    CUAssertLog(_primary != nullptr, "Attempt to pause with no primary asset");
    CUAssertLog(_playing, "Channel is not currently playing");
    
    if (_paused) {
        return false;
    }
    
    // Capture time at the pause.
    _paused = true;
    _pauseTime = impl::AudioGetChannelFrame(_player);
    _primaryTime = _pauseTime;
    impl::AudioPauseChannel(_player);
    return true;
}

/**
 * Resumes the current asset.
 *
 * If the sound was previously paused, this pick up from where it stopped.
 * If the sound is not paused, this method will fail.
 *
 * @return true if the sound is successfully resumed
 */
bool SoundChannel::resume() {
    CUAssertLog(_primary != nullptr, "Attempt to resume with no primary asset");
    CUAssertLog(_playing, "Channel is not currently playing");
    
    if (!_paused) {
        return false;
    }
    
    // Reschedule if time changed while paused.
    if (_primaryTime != _pauseTime) {
        impl::AudioSetChannelFrame(_player,_primaryTime);
    }
    
    _paused = false;
    impl::AudioResumeChannel(_player);
    return true;
}

/**
 * Stops the current asset, marking it for deletion.
 *
 * The player will not be available for a new sound immediately.  The asset
 * will not detach until the next animation frame.  If the player is needed
 * immediately, the sound should be attached as a shadow asset.
 */
void SoundChannel::stop() {
    CUAssertLog(_primary != nullptr, "Attempt to stop with no primary asset");
    CUAssertLog(_playing, "Channel is not currently playing");
    _playing = false;
    _selfdelete = true;
    // Workaround because Android sucks
#if defined (__ANDROID__)
    impl::AudioSetChannelVolume(_player,ANDROID_FADEVOL);
    impl::AudioExpireChannel(_player, ANDROID_TIMEOUT);
#else
    impl::AudioSetChannelVolume(_player,0);
    impl::AudioExpireChannel(_player, CLIPPING_TIMEOUT);
#endif
}

#pragma mark -
#pragma mark Playback Attributes
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
float SoundChannel::getDuration() const {
    CUAssertLog(_primary != nullptr, "Attempt to query time with no primary asset");
    if (_shadow != nullptr) {
        return (float)_shadow->getDuration();
    }
    return (float)_primary->getDuration();
}

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
float SoundChannel::getCurrentTime() const {
    CUAssertLog(_primary != nullptr, "Attempt to query time with no primary asset");
    if (_shadow != nullptr) {
        return (float)(_shadowTime/_shadow->getSampleRate());
    }
    
    int offset = 0;
    if (_paused) {
        offset = (int)_pauseTime;
    } else if (!_playing) {
        offset = (int)_primaryTime;
    } else {
        offset = (int)impl::AudioGetChannelFrame(_player);
    }
    return (float)(offset/_primary->getSampleRate());
}

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
void SoundChannel::setCurrentTime(float time, bool force) {
    CUAssertLog(_primary != nullptr, "Attempt to set time with no primary asset");
    if (_shadow != nullptr) {
        _shadowTime = (Uint64)(time*_shadow->getSampleRate());
        if (force) {
            advance();
        }
    } else {
        if (_paused) {
            _primaryTime = (Uint64)(time*_primary->getSampleRate());
            if (force) {
                resume();
            }
        } else if (!_playing) {
            _primaryTime = (Uint64)(time*_primary->getSampleRate());
        } else {
            impl::AudioSetChannelFrame(_player, (Uint64)(time*_primary->getSampleRate()));
        }
    }
}

/**
 * Sets the volume (0 to 1) of the asset being played.
 *
 * If there is a shadow asset present, this method will apply to the shadow
 * asset instead.
 *
 * @param  volume   the volume (0 to 1) to play the asset.
 */
void SoundChannel::setVolume(float volume) {
    CUAssertLog(_primary != nullptr, "Attempt to volume with no primary asset");
    CUAssertLog(volume >=0 && volume <= 1, "The volume %.3f is out of range",volume);
    _primaryVolume = volume;
    impl::AudioSetChannelVolume(_player,volume);
}

/**
 * Returns the volume (0 to 1) of the asset being played.
 *
 * If there is a shadow asset present, this method will apply to the shadow
 * asset instead.
 *
 * @return the volume (0 to 1) of the asset being played.
 */
void SoundChannel::setLoop(bool loop) {
    CUAssertLog(_primary != nullptr, "Attempt to loop with no primary asset");
    _primaryLoop = loop;
    impl::AudioSetChannelLoop(_player,loop);
}
