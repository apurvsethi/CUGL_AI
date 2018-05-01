//
//  CUSound.h
//  Cornell University Game Library (CUGL)
//
//  This module provides a preloaded sound asset.  In general, these assets
//  should be WAV files (as there is no benefit to compression once they are
//  loaded into memory), but we do have support for other file types. However,
//  at this time, only WAV, MP3, and OGG are guaranteed to be supported on all
//  CUGL platforms (Android and iOS have very little overlap here).
//
//  We have factored out the platform-dependent code and have hidden it
//  behind this class as an abstraction.
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
#include <SDL/SDL_mixer.h>
#include <cugl/audio/CUSound.h>
#include <cugl/util/CUDebug.h>
#include <cugl/audio/CUAudioEngine.h>
#include "platform/CUAudioEngine-impl.h"

using namespace cugl;

/**
 * Deletes the sound resources and resets all attributes.
 *
 * This will delete the preloaded sound buffer. You must reinitialize the
 * sound data to use it.
 */
void Sound::dispose() {
    _source.clear();
    if (_buffer) {
        cugl::impl::AudioFreeBuffer(_buffer);
        _buffer = nullptr;
    }
}

/**
 * Initializes a new sound asset for the given source file.
 *
 * The sound will be decompressed into PCM (e.g. WAV data) which is
 * possibly large. This will be stored in a platform specific buffer.
 *
 * @param  source   the source file for the sound
 *
 * @return true if the sound was initialized successfully
 */
bool Sound::init(const std::string& source) {
    CUAssertLog(AudioEngine::get(), "AudioEngine must be initialized before loading sound assets");

    _source = source;
    _buffer = cugl::impl::AudioLoadBuffer(source.c_str());
    return (bool)_buffer;
}

/**
 * Returns the length of this sound asset in seconds.
 *
 * Because the asset is fully decompressed at load time, the result of
 * this method is reasonably accurate.
 *
 * @return the length of this sound asset in seconds.
 */
double Sound::getDuration() const {
    return (getSampleRate() == 0 ? 0.0 : (double)(getLength()/getSampleRate()));
}

/**
 * Returns the sample rate of this sound asset.
 *
 * @return the sample rate of this sound asset.
 */
double Sound::getSampleRate() const {
    return (_buffer ? cugl::impl::AudioGetBufferSampleRate(_buffer) : 0.0);
}

/**
 * Returns the frame length of this sound asset.
 *
 * The frame length is the duration times the sample rate.
 *
 * @return the frame length of this sound asset.
 */
Uint64 Sound::getLength() const {
    return (_buffer ? cugl::impl::AudioGetBufferFrames(_buffer) : 0);
}

/**
 * Returns the number of channels used by this sound asset
 *
 * A value of 1 means mono, while 2 means stereo. Depending on the file
 * format, other channels are possible.  For example, 6 channels means 
 * support for 5.1 surround sound.
 *
 * @return the number of channels used by this sound asset
 */
Uint32 Sound::getChannels() const {
    return (_buffer ? cugl::impl::AudioGetBufferChannels(_buffer) : 0);
}

/**
 * Sets the default volume of this sound asset.
 *
 * This default value will be used when the sound is played without a
 * specified volume. The value is between 0 and 1, where 0 means muted
 * and 1 is maximum volume.
 *
 * @param volume    The default volume of this sound asset.
 */
void Sound::setVolume(float volume) {
    CUAssertLog(0 <= volume && volume <= 1, "The volume %.3f is out of range",volume);
    _volume = volume;
}
