//
//  CUAudioEngine-Apple.h
//  Cornell University Game Library (CUGL)
//
//  This module provides AVFoundation support for AudioEngine. This solution
//  only works on iOS and OS X platforms. It is the prefered solution for those
//  platforms.  You can set it by defining/undefining the CU_AUDIO_AVFOUNDATION
//  compiler variable.
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
#import <AVFoundation/AVFoundation.h>
#import "AVOggAudioFile.h"
#import "AVAudioObserver.h"

#include "CUAudioEngine-impl.h"
#include <cugl/base/CUApplication.h>
#include <cugl/util/CUThreadPool.h>
#include <cugl/audio/CUMusic.h>
#include <cugl/util/CUDebug.h>
#include <algorithm>
#include <string>
#include <mutex>

/** The buffer size for streaming audio files */
#define BUFFER_SIZE 4096

/** This is a work around to handle a problem in iOS 11 */
#define FAN_OUT 8

namespace cugl {
namespace impl {
    
#pragma mark -
#pragma mark Basic Data Types
/**
 * Reference to the AVFoundation audio format.
 *
 * This is a C++ wrapper for AVAudioFile and AVAudioPCMBuffer. It is necessary
 * for a PIMPL implementation of sound assets.  It is just POD; there is no
 * associated constructor.
 */
struct AudioBuffer {
    AVAudioPCMBuffer* pcmb;  // The uncompressed source data
};

/**
 * Reference to the AVFoundation music format.
 *
 * This is a C++ wrapper for AVAudioFile. It is necessary for a PIMPL 
 * implementation of sound assets.  It is just POD; there is no associated 
 * constructor.
 */
struct AudioStream {
    id<AVAudioFileSource> file;  // File reference for AVAudioEngine.
    cugl::Music::Type  type;  // File type
};

/**
 * Reference to the AVFoundation implementation of a sound channel
 *
 * This is a C++ wrapper for AVPlayerNode. It is necessary for a PIMPL
 * implementation of a sound channel.  It is just POD; there is no associated
 * constructor.
 *
 * You will notice that this struct has a lot of attached values. First of 
 * all, the player must track its currently attached asset.  The other data
 * is because of a peculiarity in AVAudioPlayer.  It will invoke a completion 
 * callback even if the sound is stopped manually. And to make matters worse, 
 * it does not invoke the callbacks immediately on stop (it calls it in the
 * audio thread).  We need a way to ignore callbacks if we manually stop a 
 * sound. This is the purpose of the audio timestamps.
 */
struct AudioChannel {
    /** The id for this player channel */
    Uint32 channel;
    /** The player node for AVAudioEngine */
    AVAudioPlayerNode* node;
    /** The currently playing buffer (the original, not a snippet) */
    AVAudioPCMBuffer* pcmb;
    /** The sound format for the mixer graph */
    AVAudioFormat* format;
    /** The unique timestamp to mark safe deletions */
    Uint64 timeStamp;
    /** The starting frame for the pcmb buffer snippet */
    Uint32 startFrame;
    /** Whether or not we are currently in a loop */
    bool looping;
    /** Whether or not the channel is active playing */
    bool playing;
};

/**
 * Reference to the AVFoundation implementation of a streaming player
 *
 * This is a C++ wrapper for AVPlayerNode. It is necessary for a PIMPL
 * implementation of a music player.  It is just POD; there is no associated
 * constructor.
 *
 * You will notice that this struct has a lot of attached values. First of
 * all, the player must track its currently attached asset.  The other data
 * is because of a peculiarity in AVAudioPlayer.  It will invoke a completion
 * callback even if the sound is stopped manually. And to make matters worse,
 * it does not invoke the callbacks immediately on stop (it calls it in the
 * audio thread).  We need a way to ignore callbacks if we manually stop a
 * sound. This is the purpose of the audio timestamps.
 */
struct AudioPlayer {
    /** The player node for AVAudioEngine */
    AVAudioPlayerNode* node;
    /** The currently playing music file */
    id<AVAudioFileSource> file;
    /** The unique timestamp to mark safe deletions */
    Uint64 timeStamp;
    /** The number of audio frames read previously */
    Uint64 readFrame;
    /** The final frame of the first buffer being read */
    Uint64 firstOffset;
    /** The final frame of the second buffer being read */
    Uint64 secndOffset;
    /** Whether or not we are currently in a loop */
    bool looping;
    /** Whether or not the player is active playing */
    bool playing;
    /** The current (true) volume of the player */
    float volume;
    /** The number of audio frames in the audio file to fade */
    Uint64 fadeLength;
    /** The current number of frames faded so far */
    Uint64 fadePosition;
    /** The volume at the start of the fade */
    float  fadeVolume;
    /** The volume at the end of the fade */
    float  goalVolume;
    /** A mutex lock for thread safety */
    std::mutex lock;
};

/**
 * Reference to the AVFoundation audio engine
 *
 * This is a C++ wrapper for AVAudioEngine. It is necessary for a PIMPL
 * implementation of the entire audio system.  It contains the mixer graph
 * and a thread for handling stream processing.  It is just POD; there is no 
 * associated constructor.
 */
struct AudioMixer {
    AVAudioEngine* mixer;
    AVAudioMixerNode** submixers;  // iOS 11 WORKAROUND
    AVAudioObserver* observer;
    AudioPlayer* background;
    std::vector<AudioChannel*> channels;
    std::shared_ptr<cugl::ThreadPool> processor;
};
    
/** The pointer to the engine root */
static AudioMixer* _engine;

#pragma mark -
#pragma mark Audio Engine
/**
 * Builds the submixers necessary to handle the fan out problem.
 *
 * In the fan out problem, a Mixer node has a maximum of FAN_OUT input
 * channels.  This appeared in iOS 11 for the first time.
 *
 * @param input The number of sound effect channels
 */
void InternalBuildFanOut(int input) {
#if FAN_OUT <= 0
    _engine->submixers = nullptr;
#else
    CUAssertLog(input+1 < FAN_OUT*FAN_OUT, "Due to a bug in iOS 11, we currently only support %d channels",
                FAN_OUT*FAN_OUT);
    int total = input+1;
    int nodes = (int)std::ceil(total/(float)FAN_OUT);
    
    _engine->submixers = (AVAudioMixerNode**)malloc(sizeof(AVAudioMixerNode*)*(nodes));
    for(int ii = 0; ii < nodes; ii++) {
        _engine->submixers[ii] = [[AVAudioMixerNode alloc] init];
        [_engine->mixer attachNode:_engine->submixers[ii]];
        [_engine->mixer connect:_engine->submixers[ii] to:_engine->mixer.mainMixerNode format:nil];
    }
#endif
}
    
/**
 * Builds the sound effect channels for the audio engine
 *
 * The channels are attached to the mixer graph with a volume of 0.0
 *
 * @param input The number of sound effect channels
 */
void InternalBuildChannels(int input) {
    CUAssertLog(input < 64, "A bug with iOS 11 prevents us from supporting more than 64 channels");
    _engine->channels.resize(input,nullptr);
    
    for(int ii = 0; ii < input; ii++) {
        // Make the player
        AudioChannel* channel = new AudioChannel();

        // Allocate. This retains a copy
        channel->node = [[AVAudioPlayerNode alloc] init];
        channel->node.volume = 0.0f;
        channel->format = nil;
        channel->channel = ii;
        channel->looping = false;
        channel->timeStamp = 0;
        
        // Add it to the mixer graph
        // No format for now.  May change format later.
        [_engine->mixer attachNode:channel->node];
#if FAN_OUT > 0
        AVAudioMixerNode* parent = _engine->submixers[1+ii/FAN_OUT];
#else
        AVAudioMixerNode* parent = _engine->mixer.mainMixerNode;
#endif
        [_engine->mixer connect:channel->node to:parent format:nil];
        _engine->channels[ii] = channel;
    }
}

/**
 * Builds the background music player.
 *
 * The player is attached to the mixer graph with a volume of 0.0
 */
void InternalBuildBackground() {
    AudioPlayer* player = new AudioPlayer();
    player->node = [[AVAudioPlayerNode alloc] init];
    player->node.volume = 0.0f;
    player->looping = false;
    player->playing = false;
    player->timeStamp = 0;
    player->readFrame = 0;
    player->firstOffset  = 0;
    player->secndOffset  = 0;
    player->fadeLength   = 0;
    
    // Add it to the mixer graph
    // No format for now.  May change format later.
    [_engine->mixer attachNode:player->node];
#if FAN_OUT > 0
    AVAudioMixerNode* parent = _engine->submixers[0];
#else
    AVAudioMixerNode* parent = _engine->mixer.mainMixerNode;
#endif
    [_engine->mixer connect:player->node to:parent format:nil];
    _engine->background = player;
}
    
/**
 * Deletes the submixers necessary to handle the fan out problem.
 *
 * This function detaches the delegate mixers from the mixer graph and frees
 * the allocated data.
 *
 * @param input The number of sound effect channels
 */
void InternalClearFanOut(int size) {
#if FAN_OUT <= 0
    _engine->submixers = nullptr;
#else
    int nodes = (int)std::ceil(size/(float)FAN_OUT);
    for(int ii = 0; ii < nodes; ii++) {
        [_engine->mixer detachNode:_engine->submixers[ii]];
        [_engine->submixers[ii] release];
        _engine->submixers[ii] = nil;
    }
    free(_engine->submixers);
    _engine->submixers = nullptr;
#endif
    }
    

/**
 * Deletes the sound effect channels for audio engine.
 *
 * This function detaches the channels from the mixer graph and frees the
 * allocated data.
 */
int InternalClearChannels() {
    for(auto it = _engine->channels.begin(); it != _engine->channels.end(); ++it) {
        [_engine->mixer detachNode:(*it)->node];
        [(*it)->node release];
        [(*it)->pcmb release];
        [(*it)->format release];
        delete *it;
        *it = nullptr;
    }
    int result = (int)_engine->channels.size()+1;
    _engine->channels.clear();
    return result;
}

/**
 * Deletes the background music player for audio engine.
 *
 * This function detaches the player from the mixer graph and frees the
 * allocated data.
 */
void InternalClearBackground() {
    if (_engine->background) {
        AudioPlayer* player = _engine->background;
        [_engine->mixer detachNode:player->node];
        [player->node release];
        [player->file release];
        delete player;
        _engine->background = nullptr;
    }
}

/**
 * Initializes the audio engine for use.
 *
 * If you are using sound, this function should be one of the very first
 * things your application should call.  You cannot load sound or music
 * assets until this function is called. Once the audio engine is started,
 * it will continue running until stopped. It should be stopped on
 * application shutdown to prevent memory leaks.
 *
 * The audio engine should be defined with a default sampling frequency.
 * This is the ideal sampling frequency for sound and music assets. It is
 * not a good idea to the use assets with a different sampling frequency.
 *
 * While the sound engine can specify the number of output channels, the
 * only cross-platforms options are 1 (Mono) and 2 (Stereo). Cross-platform
 * 5.1 or 7.1 sound is not supported.
 *
 * @param frequency The default sampling frequency
 * @param input     The number of sound effect channels
 * @param output    The number of output channels
 */
bool AudioStart(int frequency, int input, int output) {
    CUAssertLog(!_engine, "Audio engine has already been started");
    CUAssertLog(input < 64, "A bug with iOS 11 prevents us from supporting more than 64 channels");

    _engine = new AudioMixer();
    _engine->mixer = [[AVAudioEngine alloc] init];
    if (_engine->mixer != nil) {
        NSError* error = nil;
        
        InternalBuildFanOut(input);
        InternalBuildChannels(input);
        InternalBuildBackground();
        
        [_engine->mixer startAndReturnError:&error];
        if (error != nil) {
            const char* message = [[error localizedDescription] UTF8String];
            CULogError("Failed to initialize sound engine: %s", message);
            AudioStop();
            return false;
        }
        
        _engine->observer = [[AVAudioObserver alloc] init:_engine->mixer];
        [[NSNotificationCenter defaultCenter] addObserver: _engine->observer
                                                 selector:@selector(handleInterruption:)
                                                     name:AVAudioEngineConfigurationChangeNotification
                                                   object:_engine->mixer];
        
        // Create the stream processing thread
        _engine->processor = cugl::ThreadPool::alloc(1);
        return true;
    }
    
    delete _engine;
    return false;
}

/**
 * Stops the audio engine, preventing it from further use.
 *
 * This should be the very last thing called at application shutdown.  All
 * sound and music assets should be unloaded before this function is called.
 */
void AudioStop() {
    if (_engine) {
        [_engine->mixer stop];
        int input = (int)_engine->channels.size();
        input = 1+input/8;
        
        InternalClearBackground();
        int amt = InternalClearChannels();
        InternalClearFanOut(amt);
        
        [_engine->mixer release];
        [_engine->observer release];
        _engine->processor = nullptr;
        delete _engine;
        _engine = nullptr;
    }
}


#pragma mark -
#pragma mark Sound Assets
/**
 * Returns an AVAudioPCMBuffer that is a shorter snippet of the given one
 *
 * A snippet is a subsection of an audio buffer.  It may start after the 
 * beginning and end before the end.  It is no unlike a "substring" of audio.
 * The new buffer is a completely different object in memory.  We need to do
 * this whenever we want to fast-forward or rewind and in-memory sound asset.
 *
 * The returned object is automatically autoreleased.  However, this requires
 * an explicit autorelease block to be effective in Objective-C++.
 *
 * @param buffer    The buffer to cut from
 * @param start     The starting audio frame of the snippet
 * @param length    The length (in audio frames) of the snippet
 *
 * @return an ACAudioPCMBuffer that is a shorter snippet of the given one
 */
AVAudioPCMBuffer* InternalAllocPCMSnippet(AVAudioPCMBuffer* buffer,
                                          AVAudioFramePosition start,
                                          AVAudioFrameCount length) {
    AVAudioFormat* format = buffer.format;
    AVAudioPCMBuffer* snippet = [[AVAudioPCMBuffer alloc] initWithPCMFormat:format
                                                              frameCapacity:length];
    // It would be nice if there was a cleaner way
    // But these properties do not play nice with pointers
    switch(format.commonFormat) {
        case AVAudioPCMFormatFloat32:
            if (format.interleaved) {
                size_t amount = length * format.streamDescription->mBytesPerFrame*format.channelCount;
                memcpy(snippet.floatChannelData[0], &(buffer.floatChannelData[0][start]), amount);
            } else {
                for (AVAudioChannelCount ch = 0; ch < format.channelCount; ++ch) {
                    size_t amount = length * format.streamDescription->mBytesPerFrame;
                    memcpy(snippet.floatChannelData[ch], &(buffer.floatChannelData[ch][start]), amount);
                }
            }
            break;
        case AVAudioPCMFormatInt16:
            if (format.interleaved) {
                size_t amount = length * format.streamDescription->mBytesPerFrame*format.channelCount;
                memcpy(snippet.int16ChannelData[0], &(buffer.int16ChannelData[0][start]), amount);
            } else {
                for (AVAudioChannelCount ch = 0; ch < format.channelCount; ++ch) {
                    size_t amount = length * format.streamDescription->mBytesPerFrame;
                    memcpy(snippet.int16ChannelData[ch], &(buffer.int16ChannelData[ch][start]), amount);
                }
            }
            break;
        case AVAudioPCMFormatInt32:
            if (format.interleaved) {
                size_t amount = length * format.streamDescription->mBytesPerFrame*format.channelCount;
                memcpy(snippet.int32ChannelData[0], &(buffer.int32ChannelData[0][start]), amount);
            } else {
                for (AVAudioChannelCount ch = 0; ch < format.channelCount; ++ch) {
                    size_t amount = length * format.streamDescription->mBytesPerFrame;
                    memcpy(snippet.int32ChannelData[ch], &(buffer.int32ChannelData[ch][start]), amount);
                }
            }
            break;
        case AVAudioPCMFormatFloat64:
        case AVAudioOtherFormat:
            // Not supported
            [snippet release];
            return nullptr;
    }
    
    snippet.frameLength = length;
    return [snippet autorelease];
}

/**
 * Returns an in-memory PCM buffer for the given audio asset
 *
 * This function will attempt to read the sound asset file.  If file is
 * a relative path, it will search in the asset directory.  Otherwise, it
 * will use the full path specified.
 *
 * The success of this function may depend on the platform.  Only WAV,
 * MP3, and OGG Vorbis files are cross-platform. Everything else (AAC,
 * M4A, FLAC) is platform-dependent.  If the function cannot decode the
 * file, it will return nullptr.
 *
 * @param file  The path (absolute or relative) for the sound asset
 *
 * @return an in-memory PCM buffer for the given audio asset
 */
AudioBuffer* AudioLoadBuffer(const char* file) {
    CUAssertLog(file, "No audio file specified");
    @autoreleasepool { // Always do this in Objective-C++ if in doubt
        NSURL* url;
        if (file[0] == '/') {
            url = [NSURL fileURLWithPath: [NSString stringWithUTF8String:file]];
        } else {
            NSString* path = [NSString stringWithUTF8String:cugl::Application::get()->getAssetDirectory().c_str()];
            NSString* rsrc = [path stringByAppendingPathComponent:[NSString stringWithUTF8String:file]];
            url = [NSURL fileURLWithPath:rsrc];
        }
    
        // Read the file and process any errors
        NSError* error = nil;
        id<AVAudioFileSource> avfile = nil;
    
        // OGG is the only non-Apple file we support
        // Because their decoder API is not a mess.
        if ([[url pathExtension] isEqualToString:@"ogg"]) {
            avfile = [[AVOggAudioFile alloc] initForReading:url error:&error];
        } else {
            avfile = [[AVAudioFile alloc] initForReading:url error:&error];
        }
    
        if (error != nil) {
            CULogError("Failed to access audio file %s: %s", file, [[error localizedDescription] UTF8String]);
            return nullptr;
        }

        // Allocate the buffer
        AVAudioPCMBuffer* buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:avfile.processingFormat
                                                                 frameCapacity:(AVAudioFrameCount)avfile.length];
        [avfile readIntoBuffer:buffer error:&error];
        [avfile release];

        if (error != nil) {
            CULogError("Failed to decode audio file %s: %s", file, [[error localizedDescription] UTF8String]);
            [buffer release];
            return nullptr;
        }

        AudioBuffer* result = new AudioBuffer();
        result->pcmb = buffer;
        return result;
    }
}

/**
 * Frees the given PCM buffer, releasing all resources
 *
 * @param source    The PCM buffer to free
 */
void AudioFreeBuffer(AudioBuffer* source) {
    if (source) {
        [source->pcmb release];
        delete source;
    }
}

/**
 * Returns the number of audio frames for the given PCM buffer
 *
 * @param source    The PCM buffer
 *
 * @return the number of audio frames for the given PCM buffer
 */
Uint64 AudioGetBufferFrames(AudioBuffer* source) {
    return (Uint64)source->pcmb.frameLength;
}

/**
 * Returns the number of audio channels for the given PCM buffer
 *
 * @param source    The PCM buffer
 *
 * @return the number of audio channels for the given PCM buffer
 */
Uint32 AudioGetBufferChannels(AudioBuffer* source) {
    return (Uint32)source->pcmb.format.channelCount;
}

/**
 * Returns the number of sample rate (in HZ) for the given PCM buffer
 *
 * @param source    The PCM buffer
 *
 * @return the number of sample rate (in HZ) for the given PCM buffer
 */
double AudioGetBufferSampleRate(AudioBuffer* source) {
    return source->pcmb.format.sampleRate;
}

#pragma mark -
#pragma mark Music Assets
/**
 * Returns an audio stream for the given music asset
 *
 * This function will attempt to decode the music asset file.  If file is
 * a relative path, it will search in the asset directory.  Otherwise, it
 * will use the full path specified.
 *
 * The success of this function may depend on the platform.  Only WAV,
 * MP3, and OGG Vorbis files are cross-platform. Everything else (AAC,
 * M4A, FLAC) is platform-dependent.  If the function cannot decode the
 * file, it will return nullptr.
 *
 * @param file  The path (absolute or relative) for the sound asset
 *
 * @return an audio stream for the given music asset
 */
AudioStream* AudioLoadStream(const char* file) {
    CUAssertLog(file, "No music file specified");
    @autoreleasepool { // Always do this in Objective-C++ if in doubt
        NSURL* url;
        if (file[0] == '/') {
            url = [NSURL fileURLWithPath: [NSString stringWithUTF8String:file]];
        } else {
            NSString* tempPath = [NSString stringWithUTF8String:cugl::Application::get()->getAssetDirectory().c_str()];
            NSString* tempFile = [tempPath stringByAppendingPathComponent:[NSString stringWithUTF8String:file]];
            url = [NSURL fileURLWithPath: tempFile];
        }
    
        // Figure out what type of file we are
        NSString* exten = [url pathExtension];
    
        // Read the file and process any errors
        NSError* error = nil;
        id<AVAudioFileSource> source = nil;
        if ([exten isEqualToString:@"ogg"]) {
            source = [[AVOggAudioFile alloc] initForReading:url error:&error];
        } else {
            source = [[AVAudioFile alloc] initForReading:url error:&error];
        }
    
        if (error != nil) {
            CULogError("Failed to load music file %s: %s", file, [[error localizedDescription] UTF8String]);
            return nullptr;
        }
    
        AudioStream* buffer = new AudioStream();
        buffer->file = source;

        if ([exten isEqualToString:@"mp3"]) {
            buffer->type = cugl::Music::Type::MP3;
        } else if ([exten isEqualToString:@"ogg"]) {
            buffer->type = cugl::Music::Type::OGG;
        } else if ([exten isEqualToString:@"aac"]) {
            buffer->type = cugl::Music::Type::AAC;
        } else if ([exten isEqualToString:@"m4a"]) {
            buffer->type = cugl::Music::Type::M4A;
        } else if ([exten isEqualToString:@"wav"]) {
            buffer->type = cugl::Music::Type::WAV;
        } else if ([exten isEqualToString:@"aiff"]) {
            buffer->type = cugl::Music::Type::WAV;
        } else if ([exten isEqualToString:@"aif"]) {
            buffer->type = cugl::Music::Type::WAV;
        } else {
            buffer->type = cugl::Music::Type::UNSUPPORTED;
        }

        // Create objective C wrapper
        return buffer;
    }
}

/**
 * Frees the given audio stream, releasing all resources
 *
 * @param source    The audio stream to free
 */
void AudioFreeStream(AudioStream* source) {
    if (source) {
        [source->file release];
        delete source;
    }
}

/**
 * Returns the duration of the music asset in seconds
 *
 * This value is computed from the sample rate which is deliberately
 * hidden from the user, as that is not important for tream assets.
 *
 * @param source    The audio stream
 *
 * @return the duration of the music asset in seconds
 */
double AudioGetStreamDuration(AudioStream* source) {
    return source->file.length/source->file.fileFormat.sampleRate;
}

/**
 * Returns the music type of this audio stream
 *
 * @param source    The audio stream
 *
 * @return the music type of this audio stream
 */
cugl::Music::Type AudioGetStreamType(AudioStream* source) {
    return source->type;
}

    
#pragma mark -
#pragma mark Sound Channels

/**
 * Sends the AVAudioPCMBuffer to the given player immediately
 * 
 * The asset will play as soon as it is able to.  If loop is true, it will
 * play indefinitely until halted explicilty.  When the audio halts it
 * will call the gcEffect() method in AudioEngine.
 *
 * If the asset is set to loop, it will loop back to the beginning, not
 * the beginning audio frame.
 *
 * @param player    The sound channel
 * @param buffer    The PCM buffer
 * @param loop      Whether to loop the given asset
 * @param start     The audio frame to start playback
 */
void InternalSchedulePCMBuffer(AudioChannel* player, AVAudioPCMBuffer* buffer, bool loop, Uint32 start) {
    player->timeStamp++;
    
    // Capture the current timestamp by value
    Uint64 stamp = player->timeStamp;
    
    // Objective-C style closure
    id callback = ^( void ) {
        if (player->timeStamp == stamp) {
            player->playing = false;
            cugl::AudioEngine::get()->gcEffect(player->channel,true);
        }
    };
    
    if (start > 0) {
        @autoreleasepool {
            AVAudioFramePosition position = start;
            AVAudioFrameCount length = (AVAudioFrameCount)(buffer.frameLength - position);
            AVAudioPCMBuffer* snippet = InternalAllocPCMSnippet(buffer,position,length);
        
            [player->node scheduleBuffer:snippet
                                  atTime:nil
                                 options:AVAudioPlayerNodeBufferInterrupts
                       completionHandler:(loop ? nil : callback)];
        }
    }
    if (loop) {
        [player->node scheduleBuffer:buffer
                              atTime:nil
                             options:AVAudioPlayerNodeBufferLoops
                   completionHandler:callback];
    } else if (start == 0) {
        [player->node scheduleBuffer:buffer
                              atTime:nil
                             options:AVAudioPlayerNodeBufferInterrupts
                   completionHandler:callback];
    }
    
    player->pcmb = buffer;
    player->looping = loop;
    player->startFrame = start;
}

/**
 * Returns a sound channel allocated for use with the audio engine
 *
 * The audio engine must be initialized for this function to succeed.  In
 * addtion, channel must be non-negative and less than the number of input
 * channels of the audio engine
 *
 * If the function cannot allocate a channel, it returns nullptr.  If this
 * channel has already been allocated, this returns a reference to the
 * previously allocated channel.  As a result, it is a bad idea to reallocate
 * a channel with the same id.
 *
 * @param channel   The id of the sound channel to allocate
 *
 * @return a sound channel allocated for use with the audio engine
 */
AudioChannel* AudioAllocChannel(int channel) {
    CUAssertLog(channel >= 0 && channel < _engine->channels.size(),
                "Channel %d is not a valid channel",channel);
    return _engine->channels[channel];
}

/**
 * Frees the sound channel, releasing all resources
 *
 * Once this method is called, it is unsafe to play audio on this channel
 * any longer.
 *
 * @param channel   The sound channel to free
 */
void AudioFreeChannel(AudioChannel* player) {
    CUAssertLog(!player->playing, "Attempt to free a playing channel");
    if (player->pcmb) {
        [player->pcmb release];
        player->pcmb = nullptr;
    }
}
    
/**
 * Plays the sound asset on the given channel
 *
 * The sound will play until it is finished.  If loop is true, it will
 * play indefinitely until halted explicilty.  When the audio halts it
 * will call the gcEffect() method in AudioEngine.
 *
 * If the asset is set to loop, it will loop back to the beginning, not
 * the beginning audio frame.
 *
 * @param player    The sound channel
 * @param source    The (PCM) sound asset
 * @param loop      Whether to loop the given asset
 * @param start     The audio frame to start playback
 */
void AudioPlayChannel(AudioChannel* player, AudioBuffer* source, bool loop, Uint32 start) {
    if (player->node.playing) {
        [player->node stop];
    }
    if (player->pcmb) {
        [player->pcmb release];
        player->pcmb = nullptr;
    }
    
    // Previously we reconfigured if there was a format change
    // Now we just let it fail
    
    [player->node stop];
    InternalSchedulePCMBuffer(player,source->pcmb,loop,start);
    [player->pcmb retain];
    [player->node play];
    player->playing = true;
}

/**
 * Halts the sound channel, garbage collecting the attached sound asset.
 *
 * This function will call the gcEffect() method in AudioEngine.
 *
 * @param player    The sound channel
 */
void AudioHaltChannel(AudioChannel* player) {
    player->timeStamp++;
    player->playing = false;
    [player->node stop];
    
    // We call the garbage collector ourselves. No callback
    cugl::AudioEngine::get()->gcEffect(player->channel,false);
}

/**
 * Halts the sound channel after the given number of milliseconds.
 *
 * Once this sound is complete, this function will call the gcEffect()
 * method in AudioEngine.
 *
 * @param player    The sound channel
 * @param millis    The number of millisecond before halting the asset
 */
void AudioExpireChannel(AudioChannel* player, Uint32 millis) {
    // Capture the current timestamp by value
    Uint64 stamp = player->timeStamp;
    
    cugl::Application::get()->schedule([=] {
        if (player->timeStamp == stamp) {
            AudioHaltChannel(player);
        }
        return false;
    }, millis);
}

/**
 * Pauses the sound channel
 *
 * The sound asset remains attached to the channel and will continue to
 * play once it resumes.  If this channel is already paused, this function
 * does nothing.
 *
 * @param player    The sound channel
 */
void AudioPauseChannel(AudioChannel* player) {
    [player->node pause];
}

/**
 * Resumes the sound channel
 *
 * Playback begins from the audio frame it was paused at.  If this channel
 * is paused, this function does nothing.
 *
 * @param player    The sound channel
 */
void AudioResumeChannel(AudioChannel* player) {
    [player->node play];
}

/**
 * Returns true if this channel is actively playing.
 *
 * This function does not check if the channel is paused.
 *
 * @param player    The sound channel
 *
 * @return true if this channel is actively playing.
 */
bool AudioChannelPlaying(AudioChannel* player) {
    return player->playing;
}

/**
 * Returns true if this channel is actively paused.
 *
 * This function will also return false if the channel is not playing
 * anything at all.
 *
 * @param player    The sound channel
 *
 * @return true if this channel is actively paused.
 */
bool AudioChannelPaused(AudioChannel* player) {
    return !player->node.isPlaying && player->playing;
}

/**
 * Sets the volume for this sound channel
 *
 * The value set should be 0 to 1 where 0 is no volume and 1 is maximum
 * volume.  Any value outside of this range has undefined behavior.
 *
 * @param player    The sound channel
 * @param volume   The volume (0 to 1) to play the asset
 */
void AudioSetChannelVolume(AudioChannel* player, float volume) {
    player->node.volume = volume;
}

/**
 * Sets the loop option for this sound channel
 *
 * This setting only affects the currently attached sound asset.  If loop
 * is true, the sound asset will continue to play indefinitely unless
 * stopped manually.  If it is set to false, it will terminate the asset
 * at the end of the natural loop point.
 *
 * @param player    The sound channel
 * @param loop      Whether to loop the current attached asset
 */
void AudioSetChannelLoop(AudioChannel* player, bool loop) {
    player->timeStamp++;
    
    // Capture the current timestamp by value
    Uint64 stamp = player->timeStamp;
    
    // Objective-C style closure
    id callback = ^( void ) {
        if (player->timeStamp == stamp) {
            player->playing = false;
            cugl::AudioEngine::get()->gcEffect(player->channel,true);
        }
    };
    
    // Queue up the sound for as soon as possible
    if (!loop) {
        @autoreleasepool {
            // Create a blank buffer to break us out of the loop at loop point.
            AVAudioPCMBuffer* blank = [[AVAudioPCMBuffer alloc] initWithPCMFormat:player->format frameCapacity:0];
            [blank autorelease];
            [player->node scheduleBuffer:blank
                                  atTime:nil
                                 options:AVAudioPlayerNodeBufferInterruptsAtLoop
                       completionHandler:nil];
        }
    } else {
        // Add a loop
        [player->node scheduleBuffer:player->pcmb
                              atTime:nil
                             options:AVAudioPlayerNodeBufferInterruptsAtLoop
                   completionHandler:callback];
    }
    player->looping = loop;
}

/**
 * Returns the current audio frame of the given sound channel
 *
 * Together with the sampling rate, this can be used to compute the
 * number of seconds the player is into the sound asset.
 *
 * @param player    The sound channel
 *
 * @return the current audio frame of the given sound channel
 */
Uint64 AudioGetChannelFrame(AudioChannel* player) {
    AVAudioTime* last = player->node.lastRenderTime;
    NSTimeInterval time = (NSTimeInterval)[player->node playerTimeForNodeTime:last].sampleTime;
    return (player->startFrame+(Uint64)time) % player->pcmb.frameLength;
}

/**
 * Sets the current audio frame of the given sound channel
 *
 * This function will fast-forward or rewind the sound asset to the
 * given position.  This function will not pause or halt playback.
 *
 * @param player    The sound channel
 * @param frame     The audio frame to jump to
 */
void AudioSetChannelFrame(AudioChannel* player, Uint64 frame) {
    bool active = player->node.isPlaying;
    [player->node stop];
    InternalSchedulePCMBuffer(player,player->pcmb,player->looping,(Uint32)frame);
    if (active) {
        [player->node play];
    }
}

    
#pragma mark -
#pragma mark Background Music
/**
 * Advances the audo stream to the next page.
 *
 * This function is the core task of the audio engine processing thread. It
 * provides streaming audio by extracting the next PCM buffer from the audio
 * file.  It is called whenever the player complete a previously scheduled 
 * page.  By making sure we always have two pages "in flight", we guarantee
 * that the sound is smooth.
 *
 * @param player    The music player
 * @param stamp     The player timestamp (for garbage collection)
 */
void InternalAdvanceBackground(AudioPlayer* player, Uint64 stamp) {
    // Abort if garbage collector got here first
    if (player->timeStamp != stamp) {
        return;
    }
    
    @autoreleasepool {
        // The callback MUST send the next rquest to scheduling thread
        id callback = ^( void ) {
            _engine->processor->addTask([=] {
                InternalAdvanceBackground(player,stamp);
            });
        };
    
        AVAudioPCMBuffer* buffer  = nil;
        AVAudioFrameCount offset  = 0;
        AVAudioFrameCount remains = 0;
        bool fail = false;
        float volume = -1;
    
        {  // LOCK
            std::unique_lock<std::mutex> hold(player->lock);
            // Look at the position of the file
            remains = (AVAudioFrameCount)(player->file.length - player->file.framePosition);
        
            player->readFrame += player->firstOffset;
            player->firstOffset = player->secndOffset;

            if (remains == 0 && player->looping) {
                player->file.framePosition = 0;
                remains = (AVAudioFrameCount)player->file.length;
            } else if (remains == 0) {
                cugl::Application::get()->schedule([=] {
                    if (player->timeStamp == stamp) {
                        player->timeStamp++;
                        player->playing = false;
                        cugl::AudioEngine::get()->gcMusic(true);
                    }
                    return false;
                });
                return;
            }
        
            // Read into the back buffer if anything left
            offset = remains < BUFFER_SIZE ? remains : BUFFER_SIZE;
            buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:player->file.processingFormat
                                                   frameCapacity:offset];
            [buffer autorelease];
            
            NSError* error = nil;
            [player->file readIntoBuffer:buffer error:&error];
            if (error != nil) {
                CULogError("Audio streaming failure: %s", [[error localizedDescription] UTF8String]);
                player->timeStamp++;
                fail = true;
            }
        
            player->secndOffset = offset;
        
            // Into the fade
            if (player->fadeLength > 0) {
                player->fadePosition += offset;
                if (player->fadePosition <= player->fadeLength) {
                    float alpha = player->fadePosition/(float)player->fadeLength;
                    volume = player->fadeVolume*(1-alpha)+player->goalVolume*alpha;
                } else {
                    volume = player->goalVolume;
                    player->fadeLength = 0;
                
                    // Kill off future frames if this is to 0
                    if (volume == 0) {
                        offset = remains;
                        callback = ^( void ) {
                            cugl::Application::get()->schedule([=] {
                                if (player->timeStamp == stamp) {
                                    player->timeStamp++;
                                    player->playing = false;
                                    cugl::AudioEngine::get()->gcMusic(true);
                                }
                                return false;
                            });
                        };
                    }
                }
            
                player->node.volume = volume;
            }
        } // END LOCK
    
        if (fail) {
            [player->node stop];
        } else {
            [player->node scheduleBuffer:buffer
                                  atTime:nil
                                 options:0
                       completionHandler:callback];
        }
    }
}
    
/**
 * Plays the attached music asset in the background
 *
 * If there is no attached asset, the function call with fail. Otherwise, the
 * song will play until it is finished.  If loop is true, it will play 
 * indefinitely until halted explicilty.  When the audio halts it will call 
 * the gcMusic() method in AudioEngine.
 *
 * If the asset is set to loop, it will loop back to the beginning, not to
 * the start position.
 *
 * @param player    The music player
 * @param stamp     The player timestamp (for garbage collection)
 */
bool InternalScheduleBackground(AudioPlayer* player, Uint64 start, Uint64 stamp) {
    @autoreleasepool {
        std::unique_lock<std::mutex> hold(player->lock);
        if (start > player->file.length && !player->looping) {
            return false;
        }
        
        id callback = ^( void ) {
            _engine->processor->addTask([=] {
                InternalAdvanceBackground(player,stamp);
            });
        };
    
        AVAudioFrameCount remains = (AVAudioFrameCount)(player->file.length-start);
        AVAudioFrameCount first = 0;
        AVAudioFrameCount secnd = 0;
        AVAudioPCMBuffer* buffer1 = nil;
        AVAudioPCMBuffer* buffer2 = nil;
    
        player->file.framePosition = remains > 0 ? start : remains;
        player->readFrame = player->file.framePosition;
        
        // Read into the front two buffers
        if (remains > 0) {
            first = remains < BUFFER_SIZE ? remains : BUFFER_SIZE;
            buffer1 = [[AVAudioPCMBuffer alloc] initWithPCMFormat:player->file.processingFormat
                                                    frameCapacity:first];
            [buffer1 autorelease];
            
            NSError* error = nil;
            [player->file readIntoBuffer:buffer1 error:&error];
            if (error != nil) {
                return false;
            }
            
            player->firstOffset = first;
        }
        
        if (remains-first > 0 || player->looping) {
            secnd = (remains-first > 0 ? remains-first : remains);
            secnd = secnd < BUFFER_SIZE ? secnd : BUFFER_SIZE;
            
            buffer2 = [[AVAudioPCMBuffer alloc] initWithPCMFormat:player->file.processingFormat
                                                    frameCapacity:secnd];
            [buffer2 autorelease];
                
            NSError* error = nil;
            [player->file readIntoBuffer:buffer2 error:&error];
            if (error != nil) {
                return false;
            }
            
            player->secndOffset = secnd;
        }
    
        if (buffer1) {
            [player->node scheduleBuffer:buffer1
                                  atTime:nil
                                 options:0
                       completionHandler:callback];

        }
        if (buffer2) {
            [player->node scheduleBuffer:buffer2
                                  atTime:nil
                                 options:0
                       completionHandler:callback];
        }
        
        return true;
    }
}

/**
 * Returns a music player allocated for use with the audio engine
 *
 * The audio engine must be initialized for this function to succeed.  If
 * the function cannot allocate the player, it returns nullptr.  If a
 * music playerhas already been allocated, this returns a reference to the
 * previously allocated player.  As a result, it is a bad idea to call this
 * function more than once before freeing the player.
 *
 * @return a music player allocated for use with the audio engine
 */
AudioPlayer* AudioAllocBackground() {
    return _engine->background;
}

/**
 * Frees the music player, releasing all resources
 *
 * Once this method is called, it is unsafe to play music on this player
 * any longer.
 *
 * @param channel   The music player to free
 */
void AudioFreeBackground(AudioPlayer* player) {
    CUAssertLog(!player->playing, "Attempt to free an active music player");
    [player->file release];
    player->file = nil;
}
    
/**
 * Plays the music asset in the background
 *
 * The song will play until it is finished.  If loop is true, it will
 * play indefinitely until halted explicilty.  When the audio halts it
 * will call the gcMusic() method in AudioEngine.
 *
 * If the asset is set to loop, it will loop back to the beginning, not
 * the start position.
 *
 * @param player    The music player
 * @param source    The (streaming) audio asset
 * @param loop      Whether to loop the given asset
 * @param start     The position (in milliseconds) to start playback
 */
void AudioPlayBackground(AudioPlayer* player, AudioStream* source, bool loop, Uint32 start) {
    player->timeStamp++;
    
    if (player->node.playing) {
        [player->node stop];
    }
    
    if (player->file) {
        [player->file release];
        player->file = nullptr;
    }
    
    player->file = source->file;
    [player->file retain];
    
    {
        std::unique_lock<std::mutex> hold(player->lock);
        player->looping = loop;
        player->fadeLength = 0;
        player->node.volume = player->volume;
    }
    
    double sample = player->file.processingFormat.sampleRate;
    if (InternalScheduleBackground(player,(start/1000.0f)*sample,player->timeStamp)) {
        player->playing = true;
        [player->node play];
    } else {
        player->playing = false;
        cugl::AudioEngine::get()->gcMusic(false);
    }
}

/**
 * Plays the music asset in the background, fading from 0 volume.
 *
 * The song will play until it is finished.  If loop is true, it will
 * play indefinitely until halted explicilty.  When the audio halts it
 * will call the gcMusic() method in AudioEngine.
 *
 * The song will start out at 0 volume, rising the current player volume
 * after fade milliseconds.  If the volume is changed before then (or
 * the song finishes early), the fade effect will be canceled, and the
 * player will restore to its natural volume.
 *
 * If the asset is set to loop, it will loop back to the beginning, not
 * the start position.
 *
 * @param player    The music player
 * @param source    The (streaming) audio asset
 * @param loop      Whether to loop the given asset
 * @param start     The position (in milliseconds) to start playback
 * @param start     The time (in milliseconds) to fade in playback
 */
void AudioFadeInBackground(AudioPlayer* player, AudioStream* source, bool loop, Uint32 start, Uint32 fade) {
    player->timeStamp++;
    
    if (player->node.playing) {
        [player->node stop];
    }
    
    if (player->file) {
        [player->file release];
        player->file = nullptr;
    }
    
    player->file = source->file;
    [player->file retain];
    player->looping = loop;
    
    double sample = player->file.processingFormat.sampleRate;
    {
        std::unique_lock<std::mutex> hold(player->lock);
        player->fadeLength = (fade/1000.0f)*sample;
        player->fadePosition = 0;
        player->fadeVolume = 0;
        player->goalVolume = player->volume;
        player->node.volume = 0;
    }
    
    if (InternalScheduleBackground(player,(start/1000.0f)*sample,player->timeStamp)) {
        player->playing = true;
        [player->node play];
    } else {
        player->playing = false;
        cugl::AudioEngine::get()->gcMusic(false);
    }
}

/**
 * Halts the background music, garbage collecting the attached music asset.
 *
 * This function will call the gcMusic() method in AudioEngine.
 *
 * @param player    The music player
 */
void AudioHaltBackground(AudioPlayer* player) {
    {
        std::unique_lock<std::mutex> hold(player->lock);
        player->timeStamp++;
        player->playing = false;
        player->looping = false;
        player->node.volume = 0;
        // DO NOT STOP.  THIS CAUSES CLIPPING
    }
    cugl::AudioEngine::get()->gcMusic(false);
}

/**
 * Fades out the background music over given number of milliseconds.
 *
 * After the fade out is complete, the song will be halted and the player
 * will return to normal volume.  In addition, this function will then
 * call the gcMusic() method in AudioEngine.
 *
 * @param player    The music player
 * @param millis    The number of millisecond before halting the asset
 */
void AudioFadeOutBackground(AudioPlayer* player, Uint32 millis) {
    std::unique_lock<std::mutex> hold(player->lock);
    double sample = player->file.processingFormat.sampleRate;
    player->fadeLength = (millis/1000.0f)*sample;
    player->fadePosition = 0;
    player->goalVolume = 0;
    player->fadeVolume = player->node.volume;
}

/**
 * Pauses the music player
 *
 * The music asset remains attached to the player and will continue to
 * play once it resumes.  If this player is already paused, this function
 * does nothing.
 *
 * @param player    The music player
 */
void AudioPauseBackground(AudioPlayer* player) {
    [player->node pause];
}

/**
 * Resumes the music player
 *
 * Playback begins from the second it was paused at.  If this channel
 * is paused, this function does nothing.
 *
 * @param player    The music player
 */
void AudioResumeBackground(AudioPlayer* player) {
    [player->node play];
}

/**
 * Returns true if the background music is actively playing.
 *
 * This function does not check if the player is paused.
 *
 * @param player    The music player
 *
 * @return true if the background music is actively playing.
 */
bool AudioBackgroundPlaying(AudioPlayer* player) {
    return player->playing;
}

/**
 * Returns true if the background music is actively paused.
 *
 * This function will also return false if there is no music playing at all.
 *
 * @param player    The music player
 *
 * @return true if the background music is actively paused.
 */
bool AudioBackgroundPaused(AudioPlayer* player) {
    return player->playing && !player->node.isPlaying;
}

/**
 * Sets the volume for the background music
 *
 * The value set should be 0 to 1 where 0 is no volume and 1 is maximum
 * volume.  Any value outside of this range has undefined behavior.
 *
 * @param player    The music player
 * @param volume    The volume (0 to 1) to play the asset
 */
void AudioSetBackgroundVolume(AudioPlayer* player, float volume) {
    std::unique_lock<std::mutex> hold(player->lock);
    player->fadeLength = 0;
    player->volume = volume;
    player->node.volume = volume;
}

/**
 * Sets the loop option for this music player
 *
 * This setting only affects the currently attached music asset.  If loop
 * is true, the music asset will continue to play indefinitely unless
 * stopped manually.  If it is set to false, it will terminate the asset
 * at the end of the natural loop point.
 *
 * @param player    The music player
 * @param loop      Whether to loop the current attached asset
 */
void AudioSetBackgroundLoop(AudioPlayer* player, bool loop) {
    std::unique_lock<std::mutex> hold(player->lock);
    player->looping = loop;
}

/**
 * Returns the elapsed number of seconds of the audio stream
 *
 * If the music asset is playing in a loop, this function returns the
 * elapsed time since the beginning of the song.
 *
 * @param player    The music player
 *
 * @return the elapsed number of seconds of the audio stream
 */
double AudioGetBackgroundTime(AudioPlayer* player) {
    // With the buffer size, this is just as accurate as SDL
    std::unique_lock<std::mutex> hold(player->lock);
    Uint64 frame = player->readFrame+player->firstOffset/2;
    return frame/player->file.processingFormat.sampleRate;
}

/**
 * Sets the elapsed number of seconds of the audio stream
 *
 * This function will fast-forward or rewind the sound asset to the
 * given position.  This function will not pause or halt playback.
 *
 * @param player    The sound channel
 * @param time      The position to jump to
 */
void  AudioSetBackgroundTime(AudioPlayer* player, double time) {
    Uint64 frame = 0;
    {
        std::unique_lock<std::mutex> hold(player->lock);
        player->timeStamp++;
        if (player->fadeLength > 0) {
            player->fadeLength = 0;
            player->node.volume = player->goalVolume;
        }
        frame = time*player->file.processingFormat.sampleRate;
    }
    InternalScheduleBackground(player,frame,player->timeStamp);
}

}
}
