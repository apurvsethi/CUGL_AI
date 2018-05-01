//
//  CUAudioEngine.cpp
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
#include <cugl/cugl.h>
#include "platform/CUAudioEngine-impl.h"
#include "CUSoundChannel.h"
#include "CUMusicQueue.h"

using namespace cugl;

#pragma mark -
#pragma mark Event Dispatch

/** Reference to the sound engine singleton */
AudioEngine* AudioEngine::_gEngine = nullptr;


#pragma mark -
#pragma mark Constructors
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
bool AudioEngine::init(unsigned int channels) {
    CUAssertLog(channels, "The number of channels must be non-zero");
    
    if (!cugl::impl::AudioStart(AUDIO_FREQUENCY, channels, AUDIO_OUTPUT_CHANNELS)) {
        return false;
    }
    
    _capacity = channels;
    for(int ii = 0; ii < _capacity; ii++) {
        std::shared_ptr<SoundChannel> entity = SoundChannel::alloc(ii);
        _channels.push_back(entity);
    }
    _mqueue = MusicQueue::alloc();
    
    // Initialize callbacks here
    return true;
}

/**
 * Releases all resources for this singleton audio engine.
 *
 * Sounds and music assets can no longer be loaded. If you need to use the
 * engine again, you must call init().
 */
void AudioEngine::dispose() {
    if (_capacity) {
        _mqueue = nullptr;
        _channels.clear();
        _capacity = 0;
        
        cugl::impl::AudioStop();
    }
}


#pragma mark -
#pragma mark Audio Helpers
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
void AudioEngine::gcMusic(bool status) {
    if (_mqueue == nullptr) {
        return;
    }
    std::shared_ptr<Music> prev = _mqueue->getCurrent();
    _mqueue->advance();
    if (_musicCB) {
        _musicCB(prev.get(),status);
    }
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
void AudioEngine::gcEffect(int id, bool status) {
    SoundChannel* channel = _channels[id].get();
    if (channel->isDeleted()) {
        channel->resetDelete();
        return;
    } else if (!channel->attached()) {
        return;
    }
    
    std::string key = channel->getPrimaryKey();
    removeKey(key);
    if (channel->attached() == 2) {
        channel->advance();
    } else {
        channel->clear();
    }
    if (_soundCB) {
        _soundCB(key,status);
    }

}

/**
 * Purges this key from the list of active effects.
 *
 * This method is not the same as stopping the channel. A channel may play a
 * little longer after the key is removed.  This is simply a clean-up method.
 *
 * @remove key  The key to purge from the list of active effects.
 */
void AudioEngine::removeKey(std::string key) {
    _effects.erase(key);
    for(auto it = _equeue.begin(); it != _equeue.end(); ) {
        if (*it == key) {
            it = _equeue.erase(it);
            break;
        } else {
            it++;
        }
    }
}


#pragma mark -
#pragma mark Static Accessors
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
void AudioEngine::start(unsigned int channels) {
    if (_gEngine != nullptr) {
        return;
    }
    _gEngine = new AudioEngine();
    if (!_gEngine->init(channels)) {
        delete _gEngine;
        _gEngine = nullptr;
        CUAssertLog(false,"Sound engine failed to initialize");
    }
}

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
void AudioEngine::stop() {
    if (_gEngine == nullptr) {
        return;
    }
    delete _gEngine;
    _gEngine = nullptr;
}


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
void AudioEngine::playMusic(const std::shared_ptr<Music>& music, bool loop, float volume, float fade) {
    _mqueue->stop();
    float vol = (volume >= 0 ? volume : music->getVolume());
    _mqueue->enqueue(music,vol,loop,fade);
    _mqueue->play();
}

/**
 * Returns the music asset currently playing
 *
 * If there is no active background music, this method returns nullptr.
 *
 * @return the music asset currently playing
 */
const Music* AudioEngine::currentMusic() const {
    return _mqueue->getCurrent().get();
}

/**
 * Returns the current state of the background music
 *
 * @return the current state of the background music
 */
AudioEngine::State AudioEngine::getMusicState() const {
    if (_mqueue == nullptr) {
        return State::INACTIVE;
    } else if (_mqueue->isPaused()) {
        return State::PAUSED;
    } else if (!_mqueue->isStopped()) {
        return State::PLAYING;
    }
    return State::INACTIVE;
}

/**
 * Returns true if the background music is in a continuous loop.
 *
 * If there is no active background music, this method will return false.
 *
 * @return true if the background music is in a continuous loop.
 */
bool AudioEngine::isMusicLoop() const {
    if (_mqueue == nullptr) {
        return false;
    }
    return _mqueue->getLoop();
}

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
void AudioEngine::setMusicLoop(bool loop) {
    if (_mqueue == nullptr) {
        return;
    }
    return _mqueue->setLoop(loop);
}

/**
 * Returns the volume of the background music
 *
 * If there is no active background music, this method will return 0.
 *
 * @return the volume of the background music
 */
float AudioEngine::getMusicVolume() const {
    if (_mqueue == nullptr) {
        return 0.0f;
    }
    return _mqueue->getVolume();
}

/**
 * Sets the volume of the background music
 *
 * If there is no active background music, this method will raise an error.
 *
 * @param  volume   the volume of the background music
 */
void AudioEngine::setMusicVolume(float volume) {
    if (_mqueue == nullptr) {
        return;
    }
    return _mqueue->setVolume(volume);
}

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
float AudioEngine::getMusicDuration() const {
    if (_mqueue == nullptr) {
        return 0.0;
    }
    return (float)_mqueue->getCurrent()->getDuration();
}

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
float AudioEngine::getMusicElapsed() const {
    if (_mqueue == nullptr) {
        return 0.0;
    }
    return _mqueue->getCurrentTime();
}

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
float AudioEngine::getMusicRemaining() const {
    if (_mqueue == nullptr) {
        return 0.0;
    }
    return (float)_mqueue->getCurrent()->getDuration()-_mqueue->getCurrentTime();
}

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
void AudioEngine::setMusicElapsed(float time) {
    if (_mqueue == nullptr) {
        return;
    }
    return _mqueue->setCurrentTime((float)time);
}

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
 *
 * @param  time  the new time remaining of the background music
 */
void AudioEngine::setMusicRemaining(float time) {
    if (_mqueue == nullptr) {
        return;
    }
    float remain = (float)_mqueue->getCurrent()->getDuration()-time;
    return _mqueue->setCurrentTime(remain);
}

/**
 * Stops the background music and clears the entire queue.
 *
 * Before the music is stopped, this method gives the user an option to
 * fade out the music.  If the argument is 0, it will halt the music
 * immediately. Otherwise it will fade to completion over the given number
 * of seconds.
 *
 * This method also clears the queue of any further music.
 *
 * @param fade  The number of seconds to fade out
 */
void AudioEngine::stopMusic(float fade) {
    if (_mqueue == nullptr) {
        return;
    }
    return _mqueue->stop(fade);
}

/**
 * Clears the music queue, but does not release any other resources.
 *
 * This method does not stop the current background music from playing. It
 * only clears pending music assets from the queue.
 */
void AudioEngine::clearMusicQueue() {
    if (_mqueue == nullptr) {
        return;
    }
    return _mqueue->clear();
}

/**
 * Pauses the background music, allowing it to be resumed later.
 *
 * This method has no effect on the music queue.
 */
void AudioEngine::pauseMusic() {
    if (_mqueue == nullptr) {
        return;
    } else if (!_mqueue->isStopped()) {
        _mqueue->pause();
    }
}

/**
 * Resumes the background music assuming that it was paused previously.
 *
 * This method has no effect on the music queue.
 */
void AudioEngine::resumeMusic() {
    if (_mqueue == nullptr) {
        return;
    } else if (_mqueue->isPaused()) {
        _mqueue->resume();
    }
}


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
void AudioEngine::queueMusic(const std::shared_ptr<Music>& music, bool loop, float volume, float fade) {
    if (_mqueue == nullptr) {
        return;
    }
    float vol = (volume >= 0 ? volume : music->getVolume());
    _mqueue->enqueue(music,vol,loop,fade);
    if (_mqueue->isStopped()) {
        _mqueue->play();
    }
}

/**
 * Returns the list of assets for the music queue
 *
 * @return the list of assets for the music queue
 */
const std::vector<const Music*> AudioEngine::getMusicQueue() const {
    if (_mqueue == nullptr) {
        return std::vector<const Music*>();
    }
    return _mqueue->getQueue();
}

/**
 * Returns the size of the music queue
 *
 * @return the size of the music queue
 */
size_t AudioEngine::getMusicQueueSize() const {
    if (_mqueue == nullptr) {
        return 0;
    }
    return _mqueue->size();
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
void AudioEngine::advanceMusicQueue(unsigned int steps) {
    if (_mqueue == nullptr) {
        return;
    }
    return _mqueue->advance(steps);
}


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
bool AudioEngine::playEffect(const std::string& key, const std::shared_ptr<Sound>& sound,
                             bool loop, float volume, bool force) {
    if (isActiveEffect(key)) {
        if (force) {
            stopEffect(key);
        } else {
            CULogError("Sound effect key is in use");
            return false;
        }
    }
    
    bool shadow = false;
    int audioID = -1;
    for(auto it = _channels.begin(); audioID == -1 && it != _channels.end(); ++it) {
        if (!(*it)->attached()) {
            audioID = (*it)->getId();
        }
    }
    
    // Search for those queued for deletion if necessary
    for(auto it = _channels.begin(); audioID == -1 && it != _channels.end(); ++it) {
        if ((*it)->isStopped() && (*it)->attached() == 1) {
            audioID = (*it)->getId();
            shadow = true;
        }
    }
    
    if (audioID == -1) {
        if (force) {
            std::string altkey = _equeue.front();
            audioID = _effects[altkey];
            stopEffect(altkey);
        } else {
            // Fail if nothing available
            CULogError("No available sound channels");
            return false;
        }
    }
    
    float vol = (volume >= 0 ? volume : sound->getVolume());
    if (shadow) {
        std::shared_ptr<SoundChannel> thechannel = _channels[audioID];
        thechannel->attach(key,sound,vol,loop);
        Application::get()->schedule([=] {
            if (thechannel->attached() == 2) {
                thechannel->advance();
            }
            return false;
        });
    } else {
        _channels[audioID]->attach(key,sound,vol,loop);
        _channels[audioID]->play();
    }
    _effects.emplace(key,audioID);
    _equeue.push_back(key);
    return true;
}

/**
 * Returns the current state of the sound effect for the given key.
 *
 * If there is no sound effect for the given key, it returns State::INACTIVE.
 *
 * @param  key      the reference key for the sound effect
 *
 * @return the current state of the sound effect for the given key.
 */
AudioEngine::State AudioEngine::getEffectState(const std::string& key) const {
    if (_effects.find(key) == _effects.end()) {
        return State::INACTIVE;
    }
    int id = _effects.at(key);
    return _channels.at(id)->isPaused() ? State::PAUSED : State::PLAYING;
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
bool AudioEngine::isEffectLoop(const std::string& key) const {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());

    int id = _effects.at(key);
    return _channels.at(id)->getLoop();
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
const Sound* AudioEngine::currentEffect(const std::string& key) const {
    if (_effects.find(key) == _effects.end()) {
        return nullptr;
    }
    SoundChannel* channel = _channels.at(_effects.at(key)).get();
    if (channel->getShadow()) {
        return channel->getShadow().get();
    }
    return channel->getPrimary().get();
}

/**
 * Sets whether the sound effect is in a continuous loop.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 * @param  loop     whether the sound effect is in a continuous loop
 */
void AudioEngine::setEffectLoop(const std::string& key, bool loop) {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());

    int id = _effects.at(key);
    _channels[id]->setLoop(loop);
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
float AudioEngine::getEffectVolume(const std::string& key) const {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());

    int id = _effects.at(key);
    return _channels.at(id)->getVolume();
}

/**
 * Sets the current volume of the sound effect.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 * @param  volume   the current volume of the sound effect
 */
void AudioEngine::setEffectVolume(const std::string& key, float volume) {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());

    int id = _effects.at(key);
    _channels[id]->setVolume(volume);
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
float AudioEngine::getEffectDuration(const std::string& key) const {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());
    
    int id = _effects.at(key);
    return _channels.at(id)->getDuration();
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
float AudioEngine::getEffectElapsed(const std::string& key) const {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());

    int id = _effects.at(key);
    return _channels.at(id)->getCurrentTime();
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
float AudioEngine::getEffectRemaining(const std::string& key) const {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());
    
    int id = _effects.at(key);
    float duration = _channels.at(id)->getDuration();
    return duration - _channels.at(id)->getCurrentTime();
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
void AudioEngine::setEffectElapsed(const std::string& key, float time) {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());
    
    int id = _effects.at(key);
    _channels.at(id)->setCurrentTime(time);
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
void AudioEngine::setEffectRemaining(const std::string& key, float time) {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());
    
    int id = _effects.at(key);
    float duration = _channels.at(id)->getDuration();
    _channels.at(id)->setCurrentTime(duration-time);
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
void AudioEngine::stopEffect(const std::string& key) {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());
    
    int id = _effects.at(key);
    _channels.at(id)->stop();
    removeKey(key);
}

/**
 * Pauses the sound effect for the given key.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 */
void AudioEngine::pauseEffect(const std::string& key) {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());
    
    int id = _effects.at(key);
    CUAssertLog(!_channels.at(id)->isPaused(), "The sound for that effect is already paused");
    _channels.at(id)->pause();
}

/**
 * Resumes the sound effect for the given key.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 */
void AudioEngine::resumeEffect(std::string key) {
    CUAssertLog(_effects.find(key) != _effects.end(),
                "There is no active sound with key '%s'",key.c_str());
    
    int id = _effects.at(key);
    CUAssertLog(_channels.at(id)->isPaused(), "The sound for that effect is not paused");
    _channels.at(id)->resume();
}

/**
 * Stops all sound effects, removing them from the engine.
 *
 * You will need to add the effects again if you wish to replay them.
 */
void AudioEngine::stopAllEffects() {
    for(auto it = _channels.begin(); it != _channels.end(); ++it) {
        if (!(*it)->isStopped()) {
            (*it)->stop();
        }
    }
    _effects.clear();
    _equeue.clear();
}

/**
 * Pauses all sound effects, allowing them to be resumed later.
 *
 * Sound effects already paused will remain paused.
 */
void AudioEngine::pauseAllEffects() {
    for(auto it = _channels.begin(); it != _channels.end(); ++it) {
        if (!(*it)->isStopped() && !(*it)->isPaused()) {
            (*it)->pause();
        }
    }
}

/**
 * Resumes all paused sound effects.
 */
void AudioEngine::resumeAllEffects() {
    for(auto it = _channels.begin(); it != _channels.end(); ++it) {
        if ((*it)->isPaused()) {
            (*it)->resume();
        }
    }
}


#pragma mark -
#pragma mark Global Management
/**
 * Stops all sounds, both music and sound effects.
 *
 * This effectively clears the sound engine.
 */
void AudioEngine::stopAll() {
    stopAllEffects();
    stopMusic();
}

/**
 * Pauses all sounds, both music and sound effects.
 *
 * This method allows them to be resumed later. You should generally
 * call this method just before the app pages to the background.
 */
void AudioEngine::pauseAll() {
    pauseAllEffects();
    pauseMusic();
}

/**
 * Resumes all paused sounds, both music and sound effects.
 *
 * You should generally call this method right after the app returns
 * from the background.
 */
void AudioEngine::resumeAll() {
    resumeAllEffects();
    resumeMusic();
}
