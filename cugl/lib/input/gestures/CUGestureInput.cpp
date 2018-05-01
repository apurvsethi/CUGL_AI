//
//  CUGestureInput.cpp
//  Cornell University Game Library (CUGL)
//
//  This class provides basic support for custom defined gestures. It uses the
//  SDL implementation of the $1 (dollar) gesture system.  For more information,
//  see the research page
//
//      http://depts.washington.edu/madlab/proj/dollar/index.html
//
//  This class is a singleton and should never be allocated directly.  It
//  should only be accessed via the Input dispatcher.
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
//  Version: 12/20/16
//
#include <cugl/cugl.h>

using namespace cugl;

/** The bizarre normalization factor SDL decided to use */
#define DOLLARSIZE  256

#pragma mark Constructor
/**
 * Creates and initializes a new gesture input device.
 *
 * WARNING: Never allocate a gesture input device directly.  Always use the
 * {@link Input#activate()} method instead.
 */
GestureInput::GestureInput() :
_waits(GestureState::MATCHING),
_match(false),
_tolerance(-1.0f) {
    _state = SDL_GetNumTouchDevices() ? GestureState::MATCHING : GestureState::UNDEFINED;
    _event.fingers = 0;
    _event.error = 0.0f;
}

/**
 * Unintializes this device, returning it to its default state
 *
 * An uninitialized device may not work without reinitialization.
 */
void GestureInput::dispose() {
    _gestures.clear();
    _matchListeners.clear();
    _stateListeners.clear();
    SDL_ClearDollarGestures();
    _loader = nullptr;
}

#pragma mark -
#pragma mark Device State
/**
 * Returns the list of gestures currently recorded
 *
 * @return the list of gestures currently recorded
 */
std::vector<std::string> GestureInput::getGestures() const {
    std::vector<std::string> result;
    result.reserve(_inverses.size());
    for(auto it = _inverses.begin(); it != _inverses.end(); ++it) {
        result.push_back(it->first);
    }
    return result;
}

/**
 * Pauses the input device, temporally disabling gesture matching.
 *
 * Dollar template matching is potentionally very heavy-weight, particularly
 * if there is no assigned error tolerance. However, deactivating the
 * device will cause all of the recorded gestures to be lost.  This method
 * allows you to pause the device, without losing any gestures.
 *
 * Pausing the input device will pause the underlying SDL dollar gesture
 * system.  If you have any other custom classes accessing the SDL dollar
 * gesture system, it will no longer receive events until this device
 * is resumed.  In general, it is a bad idea to have more than on interface
 * to the SDL input API anyway.
 *
 * The device may only be paused if it is currently ready.  Any blocking
 * operations must complete before the device can be paused.
 *
 * @return true if the device was successfully paused
 */
bool GestureInput::pause() {
    if (!ready()) {
        return false;
    }
    changeState(GestureState::PAUSED);
    SDL_EventState(SDL_DOLLARGESTURE,SDL_DISABLE);
    SDL_EventState(SDL_DOLLARRECORD,SDL_DISABLE);
    return true;
}

/**
 * Resumes the input device, temporally disabling gesture matching.
 *
 * Dollar template matching is potentionally very heavy-weight, particularly
 * if there is no assigned error tolerance. However, deactivating the
 * device will cause all of the recorded gestures to be lost.  This method
 * allows you to pause the device, without losing any gestures.
 *
 * Pausing the input device will resume the underlying SDL dollar gesture
 * system.  If you have any other custom classes accessing the SDL dollar
 * gesture system, it will start to receive input events. In general, it
 * is a bad idea to have more than on interface to the SDL input API anyway.
 *
 * @return true if the device was successfully resumed
 */
bool GestureInput::resume() {
    if (_state != GestureState::PAUSED) {
        return false;
    }
    changeState(GestureState::MATCHING);
    SDL_EventState(SDL_DOLLARGESTURE,SDL_ENABLE);
    SDL_EventState(SDL_DOLLARRECORD,SDL_ENABLE);
    return true;
}

/**
 * Records a gesture for the given key.
 *
 * The key will be bound to the next recorded gesture. Gestures can be
 * performed with any number of fingers (the centroid of the fingers must
 * follow the path of the gesture), but the number of fingers must be
 * constant (a finger cannot go down in the middle of a gesture). The path
 * of a gesture is considered the path from the time when the final finger
 * went down, to the first time any finger comes up.
 *
 * This method fails if there is already a gesture for the given key.
 *
 * @param key   The key to bind to the gesture
 *
 * @return true if the key is available for recording
 */
bool GestureInput::record(const std::string& key) {
    if (_state != GestureState::UNDEFINED && _state != GestureState::MATCHING) {
        return false;
    } else if (_inverses.find(key) != _inverses.end()) {
        return false;
    }
    
    _recrd = key;
    changeState(GestureState::RECORDING);
    if (SDL_RecordGesture(-1)) {
        return true;
    }
    
    changeState(GestureState::MATCHING);
    return false;
}

/**
 * Abort a gesture currently being recorded.
 *
 * This method only succeeds if the device is currently in the recording
 * state.  It will delete the gesture on completion.
 *
 * @return true if the recording was successfully aborted
 */
bool GestureInput::abort() {
    if (_state != GestureState::RECORDING) {
        return false;
    }
    
    _recrd = "";
    changeState(GestureState::ABORTING);
    return true;
}

/**
 * Deletes the gesture from the input device
 *
 * This method will fail if the device is not matching and is involved in
 * any blocking operations. It will also fail if there is no gesture with
 * the given name.
 *
 * @param key   The gesture to delete
 *
 * @return true if the gesture was successfully deleted.
 */
bool GestureInput::remove(const std::string& key) {
    if (!ready()) {
        return false;
    } else if (_inverses.find(key) == _inverses.end()) {
        return false;
    }
    
    SDL_GestureID gid = _inverses[key];
    if (SDL_RemoveDollarGesture(gid)) {
        _inverses.erase(key);
        _gestures.erase(gid);
        return true;
    }
    return false;
}

/**
 * Renames the given gesture
 *
 * This method will fail if the device is not matching and is involved in
 * any blocking operations. It will also fail if there is no gesture with
 * the given name.
 *
 * @param key   The gesture to rename
 * @param name  The new gesture name
 *
 * @return true if the gesture was successfully renamed.
 */
bool GestureInput::rename(const std::string& key, const std::string& name) {
    if (_inverses.find(key) == _inverses.end()) {
        return false;
    }
    
    SDL_GestureID gid = _inverses[key];
    _inverses[name] = gid;
    _inverses.erase(key);
    _gestures[gid] = name;
    return true;
}


#pragma mark -
#pragma mark Serialization

/**
 * Reads the stored gestures from the given gesture file.
 *
 * This method is called directly for synchronous loading and as part of
 * the thread pool in asynchronous loading. The optional callback function
 * is called with the return value of the function.
 *
 * The gesture file will be loaded in a non-destructive manner. That means
 * if there is already a recorded gesture with the given key, it will not
 * load a new version from the file.  You will need to delete the recorded
 * version before loading the file.
 *
 * Technically, gesture files are directories (a later refactoring should
 * convert them into custom ZIP files). The directory has an index JSON
 * mapping keys to hashes.  For each hash, there is a separate SDL file
 * with the gesture information.
 *
 * Because of this structure, some gestures may be read into memory even
 * if the entire read fails (because of a bad gesture).
 *
 * @param file      The name of the gesture file (directory)
 * @param callback  An optional callback for asynchronous loading.
 *
 * @return true if the entire read was successful
 */
bool GestureInput::read(const std::string& file, std::function<void(bool success)> callback) {
    bool success = true;
    std::string sep = Pathname::getSeparator();
    std::string index = file+sep+"index.json";
    
    // Get the index file
    std::shared_ptr<JsonReader> reader = JsonReader::alloc(index);
    if (reader == nullptr) {
        success = false;
    } else {
        std::shared_ptr<JsonValue> json = reader->readJson();
        reader->close();
    
        for(int ii = 0; ii < json->size(); ii++) {
            std::shared_ptr<JsonValue> child = json->get(ii);
            std::string key = child->key();
            SDL_GestureID gid = cugl::stos64(child->asString());
            
            if (_inverses.find(key) == _inverses.end()) {
                std::string data = file+sep+child->asString()+".dat";
                SDL_RWops* ops = SDL_RWFromFile(data.c_str(), "rb");
                if (ops) {
                    for(int ii = 0; ii < SDL_GetNumTouchDevices(); ii++) {
                        SDL_LoadDollarTemplates(SDL_GetTouchDevice(ii), ops);
                    }
                    SDL_RWclose(ops);
                    _inverses[key] = gid;
                    _gestures[gid] = key;
                } else {
                    success = false;
                }
            }
        }
    }
    
    // Callbacks should always be invoked in the main thread.
    if (callback != nullptr) {
        Application::get()->schedule([=](void) {
            callback(success);
            return false;
        });
    }
    return success;
}

/**
 * Writes the current gestures to the given gesture file.
 *
 * This method is called directly for synchronous writing and as part of
 * the thread pool in asynchronous writing. The optional callback function
 * is called with the return value of the function.
 *
 * Technically, gesture files are directories (a later refactoring should
 * convert them into custom ZIP files). The directory has an index JSON
 * mapping keys to hashes.  For each hash, there is a separate SDL file
 * with the gesture information.
 *
 * Because of this structure, some gestures may be written to a file even
 * if the entire write fails (because of bad access or other issues).
 *
 * @param file      The path to the gesture file (directory)
 * @param callback  An optional callback for asynchronous loading.
 *
 * @return true if the entire write was successful
 */
bool GestureInput::store(Pathname& file, std::function<void(bool success)> callback) {
    bool success = true;
    if (file.exists() && !file.isDirectory()) {
        success = false;
    } else if (!file.exists()) {
        if (!file.createDirectory()) {
            success = false;
        }
    }
    
    if (success) {
        Pathname index(file,"index.json");
        std::shared_ptr<JsonValue> json = JsonValue::alloc(JsonValue::Type::ObjectType);
        if (json == nullptr) {
            success = false;
        } else {
            for(auto it = _inverses.begin(); it != _inverses.end(); ++it) {
                std::string hash = cugl::to_string(it->second);
                json->appendChild(it->first, JsonValue::alloc(hash));
                Pathname data(file,hash+".dat");
                SDL_RWops* ops = SDL_RWFromFile(data.getAbsoluteName().c_str(), "wb");
                if (ops) {
                    SDL_SaveDollarTemplate(it->second, ops);
                    SDL_RWclose(ops);
                } else {
                    success = false;
                }
            }
    
            std::shared_ptr<JsonWriter> writer = JsonWriter::alloc(index.getAbsoluteName());
            writer->writeJson(json);
            writer->close();
        }
    }
    
    // Callbacks should always be invoked in the main thread.
    if (callback != nullptr) {
        Application::get()->schedule([=](void) {
            callback(success);
            return false;
        });
    }
    return success;
}

#pragma mark Serialization
/**
 * Synchronously loads the stored gestures from the given gesture file.
 *
 * The gesture file will be loaded in a non-destructive manner. That means
 * if there is already a recorded gesture with the given key, it will not
 * load a new version from the file.  You will need to delete the recorded
 * version before loading the file.
 *
 * Technically, gesture files are directories (a later refactoring should
 * convert them into custom ZIP files). The directory has an index JSON
 * mapping keys to hashes.  For each hash, there is a separate SDL file
 * with the gesture information. For this method, the gesture directory is
 * assumed to be in the application save directory.
 *
 * Because of this structure, some gestures may be read into memory even
 * if the entire read fails (because of a bad gesture).
 *
 * This method will fail if the device is not ready or involved in any
 * blocking operations.
 *
 * @param file      The name of the gesture file (directory)
 *
 * @return true if the gestures were loaded successfully
 */
bool GestureInput::load(const Pathname& file) {
    if (!ready()) {
        return false;
    }
    
    return read(file.getAbsoluteName());
}

/**
 * Asynchronously loads the stored gestures from the given gesture file.
 *
 * The gesture file will be loaded in a non-destructive manner. That means
 * if there is already a recorded gesture with the given key, it will not
 * load a new version from the file.  You will need to delete the recorded
 * version before loading the file.
 *
 * Technically, gesture files are directories (a later refactoring should
 * convert them into custom ZIP files). The directory has an index JSON
 * mapping keys to hashes.  For each hash, there is a separate SDL file
 * with the gesture information. For this method, the gesture directory is
 * assumed to be in the application save directory.
 *
 * Because of this structure, some gestures may be read into memory even
 * if the entire read fails (because of a bad gesture).
 *
 * This method will load immediately if the device is ready.  If it is not
 * actively loading or saving a file, it will block until it can load.
 * Otherwise, it will fail immediately.  The callback function (which can
 * be nullptr) will be called with the result when it is done.
 *
 * @param file      The name of the gesture file (directory)
 * @param callback  The callback to report success or failure
 */
void GestureInput::loadAsync(const Pathname& file, std::function<void(bool success)> callback) {
    // Fail immediately on serialization conflict
    if (_state == GestureState::STORING || _state == GestureState::LOADING) {
        if (callback != nullptr) {
            Application::get()->schedule([=](void) {
                callback(false);
                return false;
            });
        }
        return;
    }
    
    if (_loader == nullptr) {
        _loader = ThreadPool::alloc(1);
    }
    
    if (ready()) {
        changeState(GestureState::LOADING);
    } else {
        _waits = GestureState::LOADING;
    }
    
    _loader->addTask([=](void) {
        std::unique_lock<std::mutex> block(this->_lock);
        
        // These are the only two states we block on
        while (this->_state != GestureState::MATCHING && this->_state != GestureState::LOADING) {
            _monitor.wait(block);
        }
        if (this->_state == GestureState::LOADING) {
            this->read(file.getAbsoluteName(),callback);
        }
        this->changeState(this->_waits,false);
    });
}

/**
 * Synchronously loads the stored gestures from the given gesture file.
 *
 * The gesture file will be loaded in a non-destructive manner. That means
 * if there is already a recorded gesture with the given key, it will not
 * load a new version from the file.  You will need to delete the recorded
 * version before loading the file.
 *
 * Technically, gesture files are directories (a later refactoring should
 * convert them into custom ZIP files). The directory has an index JSON
 * mapping keys to hashes.  For each hash, there is a separate SDL file
 * with the gesture information. For this method, the gesture directory is
 * assumed to be in the asset directory.
 *
 * Because of this structure, some gestures may be read into memory even
 * if the entire read fails (because of a bad gesture).
 *
 * This method will fail if the device is not ready or involved in any
 * blocking operations.
 *
 * @param file      The name of the gesture file (directory)
 *
 * @return true if the gestures were loaded successfully
 */
bool GestureInput::loadAsset(const std::string& file) {
    if (!ready()) {
        return false;
    }
    
    return read(file);
}

/**
 * Asynchronously loads the stored gestures from the given gesture file.
 *
 * The gesture file will be loaded in a non-destructive manner. That means
 * if there is already a recorded gesture with the given key, it will not
 * load a new version from the file.  You will need to delete the recorded
 * version before loading the file.
 *
 * Technically, gesture files are directories (a later refactoring should
 * convert them into custom ZIP files). The directory has an index JSON
 * mapping keys to hashes.  For each hash, there is a separate SDL file
 * with the gesture information. For this method, the gesture directory is
 * assumed to be in the asset directory.
 *
 * Because of this structure, some gestures may be read into memory even
 * if the entire read fails (because of a bad gesture).
 *
 * This method will load immediately if the device is ready.  If it is not
 * actively loading or saving a file, it will block until it can load.
 * Otherwise, it will fail immediately.  The callback function (which can
 * be nullptr) will be called with the result when it is done.
 *
 * @param file      The name of the gesture file (directory)
 * @param callback  The callback to report success or failure
 */
void GestureInput::loadAssetAsync(const std::string& file, std::function<void(bool success)> callback) {
    // Fail immediately on serialization conflict
    if (_state == GestureState::STORING || _state == GestureState::LOADING) {
        if (callback != nullptr) {
            Application::get()->schedule([=](void) {
                callback(false);
                return false;
            });
        }
        return;
    }

    if (_loader == nullptr) {
        _loader = ThreadPool::alloc(1);
    }
    
    if (ready()) {
        changeState(GestureState::LOADING);
    } else {
        _waits = GestureState::LOADING;
    }

    _loader->addTask([=](void) {
        std::unique_lock<std::mutex> block(this->_lock);

        // These are the only two states we block on
        while (this->_state != GestureState::MATCHING && this->_state != GestureState::LOADING) {
            _monitor.wait(block);
        }
        if (this->_state == GestureState::LOADING) {
            this->read(file,callback);
        }
        this->changeState(this->_waits,false);
    });
}

/**
 * Synchronously writes the current gestures to the given gesture file.
 *
 * Technically, gesture files are directories (a later refactoring should
 * convert them into custom ZIP files). The directory has an index JSON
 * mapping keys to hashes.  For each hash, there is a separate SDL file
 * with the gesture information. For this method, the gesture directory is
 * assumed to be in the application save directory.
 *
 * Because of this structure, some gestures may be written to a file even
 * if the entire write fails (because of bad access or other issues).
 *
 * This method will fail if the device is not ready or involved in any
 * blocking operations.
 *
 * @param file      The name of the gesture file (directory)
 *
 * @return true if the gestures were written successfully
 */
bool GestureInput::save(Pathname& file) {
    if (!ready()) {
        return false;
    }
    
    return store(file,nullptr);
}

/**
 * Asynchronously writes the current gestures to the given gesture file.
 *
 * Technically, gesture files are directories (a later refactoring should
 * convert them into custom ZIP files). The directory has an index JSON
 * mapping keys to hashes.  For each hash, there is a separate SDL file
 * with the gesture information. For this method, the gesture directory is
 * assumed to be in the application save directory.
 *
 * Because of this structure, some gestures may be written to a file even
 * if the entire write fails (because of bad access or other issues).
 *
 * This method will write immediately if the device is ready.  If it is not
 * actively loading or saving a file, it will block until it can write.
 * Otherwise, it will fail immediately.  The callback function (which can
 * be nullptr) will be called with the result when it is done.
 *
 * @param file      The name of the gesture file (directory)
 * @param callback  The callback to report success or failure
 */
void GestureInput::saveAsync(Pathname& file, std::function<void(bool success)> callback) {
    // Fail immediately on serialization conflict
    if (_state == GestureState::STORING || _state == GestureState::LOADING) {
        if (callback != nullptr) {
            Application::get()->schedule([=](void) {
                callback(false);
                return false;
            });
        }
        return;
    }
    
    if (_loader == nullptr) {
        _loader = ThreadPool::alloc(1);
    }
    
    if (ready()) {
        changeState(GestureState::STORING);
    } else {
        _waits = GestureState::STORING;
    }
    
    _loader->addTask([=](void) {
        std::unique_lock<std::mutex> block(this->_lock);
        
        // These are the only two states we block on
        while (this->_state != GestureState::MATCHING && this->_state != GestureState::STORING) {
            _monitor.wait(block);
        }
        if (this->_state == GestureState::STORING) {
            Pathname path = file;
            this->store(path,callback);
        }
        this->changeState(this->_waits,false);
    });
}

#pragma mark -
#pragma mark Listeners
/**
 * Requests focus for the given identifier
 *
 * Only a listener can have focus. This method returns false if the key
 * does not refer to an active listener
 *
 * @param key   The identifier for the focus object
 *
 * @return false if key does not refer to an active listener
 */
bool GestureInput::requestFocus(Uint32 key) {
    if (isListener(key)) {
        _focus = key;
        return true;
    }
    return false;
}

/**
 * Returns true if key represents a listener object
 *
 * An object is a listener if it is a listener for either a match or a
 * state change.
 *
 * @param key   The identifier for the listener
 *
 * @return true if key represents a listener object
 */
bool GestureInput::isListener(Uint32 key) const {
    bool result = _matchListeners.find(key) != _matchListeners.end();
    result = result || _stateListeners.find(key) != _stateListeners.end();
    return result;
}

/**
 * Returns the gesture match listener for the given object key
 *
 * This listener is invoked when a gesture is matched.  If threshold is
 * negative (and the device is not paused), this will be whenever a finger
 * is released.  Otherwise, it will only return a match when the best match
 * has an error value less than the tolerance.
 *
 * If there is no listener for the given key, it returns nullptr.
 *
 * @param key   The identifier for the listener
 *
 * @return the gesture match listener for the given object key
 */
const GestureListener GestureInput::getMatchListener(Uint32 key) const {
    if (_matchListeners.find(key) != _matchListeners.end()) {
        return (_matchListeners.at(key));
    }
    return nullptr;
}

/**
 * Returns the state change listener for the given object key
 *
 * This listener is invoked when the input state changes.  This includes
 * synchronous methods such as pause or resume. It is not restricted to
 * asynchronous changes.
 *
 * If there is no listener for the given key, it returns nullptr.
 *
 * @param key   The identifier for the listener
 *
 * @return the state change listener for the given object key
 */
const GestureStateListener GestureInput::getStateListener(Uint32 key) const {
    if (_stateListeners.find(key) != _stateListeners.end()) {
        return (_stateListeners.at(key));
    }
    return nullptr;
}

/**
 * Adds a gesture match listener for the given object key
 *
 * There can only be one listener for a given key.  If there is already
 * a listener for the key, the method will fail and return false.  You
 * must remove a listener before adding a new one for the same key.
 *
 * This listener is invoked when a gesture is matched.  If threshold is
 * negative (and the device is not paused), this will be whenever a finger
 * is released.  Otherwise, it will only return a match when the best match
 * has an error value less than the tolerance.
 *
 * @param key       The identifier for the listener
 * @param listener  The listener to add
 *
 * @return true if the listener was succesfully added
 */
bool GestureInput::addMatchListener(Uint32 key, GestureListener listener) {
    if (_matchListeners.find(key) == _matchListeners.end()) {
        _matchListeners[key] = listener;
        return true;
    }
    return false;
}

/**
 * Adds a state change listener for the given object key
 *
 * There can only be one listener for a given key.  If there is already
 * a listener for the key, the method will fail and return false.  You
 * must remove a listener before adding a new one for the same key.
 *
 * This listener is invoked when the input state changes.  This includes
 * synchronous methods such as pause or resume. It is not restricted to
 * asynchronous changes.
 *
 * @param key       The identifier for the listener
 * @param listener  The listener to add
 *
 * @return true if the listener was succesfully added
 */
bool GestureInput::addStateListener(Uint32 key, GestureStateListener listener) {
    if (_stateListeners.find(key) == _stateListeners.end()) {
        _stateListeners[key] = listener;
        return true;
    }
    return false;
}

/**
 * Removes the gesture match listener for the given object key
 *
 * If there is no active listener for the given key, this method fails and
 * returns false.
 *
 * This listener is invoked when a gesture is matched.  If threshold is
 * negative (and the device is not paused), this will be whenever a finger
 * is released.  Otherwise, it will only return a match when the best match
 * has an error value less than the tolerance.
 *
 * @param key   The identifier for the listener
 *
 * @return true if the listener was succesfully removed
 */
bool GestureInput::removeMatchListener(Uint32 key) {
    if (_matchListeners.find(key) != _matchListeners.end()) {
        _matchListeners.erase(key);
        return true;
    }
    return false;
}

/**
 * Removes the state change listener for the given object key
 *
 * If there is no active listener for the given key, this method fails and
 * returns false.
 *
 * This listener is invoked when the input state changes.  This includes
 * synchronous methods such as pause or resume. It is not restricted to
 * asynchronous changes.
 *
 * @param key   The identifier for the listener
 *
 * @return true if the listener was succesfully removed
 */
bool GestureInput::removeStateListener(Uint32 key) {
    if (_stateListeners.find(key) != _stateListeners.end()) {
        _stateListeners.erase(key);
        return true;
    }
    return false;
}

#pragma mark -
#pragma mark Input Device
/**
 * Clears the state of this input device, readying it for the next frame.
 *
 * Many devices keep track of what happened "this" frame.  This method is
 * necessary to advance the frame.
 */
void GestureInput::clearState() {
    _match = false;
}

/**
 * Processes an SDL_Event
 *
 * The dispatcher guarantees that an input device only receives events that
 * it subscribes to.
 *
 * @param event The input event to process
 * @param stamp The event timestamp in CUGL time
 *
 * @return false if the input indicates that the application should quit.
 */
bool GestureInput::updateState(const SDL_Event& event, const Timestamp& stamp) {
    switch (_state) {
        case GestureState::UNDEFINED:
            changeState(_waits);
            break;
        case GestureState::MATCHING:
            if (event.type == SDL_DOLLARGESTURE) {
                // May need some error checking here.
                std::string key = _gestures[event.dgesture.gestureId];

                _event.timestamp = stamp;
                _event.key = key;
                _event.fingers = event.dgesture.numFingers;
                _event.error = event.dgesture.error/DOLLARSIZE;
                
                if (_tolerance < 0 || _event.error <= _tolerance) {
                    _match = true;
                    for(auto it = _matchListeners.begin(); it != _matchListeners.end(); ++it) {
                        it->second(_event,it->first == _focus);
                    }
                }
            }
            break;
        case GestureState::RECORDING:
            if (event.type == SDL_DOLLARRECORD) {
                _gestures[event.dgesture.gestureId] = _recrd;
                _inverses[_recrd] = event.dgesture.gestureId;
                _recrd = "";
                changeState(_waits);
            }
            break;
        case GestureState::ABORTING:
            if (event.type == SDL_DOLLARRECORD) {
                SDL_RemoveDollarGesture(event.dgesture.gestureId);
                changeState(_waits);
            }
        case GestureState::LOADING:
        case GestureState::STORING:
        case GestureState::PAUSED:
            // Recognize no events during this time.
            break;
    }
    
    return true;
}

/**
 * Changes the device state to the one provided.
 *
 * This method will call any attached listeners. It guarantees that the
 * state change always happens in the primary CUGL thread.
 *
 * @param state     The new device state
 * @param thread    Whether the call is in the main thread
 */
void GestureInput::changeState(GestureState state, bool thread) {
    if (!thread) {
        Application::get()->schedule([=](void) {
            this->changeState(state,true);
            return false;
        });
        return;
    }
    
    GestureState orig = _state;
    _state = state;
    if (_waits != GestureState::MATCHING) {
        _monitor.notify_one();
    }
    _waits = GestureState::MATCHING;
    for(auto it = _stateListeners.begin(); it != _stateListeners.end(); ++it) {
        it->second(orig,state);
    }
}


/**
 * Determine the SDL events of relevance and store there types in eventset.
 *
 * An SDL_EventType is really Uint32.  This method stores the SDL event
 * types for this input device into the vector eventset, appending them
 * to the end. The Input dispatcher then uses this information to set up
 * subscriptions.
 *
 * @param eventset  The set to store the event types.
 */
void GestureInput::queryEvents(std::vector<Uint32>& eventset){
    eventset.push_back((Uint32)SDL_FINGERDOWN);
    eventset.push_back((Uint32)SDL_FINGERUP);
    eventset.push_back((Uint32)SDL_FINGERMOTION);
    eventset.push_back((Uint32)SDL_MULTIGESTURE);
    eventset.push_back((Uint32)SDL_DOLLARRECORD);
    eventset.push_back((Uint32)SDL_DOLLARGESTURE);
}


