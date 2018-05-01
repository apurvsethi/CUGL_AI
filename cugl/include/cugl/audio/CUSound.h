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
#ifndef __CU_SOUND_H__
#define __CU_SOUND_H__
#include <cugl/base/CUBase.h>

#pragma mark -
#pragma mark Sound Class

namespace cugl {

// We use the impl namespace for platform-dependent data.
namespace impl {
    /**
     * Reference to an platform-specific audio format for PCM data
     *
     * We present this as a struct, as it has more type information than a void
     * pointer, and is equally safe across C++ and Objective-C boundaries.  The
     * exact representation of the struct is platform specific.
     */
    struct AudioBuffer;
}
    
/**
 * Class provides a reference to a pre-loaded asset.
 *
 * Sound assets are loaded entirely into memory.  They are not streamed. 
 * Therefore, this type of asset should be reserved for low-memory footprint
 * sounds such as sound effects. Music files should be streamed and processed
 * as a {@link Music} asset instead.
 *
 * As a general rule, it is best for these assets to be WAV files. There are
 * no cross-platform lossless encodings for both Androi and iOS.  For lossy
 * encodings, only OGG Vorbis is good enough for sound effects.
 *
 * The internal representation of the sound buffer is platform dependent.
 * You should never attempt to access the buffer directly.
 */
class Sound {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CU_DISALLOW_COPY_AND_ASSIGN(Sound);
    
    /** The source file for the sound */
    std::string  _source;
    /** The (platform-specific) data for the file */
    impl::AudioBuffer* _buffer;
    /** The default volume for this sound */
    float _volume;
    
#pragma mark -
#pragma mark Constructors
public:
    /**
     * Creates a degenerate sound asset with no buffer.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an asset on
     * the heap, use one of the static constructors instead.
     */
    Sound() : _source(""), _buffer(nullptr), _volume(1) {}
    
    /**
     * Deletes this sound asset, disposing of all resources.
     */
    ~Sound() { dispose(); }
    
    /**
     * Deletes the sound resources and resets all attributes.
     *
     * This will delete the preloaded sound buffer. You must reinitialize the
     * sound data to use it.
     */
    void dispose();
    
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
    bool init(const std::string& source);

    /**
     * Initializes a new sound asset for the given source file.
     *
     * The sound will be decompressed into PCM (e.g. WAV data) which is
     * possibly large. This will be stored in a platform specific buffer.
     *
     * @param  source   the source file for the sound asset
     *
     * @return true if the sound asset was initialized successfully
     */
    bool init(const char* source) {
        return init(std::string(source));
    }

#pragma mark -
#pragma mark Static Constructors
    /**
     * Returns a newly allocated sound asset for the given source file.
     *
     * The sound will be decompressed into PCM (e.g. WAV data) which is
     * possibly large. This will be stored in a platform specific buffer.
     *
     * @param  source   the source file for the sound asset
     *
     * @return a newly allocated sound asset for the given source file.
     */
    static std::shared_ptr<Sound> alloc(const std::string& source) {
        std::shared_ptr<Sound> result = std::make_shared<Sound>();
        return (result->init(source) ? result : nullptr);
    }
    
    /**
     * Returns a newly allocated sound asset for the given source file.
     *
     * The sound will be decompressed into PCM (e.g. WAV data) which is
     * possibly large. This will be stored in a platform specific buffer.
     *
     * @param  source   the source file for the sound asset
     *
     * @return a newly allocated sound asset for the given source file.
     */
    static std::shared_ptr<Sound> alloc(const char* source) {
        std::shared_ptr<Sound> result = std::make_shared<Sound>();
        return (result->init(source) ? result : nullptr);
    }
    
#pragma mark Attributes
    /**
     * Returns the source file for this sound asset.
     *
     * @return the source file for this sound asset.
     */
    const std::string& getSource() const { return _source; }
    
    /**
     * Returns the file suffix for this sound asset.
     *
     * Until we expose more functionality about the encoding, this is a poor
     * man's way of determining the file format.
     *
     * @return the file suffix for this sound asset.
     */
    std::string getSuffix() const {
        size_t pos = _source.rfind(".");
        return (pos == std::string::npos ? "" : _source.substr(pos));
    }

    /**
     * Returns the length of this sound asset in seconds.
     *
     * Because the asset is fully decompressed at load time, the result of
     * this method is reasonably accurate.
     *
     * @return the length of this sound asset in seconds.
     */
    double getDuration() const;
        
    /**
     * Returns the sample rate of this sound asset.
     *
     * @return the sample rate of this sound asset.
     */
    double getSampleRate() const;
    
    /**
     * Returns the frame length of this sound asset.
     *
     * The frame length is the duration times the sample rate.
     *
     * @return the frame length of this sound asset.
     */
    Uint64 getLength() const;
    
    /**
     * Returns the number of channels used by this sound asset 
     *
     * A value of 1 means mono, while 2 means stereo. Depending on the file
     * format, other channels are possible. For example, 6 channels means
     * support for 5.1 surround sound.
     *
     * @return the number of channels used by this sound asset
     */
    Uint32 getChannels() const;
    
    /**
     * Returns the default volume of this sound asset.
     *
     * This default value will be used when the sound is played without a 
     * specified volume. The value is between 0 and 1, where 0 means muted 
     * and 1 is maximum volume.
     *
     * @return the default volume of this sound asset.
     */
    float getVolume() const { return _volume; }

    /**
     * Sets the default volume of this sound asset.
     *
     * This default value will be used when the sound is played without a
     * specified volume. The value is between 0 and 1, where 0 means muted
     * and 1 is maximum volume.
     *
     * @param volume    The default volume of this sound asset.
     */
    void setVolume(float volume);
    
    /** Allow a sound channel to access the internal buffers */
    friend class SoundChannel;
};

}

#endif /* __CU_SOUND_H__ */
