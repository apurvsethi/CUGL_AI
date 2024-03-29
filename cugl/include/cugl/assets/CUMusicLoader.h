//
//  CUMusicLoader.h
//  Cornell University Game Library (CUGL)
//
//  This module provides a specific implementation of the Loader class to load
//  music assets (e.g. streaming audio files). A music asset is identified by
//  both its source file and its volume.
//
//  As with all of our loaders, this loader is designed to be attached to an
//  asset manager.  In addition, this class uses our standard shared-pointer
//  architecture.
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
//  Version: 1/7/16
//
#ifndef __CU_MUSIC_LOADER_H__
#define __CU_MUSIC_LOADER_H__
#include <cugl/assets/CULoader.h>
#include <cugl/audio/CUMusic.h>

namespace cugl {
    
/**
 * This class is a implementation of Loader<Music>
 *
 * This asset loader allows us to allocate music assets.  See {@link Music}
 * for the supported audio files.  Since music assets are streamed, you may
 * wish to use a compressed format like MP3 or OGG.
 *
 * Note that this implementation uses a two phase loading system.  First, it
 * loads as much of the asset as possible without accessing the audio engine.
 * This allows us to load the sound asset in a separate thread. It then finishes
 * off the remainder of asset loading using {@link Application#schedule}.  This
 * is a good template for asset loaders in general.
 *
 * As with all of our loaders, this loader is designed to be attached to an
 * asset manager. Use the method {@link getHook()} to get the appropriate
 * pointer for attaching the loader.
 */
class MusicLoader : public Loader<Music> {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CU_DISALLOW_COPY_AND_ASSIGN(MusicLoader);
    
protected:
    /** The default volume for all music assets */
    float _volume;

    /**
     * Finishes loading the music file, setting its default volume.
     *
     * Allocating a music asset can be done safely in a separate thread.
     * However, setting the default volume requires the audio engine, and so
     * this step is not safe to be done in a separate thread.  Instead, it
     * takes place in the main CUGL thread via {@link Application#schedule}.
     *
     * This method supports an optional callback function which reports whether
     * the asset was successfully materialized.
     *
     * @param key       The key to access the asset after loading
     * @param msuic     The music asset partially loaded
     * @param volume    The volume to set for the music asset
     * @param callback  An optional callback for asynchronous loading
     */
    void materialize(const std::string& key, const std::shared_ptr<Music>& music,
                     float volume, LoaderCallback callback);
    
    /**
     * Internal method to support asset loading.
     *
     * This method supports either synchronous or asynchronous loading, as
     * specified by the given parameter.  If the loading is asynchronous,
     * the user may specify an optional callback function.
     *
     * This method will split the loading across the {@link Music#alloc} and
     * the internal {@link materialize} method.  This ensures that asynchronous
     * loading is safe.
     *
     * @param key       The key to access the asset after loading
     * @param source    The pathname to the asset
     * @param callback  An optional callback for asynchronous loading
     * @param async     Whether the asset was loaded asynchronously
     *
     * @return true if the asset was successfully loaded
     */
    virtual bool read(const std::string& key, const std::string& source,
                      LoaderCallback callback, bool async) override;
    
    /**
     * Internal method to support asset loading.
     *
     * This method supports either synchronous or asynchronous loading, as
     * specified by the given parameter.  If the loading is asynchronous,
     * the user may specify an optional callback function.
     *
     * This method will split the loading across the {@link Music#alloc} and
     * the internal {@link materialize} method.  This ensures that asynchronous
     * loading is safe.
     *
     * This version of read provides support for JSON directories. A music
     * directory entry has the following values
     *
     *      "file":         The path to the asset
     *      "volume":       This default sound volume (float)
     *
     * @param json      The directory entry for the asset
     * @param callback  An optional callback for asynchronous loading
     * @param async     Whether the asset was loaded asynchronously
     *
     * @return true if the asset was successfully loaded
     */
    virtual bool read(const std::shared_ptr<JsonValue>& json,
                      LoaderCallback callback, bool async) override;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new, uninitialized music loader
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a loader on
     * the heap, use one of the static constructors instead.
     */
    MusicLoader();
    
    /**
     * Disposes all resources and assets of this loader
     *
     * Any assets loaded by this object will be immediately released by the
     * loader.  However, a music asset may still be available if referenced
     * by another smart pointer.  The audio engine will only release a music
     * header once all smart pointer attached to the asset are null.
     *
     * Once the loader is disposed, any attempts to load a new asset will
     * fail.  You must reinitialize the loader to begin loading assets again.
     */
    void dispose() override {
        _assets.clear();
        _loader = nullptr;
    }
    
    /**
     * Returns a newly allocated music loader.
     *
     * This method bootstraps the loader with any initial resources that it
     * needs to load assets. In particular, the audio engine must be active.
     * Attempts to load an asset before this method is called will fail.
     *
     * This loader will have no associated threads. That means any asynchronous
     * loading will fail until a thread is provided via {@link setThreadPool}.
     *
     * @return a newly allocated music loader.
     */
    static std::shared_ptr<MusicLoader> alloc() {
        std::shared_ptr<MusicLoader> result = std::make_shared<MusicLoader>();
        return (result->init() ? result : nullptr);
    }
    
    /**
     * Returns a newly allocated music loader.
     *
     * This method bootstraps the loader with any initial resources that it
     * needs to load assets. In particular, the audio engine must be active.
     * Attempts to load an asset before this method is called will fail.
     *
     * This loader will have no associated threads. That means any asynchronous
     * loading will fail until a thread is provided via {@link setThreadPool}.
     *
     * @param threads   The thread pool for asynchronous loading
     *
     * @return a newly allocated music loader.
     */
    static std::shared_ptr<MusicLoader> alloc(const std::shared_ptr<ThreadPool>& threads) {
        std::shared_ptr<MusicLoader> result = std::make_shared<MusicLoader>();
        return (result->init(threads) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Properties
    /**
     * Returns the default volume
     *
     * Once set, any future sound asset processed by this loader will have this
     * volume unless otherwise specified.  The default is 1.0 (max volume).
     *
     * @return the default volume
     */
    float getVolume() const     { return _volume; }
    
    /**
     * Sets the default volume
     *
     * Once set, any future sound asset processed by this loader will have this
     * volume unless otherwise specified.  The default is 1.0 (max volume).
     *
     * @param volume    The default volume
     */
    void setVolume(float volume) { _volume = volume; }
};

}
#endif /* __CU_MUSIC_LOADER_H__ */
