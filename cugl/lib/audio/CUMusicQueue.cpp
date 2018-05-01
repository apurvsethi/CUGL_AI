//
//  CUMusicQueue.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for playing streaming music from MP3, OGG, WAV
//  or other files.  This class provides a simple channel abstraction that
//  hides the platform specific details.  Background music is not meant to be
//  accessed directly by the user.  All interactions should go through the
//  AudioEngine.
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
#include "CUMusicQueue.h"
#include "platform/CUAudioEngine-impl.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructor
/**
 * Initializes this playback queue for the audio engine.
 *
 * This method allocates the necessary buffers for stream processing.
 * However, it does not attach this queue to the audio engine. That
 * should be done externally.
 *
 * @return true if the queue was initialized successfully
 */
bool MusicQueue::init() {
    _player  = impl::AudioAllocBackground();
    _paused  = false;
    _playing = false;
    
    return _player;
}

/**
 * Removes this playback queue from the audio engine.
 *
 * This method differs from the destructor in that the player can be
 * reattached with a subsequent call to init().
 */
void MusicQueue::dispose() {
    if (_player) {
        if (impl::AudioBackgroundPlaying(_player)) {
            impl::AudioHaltBackground(_player);
        }
        impl::AudioFreeBackground(_player);
        _player = nullptr;
    }
    _music = nullptr;
    _mqueue.clear();
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
void MusicQueue::enqueue(const std::shared_ptr<Music>& music, float volume, bool loop, float fade) {
    if (_music == nullptr) {
        _music = music;
        _backgd = music;
        _settings.volume = volume;
        _settings.fade = fade;
        _settings.loop = loop;
        play();
        return;
    }
    
    MusicSettings set;
    set.volume = volume;
    set.fade = fade;
    set.loop = loop;
    _mqueue.push_back(music);
    _squeue.push_back(set);
}

/**
 * Returns the list of assets for the music queue
 *
 * @return the list of assets for the music queue
 */
const std::vector<const Music*> MusicQueue::getQueue() const {
    std::vector<const Music*> result;
    for(auto it = _mqueue.begin(); it != _mqueue.begin(); ++it) {
        Music* packet = it->get();
        result.push_back(packet);
    }
    return result;
}

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
void MusicQueue::advance(unsigned int steps) {
    int pos = steps;
    while (pos >= 0 && !_mqueue.empty()) {
        _music = _mqueue.front();
        _backgd = _music;
        _settings = _squeue.front();
        _mqueue.pop_front();
        _squeue.pop_front();
        pos--;
    }
    if (pos >= 0) {
        _music = nullptr;
        if (impl::AudioBackgroundPlaying(_player)) {
            impl::AudioHaltBackground(_player);
        }
    } else {
        play();
    }
}

/**
 * Clears the music queue, but does not release any other resources.
 *
 * This method does not stop the current music asset from playing. It
 * only clears pending music assets from the queue.
 */
void MusicQueue::clear() {
    _mqueue.clear();
    _squeue.clear();
}


#pragma mark -
#pragma mark Playback Control
/**
 * Plays the first element in the queue immediately.
 */
void MusicQueue::play() {
    if (_music == nullptr) {
        return;
    }
    
    impl::AudioSetBackgroundVolume(_player, _settings.volume);
    if (_settings.fade > 0) {
        Uint32 millis = (Uint32)(_settings.fade*1000);
        impl::AudioFadeInBackground(_player, _music->_buffer, _settings.loop, 0, millis);
    } else {
        impl::AudioPlayBackground(_player, _music->_buffer, _settings.loop);
    }
    
    _playing = true;
}

/**
 * Pauses the current music asset.
 *
 * The asset is not marked for deletion and will pick up from where it
 * stopped when the music is resumed.  If the music is already paused, this
 * method will fail.
 *
 * @return true if the music is successfully paused
 */
bool MusicQueue::pause() {
    CUAssertLog(_music != nullptr, "Attempt to pause with no music asset");
    CUAssertLog(_playing, "No music is currently playing");
    
    if (_paused) {
        return false;
    }
    
    // Capture time at the pause.
    _paused = true;
    impl::AudioPauseBackground(_player);
    return true;
}

/**
 * Resumes the current music asset.
 *
 * If the music was previously paused, this pick up from where it stopped.
 * If the music is not paused, this method will fail.
 *
 * @return true if the music is successfully resumed
 */
bool MusicQueue::resume() {
    CUAssertLog(_music != nullptr, "Attempt to resume with no _music asset");
    CUAssertLog(_playing, "No music is currently playing");
    
    if (!_paused) {
        return false;
    }
    
    _paused = false;
    impl::AudioResumeBackground(_player);
    return true;
}

/**
 * Stops the current music asset and clears the entire queue.
 *
 * Before the music is stopped, this method givens the user an option to
 * fade out the music.  If the argument is 0, it will halt the music
 * immediately. Otherwise it will fade to completion over the given number
 * of seconds.
 *
 * @param fade  The number of seconds to fade out
 */
void MusicQueue::stop(float fade) {
    if (fade > 0) {
        impl::AudioFadeOutBackground(_player, (Uint32)(fade*1000));
    } else {
        impl::AudioHaltBackground(_player);
    }
    _music = nullptr;
    _playing = false;
    clear();
}


#pragma mark -
#pragma mark Playback Attributes
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
float MusicQueue::getDuration() const {
    CUAssertLog(_music != nullptr, "Attempt to query time with no music asset");
    return (float)_music->getDuration();
}

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
float MusicQueue::getCurrentTime() const {
    CUAssertLog(_music != nullptr, "Attempt to query time with no music asset");
    return (float)impl::AudioGetBackgroundTime(_player);
}

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
void MusicQueue::setCurrentTime(float time, bool force) {
    CUAssertLog(_music != nullptr, "Attempt to set time with no music asset");
    
    impl::AudioSetBackgroundTime(_player, time);
    if (_paused && force) {
        resume();
    }
}

/**
 * Sets the volume (0 to 1) of the asset being played.
 *
 * This only sets the volume for the asset at the head of the queue.
 * All other music assets in the queue are ignored.
 *
 * @param  volume   the volume (0 to 1) to play the asset.
 */
void MusicQueue::setVolume(float volume) {
    CUAssertLog(_music != nullptr, "Attempt to set the volume with no music asset");
    CUAssertLog(volume >=0 && volume <= 1, "The volume %.3f is out of range",volume);
    _settings.volume = volume;
    impl::AudioSetBackgroundVolume(_player, volume);
}

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
void MusicQueue::setLoop(bool loop) {
    CUAssertLog(_music != nullptr, "Attempt to set the loop with no music asset");
    _settings.loop = loop;
    impl::AudioSetBackgroundLoop(_player, loop);
}
