//
//  CUGestureInput.h
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
#ifndef __CU_GESTURE_INPUT_H__
#define __CU_GESTURE_INPUT_H__
#include <cugl/input/CUInput.h>
#include <cugl/math/CUVec2.h>
#include <cugl/io/CUPathname.h>
#include <cugl/util/CUThreadPool.h>
#include <condition_variable>
#include <mutex>

namespace cugl {
    
#pragma mark -
#pragma mark GestureEvent
/**
 * This simple class is a struct to hold custom gesture event information.
 */
class GestureEvent {
public:
    /** The time of the gesture event */
    Timestamp timestamp;
    /** Identifier key for this gesture */
    std::string key;
    /** 
     * The delta between the template and the actual performed gesture. 
     *
     * Errors are normalized to be between 0 and 1, with 0 being accurate and
     * 1 being an extreme error.  However, errors greater than 1 are possible
     * because of the normalization process.
     */
    float error;
    /** The number of fingers involved in this pan */
    int fingers;
    
    /**
     * Constructs a new gesture event with the default values
     */
    GestureEvent() : fingers(0), error(0) {}
    
    /**
     * Constructs a new gesture event with the given values
     *
     * @param name      The identifying gesture key
     * @param down      The number of fingers in the gesture
     * @param delta     The delta between the template and the actual performed gesture
     * @param stamp     The timestamp for the event
     */
    GestureEvent(const std::string& name, float delta, int down, const Timestamp& stamp) {
        key = name; fingers = down; error = delta; timestamp = stamp;
    }
};


#pragma mark -
#pragma mark GestureListener

/**
 * @typedef GestureListener
 *
 * This type represents a listener for a gesture in the {@link GestureInput} class.
 *
 * In CUGL, listeners are implemented as a set of callback functions, not as
 * objects. This allows each listener to implement as much or as little
 * functionality as it wants. A listener is identified by a key which should
 * be a globally unique unsigned int.
 *
 * While gesture listeners do not traditionally require focus like a keyboard 
 * does, we have included that functionality. While only one listener can have 
 * focus at a time, all listeners will receive input from the GestureInput 
 * device.
 *
 * The function type is equivalent to
 *
 *      std::function<void(const GestureEvent& event, bool focus)>
 *
 * @param event     The touch event for this gesture event
 * @param focus     Whether the listener currently has focus
 */
typedef std::function<void(const GestureEvent& event, bool focus)> GestureListener;


#pragma mark -
#pragma mark GestureStateListener

/**
 * This enum represents the current state of the gesture input device
 *
 * Unlike most other input devices, this device can be in several different
 * states as it cycles between recording and template matching.  Gesture
 * events will only be generated during the MATCHING state
 */
enum class GestureState : int {
    /**
     * The input device is uninitialized.
     *
     * This is the state of the input device if there is no attached
     * touch device for receiving gestures. Because of an unusual design
     * decision in SDL, some touch devices are not recognized until the
     * user touches for the first time.  Hence it is possible that the
     * input device may change from this state to another state.
     */
    UNDEFINED   = 0,
    /**
     * The input device is matching gestures to templates.
     *
     * This is the default state of the device.  When it this state, the
     * device is actively matching against any defined dollar templates.
     */
    MATCHING    = 1,
    /**
     * The input device is paused.
     *
     * In this state, the input device is not matching against any dollar
     * templates.  The templates are not erased (as in the case when the
     * device is deactivated), but the matching overhead is not incurred.
     */
    PAUSED      = 2,
    /**
     * The input device is recording a gesture.
     *
     * In this state, the input device will record the next gesture on
     * the device.  This gesture will start from the next finger down
     * and last until the finger is removed.  Adding any fingers will
     * stop the recording as well.
     */
    RECORDING   = 3,
    /**
     * The input device is aborting a recorded gesture.
     *
     * If you abort a recorded gesture, you must wait until the recording
     * is complete.  This state will wait until it is complete and perform
     * any necessary clean-up.
     */
    ABORTING    = 4,
    /**
     * The input device is loading gestures from a file.
     *
     * The input device will block until the file is fully loading.
     */
    LOADING     = 5,
    /**
     * The input device is storing gestures to a file.
     *
     * The input device will block until the file has been written.
     */
    STORING     = 6
};

/**
 * @typedef GestureStateListener
 *
 * This type represents a listener for a state change in the {@link GestureInput} class.
 *
 * In CUGL, listeners are implemented as a set of callback functions, not as
 * objects. This allows each listener to implement as much or as little
 * functionality as it wants. A listener is identified by a key which should
 * be a globally unique unsigned int.
 *
 * This type of listener responds to changes in the input device state. This is
 * necessary because the GestureInput class cycles between many states and the
 * state changes (such as from recording back to input matching) are asynchronous.
 * As with all listeners, these functions guaranteed to be called at the start
 * of an animation frame, before the method {@link Application#update(float)}.
 *
 * While state listeners do not traditionally require focus like a keyboard does,
 * we have included that functionality. While only one listener can have focus
 * at a time, all listeners will receive input from the GestureInput device.
 *
 * The function type is equivalent to
 *
 *      std::function<void(GestureInput::State ostate, GestureInput::State nstate)>
 *
 * @param ostate    The old input state
 * @param nstate    The new input state
 */
typedef std::function<void(GestureState ostate, GestureState nstate)> GestureStateListener;


#pragma mark -
#pragma mark GestureInput
/**
 * This class is an input device recognizing custom gestures events.
 *
 * This input devices is an implementation of the SDL Dollar Gesture system.
 * Dollar gestures are custom gestures that consist of a single stroke and can 
 * be made anywhere on the screen. An example of a dollar gesture is the spell
 * casting system in Infinity Blade. These gestures can be performed with any 
 * number of fingers (the centroid of the fingers must follow the path of the 
 * gesture), but the number of fingers must be constant (e.g. a finger cannot 
 * go down in the middle of a gesture). The path of a gesture is considered 
 * the path from the time when the final finger went down, to the first time 
 * any finger comes up.  For more information see
 *
 *      http://depts.washington.edu/madlab/proj/dollar/index.html
 *
 * If this input device is activated, every single finger up event will trigger
 * and event to match it to a dollar template, and it will always return the
 * closest matching template, no matter how bad the error.  This is potentially
 * very noisy and heavy-weight.  Therefore, this device has the ability to 
 * pause and resume without losing state. Deactivating the device will cause
 * it to lose all recorded gestures.
 *
 * In addition, the input device has an error threshold.  If the closest 
 * matching template has an error value exceeding the threshold, it will be
 * ignored and no match will be reported.
 *
 * Dollar gestures are not built-into the system.  They must be recorded,
 * either by the player (according to an on-screen calibration prompt) or
 * the designer.  Therefore this inpout device has support for both recording
 * and pattern recognition.
 *
 * Dollar gestures can be saved to and written into the special save directory
 * ({@see Application#getSaveDirectory}). They are written into a special
 * subdirectory that contains the binary data for each gesture and an index
 * JSON mapping keys to hash IDs.  We also allow dollar gestures to be loaded 
 * as an asset.  That way the designer can record the gestures ahead of time
 * an package them as an asset.
 *
 * As with most devices, we provide support for both listeners and polling
 * the mouse. Because gestures only fire when a finger is released, this 
 * input device is much better designed for listeners over polling.  The
 * events are queued and processed at the start of the animation frame, before 
 * the method {@link Application#update(float)} is called.
 *
 * IMPORTANT: This input device is VERY thread unsafe.  No methods in this
 * class should ever be called outside the main CUGL thread.
 *
 * In addition, because of the way SDL is designed, this device may not support 
 * recording or file loading immediately at the start of the application. On 
 * some platforms, touch devices are not recognized until they are touched for
 * the first time.  On these platforms, the state will start out as UNDEFINED
 * instead of MATCHING.  See {@link getState()} and {@link ready()} for more
 * information.
 */
class GestureInput : public InputDevice {
protected:
    /** The current state of this input device */
    GestureState _state;
    /** The queued next state of this input device */
    GestureState _waits;
    /** The allowed error tolerance for matching gestures */
    float _tolerance;
 
    /** The map from gesture keys to SDL identifiers */
    std::unordered_map<SDL_GestureID, std::string> _gestures;
    /** The map from SDL identifiers to gesture keys */
    std::unordered_map<std::string, SDL_GestureID> _inverses;
    
    /** The name of the gesture currently being recorded */
    std::string  _recrd;
    /** Whether or not we made a match this animation frame (for polling) */
    bool  _match;
    /** The currently matched gesture event */
    GestureEvent _event;
    
    /** The thread pool for asynchronous loading */
    std::shared_ptr<ThreadPool> _loader;
    /** A mutex for thread management */
    std::mutex _lock;
    /** A conditional variable for thread management */
    std::condition_variable _monitor;
    
    /** The set of listeners called whenever a gesture is recognized */
    std::unordered_map<Uint32, GestureListener> _matchListeners;
    /** The set of listeners called whenever a state changes */
    std::unordered_map<Uint32, GestureStateListener> _stateListeners;
    
    /**
     * Changes the device state to the one provided.
     *
     * This method will call any attached listeners. It guarantees that the
     * state change always happens in the primary CUGL thread.
     *
     * @param state     The new device state
     * @param thread    Whether the call is in the main thread
     */
    void changeState(GestureState state, bool thread=true);
    
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
    bool read(const std::string& file, std::function<void(bool success)> callback=nullptr);
    
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
    bool store(Pathname& file, std::function<void(bool success)> callback=nullptr);

    
#pragma mark Constructor
    /**
     * Creates and initializes a new gesture input device.
     *
     * WARNING: Never allocate a gesture input device directly.  Always use the
     * {@link Input#activate()} method instead.
     */
    GestureInput();
    
    /**
     * Deletes this input device, disposing of all resources
     */
    virtual ~GestureInput() { dispose(); }
    
    /**
     * Unintializes this device, returning it to its default state
     *
     * An uninitialized device may not work without reinitialization.
     */
    virtual void dispose() override;
    
    
#pragma mark Device State
public:
    /**
     * Returns true if the input device is actively matching templates.
     *
     * The matching state is the only non-blocking state of this devices.
     * You should make sure the device is ready before performing any 
     * synchronous operations.
     *
     * @return true if the input device is actively matching templates.
     */
    bool ready() const { return _state == GestureState::MATCHING; }
    
    /**
     * Returns the current state of this input device.
     *
     * @return the current state of this input device.
     */
    GestureState getState() const { return _state; }

    /**
     * Returns the error tolerance for this input device 
     *
     * If tolerance is negative, the input device will always return a match
     * on a finger up, no matter how bad the error.  Otherwise, it will only
     * return matches whose error is less than the tolerance.
     *
     * Errors are normalized to be between 0 and 1, with 0 being accurate and
     * 1 being an extreme error.  However, errors greater than 1 are possible
     * because of the normalization process.
     *
     * @return the error tolerance for this input device
     */
    float getTolerance() const { return _tolerance; }

    /**
     * Sets the error tolerance for this input device
     *
     * If tolerance is negative, the input device will always return a match
     * on a finger up, no matter how bad the error.  Otherwise, it will only
     * return matches whose error is less than the tolerance.
     *
     * Errors are normalized to be between 0 and 1, with 0 being accurate and
     * 1 being an extreme error.  However, errors greater than 1 are possible
     * because of the normalization process.
     *
     * @param tolerance The error tolerance for this input device
     */
    void setTolerance(float tolerance) {
        _tolerance = tolerance;
    }
    
    /**
     * Returns the list of gestures currently recorded 
     *
     * @return the list of gestures currently recorded
     */
    std::vector<std::string> getGestures() const;
    
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
    bool pause();

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
    bool resume();

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
    bool record(const std::string& key);

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
    bool record(const char* key) {
        return record(std::string(key));
    }

    /**
     * Abort a gesture currently being recorded.
     *
     * This method only succeeds if the device is currently in the recording
     * state.  It will delete the gesture on completion.
     *
     * @return true if the recording was successfully aborted
     */
    bool abort();
    
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
    bool remove(const std::string& key);

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
    bool remove(const char* key) {
        return remove(std::string(key));
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
    bool rename(const std::string& key, const std::string& name);

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
    bool rename(const char* key, const std::string& name) {
        return rename(std::string(key),name);
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
    bool rename(const std::string& key, const char* name) {
        return rename(key,std::string(name));
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
    bool rename(const char* key, const char* name) {
        return rename(std::string(key),std::string(name));
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
    bool load(const std::string& file) {
        return load(Pathname(file));
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
    bool load(const char* file) {
        return load(Pathname(file));
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
    bool load(const Pathname& file);

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
    void loadAsync(const std::string& file, std::function<void(bool success)> callback) {
        loadAsync(Pathname(file),callback);
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
    void loadAsync(const char* file, std::function<void(bool success)> callback) {
        loadAsync(Pathname(file),callback);
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
    void loadAsync(const Pathname& file, std::function<void(bool success)> callback);

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
    bool loadAsset(const std::string& file);

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
    bool loadAsset(const char* file) {
        return loadAsset(std::string(file));
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
    void loadAssetAsync(const std::string& file, std::function<void(bool success)> callback);

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
    void loadAssetAsync(const char* file, std::function<void(bool success)> callback) {
        loadAssetAsync(std::string(file),callback);
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
    bool save(const std::string& file) {
        Pathname path(file);
        return store(path);
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
    bool save(const char* file) {
        Pathname path(file);
        return store(path);
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
    bool save(Pathname& file);

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
    void saveAsync(const std::string& file, std::function<void(bool success)> callback) {
        Pathname path(file);
        saveAsync(path, callback);
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
    void saveAsync(const char* file, std::function<void(bool success)> callback) {
        Pathname path(file);
        saveAsync(path, callback);
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
    void saveAsync(Pathname& file, std::function<void(bool success)> callback);

    
#pragma mark Data Polling
    /**
     * Returns true if the device recognized a gesture this animation frame.
     *
     * If the device is did not recognize a gesture this frame, all other 
     * polling methods will return the default value.
     *
     * @return true if the device recognized a gesture this animation frame.
     */
    bool didMatch() const { return _match; }
    
    /**
     * Returns the key of the currently matching gesture.
     *
     * This method returns the empty string if there is no matching gesture.
     *
     * @return the key of the currently matching gesture.
     */
    const std::string getKey() const { return _match ? _event.key : ""; }
    
    /**
     * Returns the error value between the gesture and its best match
     *
     * This value returns a negative value if there is no matching gesture.
     *
     * Errors are normalized to be between 0 and 1, with 0 being accurate and
     * 1 being an extreme error.  However, errors greater than 1 are possible
     * because of the normalization process.
     *
     * @return the error value between the gesture and its best match
     */
    const float getError() const  { return _match ? _event.error : -1.0f; }

    /**
     * Returns the number of fingers involved in the current gesture.
     *
     * This value is 0 if there are no matching gesture.
     *
     * @return the number of fingers involved in the current gesture.
     */
    int getFingers() const { return _match ? _event.fingers : 0; }
    
    
#pragma mark Listeners
    /**
     * Requests focus for the given identifier
     *
     * Only a listener can have focus.  This method returns false if the key
     * does not refer to an active listener
     *
     * @param key   The identifier for the focus object
     *
     * @return false if key does not refer to an active listener
     */
    virtual bool requestFocus(Uint32 key) override;
    
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
    bool isListener(Uint32 key) const;
    
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
    const GestureListener getMatchListener(Uint32 key) const;

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
    const GestureStateListener getStateListener(Uint32 key) const;

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
    bool addMatchListener(Uint32 key, GestureListener listener);

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
    bool addStateListener(Uint32 key, GestureStateListener listener);

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
    bool removeMatchListener(Uint32 key);
    
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
    bool removeStateListener(Uint32 key);
    
    
protected:
#pragma mark Input Device
    /**
     * Clears the state of this input device, readying it for the next frame.
     *
     * Many devices keep track of what happened "this" frame.  This method is
     * necessary to advance the frame.
     */
    virtual void clearState() override;
    
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
    virtual bool updateState(const SDL_Event& event, const Timestamp& stamp) override;
    
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
    virtual void queryEvents(std::vector<Uint32>& eventset) override;
    
    // Apparently friends are not inherited
    friend class Input;
};


}
#endif /* __CU_GESTURE_INPUT_H__ */
