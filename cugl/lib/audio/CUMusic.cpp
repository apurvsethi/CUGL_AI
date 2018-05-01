//
//  CUMusic.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides a stream music asset. While we support a wide range of
//  formats, only WAV, MP3, OGG are guaranteed to be supported on all CUGL
//  platforms (Android and iOS have very little overlap here).  MP3 files are
//  under patent until April 2017.  OGG is a popular alternative in games, but
//  it is still a lossy encoding. WAV files are extremely large and are not
//  suitable for streaming. If anything, you want want to create platform
//  specific assets and differentiate them in your asset directory.
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
#include <cugl/audio/CUMusic.h>
#include <cugl/audio/CUAudioEngine.h>
#include <cugl/util/CUDebug.h>
#include "platform/CUAudioEngine-impl.h"

using namespace cugl;

/**
 * Deletes the music resources and resets all attributes.
 *
 * This will delete decoding information for the stream. You must
 * reinitialize the music data to use it.
 */
void Music::dispose() {
    _source.clear();
    if (_buffer) {
        cugl::impl::AudioFreeStream(_buffer);
        _buffer = nullptr;
    }
}

/**
 * Initializes a new music asset for the given source file.
 *
 * The sound will be decoded immediately.  However, the actual data will
 * be streamed from disk.
 *
 * Be aware that music files are very not platform.  SDL wants to support
 * the Linux options (OGG, FLAC), while Apple wants to support its own
 * options (AAC, M4A).  We are working on fixing this, but until we do,
 * you should probably make different sound assets for the Android and
 * Apple platforms (unless you just want to use MP3 and WAV).
 *
 * @param  source   the source file for the music asset
 *
 * @return true if the music asset was initialized successfully
 */
bool Music::init(const std::string& source) {
    CUAssertLog(AudioEngine::get(), "The audio system must be initialized before loading music assets");
    
    _source = source;
    _buffer = cugl::impl::AudioLoadStream(source.c_str());
    return (bool)_buffer;
}

/**
 * Returns the length of this music asset in seconds.
 *
 * This information is retrieved from the decoder. As the file is completely
 * decoded at load time, the result of this method is reasonably accurate.
 *
 * @return the length of this music asset in seconds.
 */
double Music::getDuration() const {
    return  cugl::impl::AudioGetStreamDuration(_buffer);
}

/**
 * Returns the encoding type for this music asset.
 *
 * @return the encoding type for this music asset.
 */
Music::Type Music::getType() const {
    return cugl::impl::AudioGetStreamType(_buffer);
}

/**
 * Sets the default volume of this music asset.
 *
 * This default value will be used when the sound is played without a
 * specified volume. The value is between 0 and 1, where 0 means muted
 * and 1 is maximum volume.
 *
 * @param volume    The default volume of this music asset.
 */
void Music::setVolume(float volume) {
    CUAssertLog(0 <= volume && volume <= 1, "The volume %.3f is out of range",volume);
    _volume = volume;  
}
