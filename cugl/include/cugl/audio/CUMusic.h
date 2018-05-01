//
//  CUMusic.h
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
#ifndef __CU_MUSIC_H__
#define __CU_MUSIC_H__
#include <cugl/base/CUBase.h>

namespace cugl {

// We use the impl namespace for platform-dependent data.
namespace impl {
    /**
     * Reference to an platform-specific music format.
     *
     * We present this as a struct, as it has more type information than a void
     * pointer, and is equally safe across C++ and Objective-C boundaries.  The
     * exact representation of the struct is platform specific.
     */
    struct AudioStream;
}
    
/**
 * Class provides a reference to a streaming asset.
 *
 * Music assets are not loaded entirely into memory. Instead, they are streamed
 * from a file. This assets should not be used for low-latency sound effects.
 * Those should be processed as a {@link Sound} asset instead.
 *
 * These best cross-platform options for this asset are MP3 and OGG Vorbis.
 * All other options (AAC, FLAC, M4A) only work on specific platforms.
 *
 * The internal representation of the music buffer is platform dependent.
 * You should never attempt to access the buffer directly.
 */
class Music {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CU_DISALLOW_COPY_AND_ASSIGN(Music);
    
    /** The source file for the music */
    std::string    _source;
    /** The (platform-specific) data for the file */
    impl::AudioStream* _buffer;
    /** The default volume for the music */
    float _volume;
    
public:
    /** 
     * The type of supported music files
     *
     * Beware that not all platforms support all music types.
     * Right now, the only guaranteed assets that work on more than one
     * platform are WAV and MP3 files.
     */
    enum class Type : int {
        /** Uncompressed WAV (why are you doing this for music) */
        WAV = 0,
        /** The good old standard, royalty free as of April 2017 */
        MP3 = 1,
        /** The Apple alternative to MP3 */
        AAC = 2,
        /** The Linux alternative to MP3 */
        OGG = 3,
        /** The Apple lossless encoding */
        M4A = 4,
        /** The Linux lossless encoding */
        FLAC = 5,
        /** Either the encoding is unknown or unsupported */
        UNSUPPORTED = 6
    };
    
    
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a degenerate music asset with no buffer.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
     */
    Music() : _source(""), _buffer(nullptr) {}
    
    /**
     * Deletes this music asset, disposing of all resources.
     */
    ~Music() { dispose(); }
    
    /**
     * Deletes the music resources and resets all attributes.
     *
     * This will delete decoding information for the stream. You must 
     * reinitialize the music data to use it.
     */
    void dispose();
    
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
    bool init(const std::string& source);
    
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
    bool init(const char* source) {
        return init(std::string(source));
    }
    
#pragma mark -
#pragma mark Static Constructors
    /**
     * Returns a newly allocated music asset for the given source file.
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
     * @return a newly allocated music asset for the given source file.
     */
    static std::shared_ptr<Music> alloc(const std::string& source) {
        std::shared_ptr<Music> result = std::make_shared<Music>();
        return (result->init(source) ? result : nullptr);
    }
    
    /**
     * Returns a newly allocated music asset for the given source file.
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
     * @return a newly allocated music asset for the given source file.
     */
    static std::shared_ptr<Music> alloc(const char* source) {
        std::shared_ptr<Music> result = std::make_shared<Music>();
        return (result->init(source) ? result : nullptr);
    }
    
#pragma mark Attributes
    /**
     * Returns the source file for this music asset.
     *
     * @return the source file for this music asset.
     */
    const std::string& getSource() const { return _source; }
    
    /**
     * Returns the file suffix for this music asset.
     *
     * While we have another function below to return the music encoding
     * type, we still include this method to unify it with sound assets.
     *
     * @return the file suffix for this music asset.
     */
    std::string getSuffix() const {
        size_t pos = _source.rfind(".");
        return (pos == std::string::npos ? "" : _source.substr(pos));
    }
    
    /**
     * Returns the length of this music asset in seconds.
     *
     * This information is retrieved from the decoder. As the file is completely
     * decoded at load time, the result of this method is reasonably accurate.
     *
     * @return the length of this music asset in seconds.
     */
    double getDuration() const;
    
    /**
     * Returns the encoding type for this music asset.
     *
     * @return the encoding type for this music asset.
     */
    Type getType() const;
    
    /**
     * Returns the default volume of this music asset.
     *
     * This default value will be used when the sound is played without a
     * specified volume. The value is between 0 and 1, where 0 means muted
     * and 1 is maximum volume.
     *
     * @return the default volume of this music asset.
     */
    float getVolume() const { return _volume; }
    
    /**
     * Sets the default volume of this music asset.
     *
     * This default value will be used when the sound is played without a
     * specified volume. The value is between 0 and 1, where 0 means muted
     * and 1 is maximum volume.
     *
     * @param volume    The default volume of this music asset.
     */
    void setVolume(float volume);

    /** Allow a music queue to access the internal buffers */
    friend class MusicQueue;
};

}

#endif /* __CU_MUSIC_H__ */
