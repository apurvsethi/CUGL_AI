//
//  AVOggAudioFile.h
//
//  This module provides partial AVFoundation support for OGG files. It creates
//  a protocol with all of the read-only features of an AVAudioFile, and then
//  adds those to the interface of an OGG file.  This protocol gives us a
//  uniform interface for our AudioEngine.
//
//  If we could have subclasses AVAudioFile, that would have allowed us to
//  plug-and-play OGG files into AVAudioEngine.  However, most of the inherited
//  properties of AVAudioFile are read-only, so this is not possible.
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
//  Version: 12/14/16
//
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#pragma mark -
#pragma mark AVAudioFileSource Protocol

/**
 * Protocol provides a uniform interface for read-only audio files.
 *
 * The primary thing that we need from an AVAudioFile is the ability to
 * read file segments into a AVAudioPCMBuffer. The protocol allows us to
 * use a uniform pointer for multiple file implementation.  Theoretically,
 * if we were to add other codecs (though FLAC is a bit of a mess), we
 * could use this protocol for them as well.
 */
@protocol AVAudioFileSource<NSObject>

// Properties
/** The URL the file is reading */
@property(nonatomic, readonly, nullable) NSURL *url;

/**
 * The number of sample frames in the file.
 *
 * This can be computationally expensive to compute for the first time.
 */
@property(nonatomic, readonly) AVAudioFramePosition length;

/**
 * The position in the file at which the next read operation will occur.
 *
 * Set the framePosition property to perform a seek before a read. A read
 * operation advances the frame position value by the number of frames read.
 */
@property(nonatomic) AVAudioFramePosition framePosition;

/** The on-disk format of the file. */
@property(nonatomic, readonly, nullable) AVAudioFormat *fileFormat;

/** The processing format of the file. */
@property(nonatomic, readonly, nullable) AVAudioFormat *processingFormat;

// Methods
/**
 * Opens a file for reading.
 *
 * @param fileURL   The path of the file to read.
 * @param outError  Returns, by-reference, a description of the error, if an 
 *                  error occurs.
 *
 * @return An initialized audio file object for reading.
 */
- (instancetype _Nullable)initForReading:(NSURL * _Nullable)fileURL
                         error:(NSError * _Nullable * _Nullable)outError;

/**
 * Reads an entire buffer.
 *
 * Reading sequentially from the framePosition property, this method attempts 
 * to fill the buffer to its capacity. On return, the buffer’s length property 
 * indicates the number of sample frames successfully read.
 *
 * @param buffer    The buffer from which to read the file. Its format must
 *                  match the file’s processing format.
 * @param outError  Returns, by-reference, a description of the error, if an
 *                  error occurs.
 *
 * @return YES, if the read was successful; otherwise NO.
 */
- (BOOL)readIntoBuffer:(AVAudioPCMBuffer * _Nullable)buffer
                 error:(NSError * _Nullable * _Nullable)outError;

/**
 * Reads a portion of a buffer.
 *
 * Reading sequentially from the framePosition property, this method attempts
 * to fill the buffer with the given number of frames. On return, the buffer’s 
 * length property indicates the number of sample frames successfully read.
 *
 * @param buffer    The buffer from which to read the file. Its format must
 *                  match the file’s processing format.
 * @param frames    The number of frames to read.
 * @param outError  Returns, by-reference, a description of the error, if an
 *                  error occurs.
 *
 * @return YES, if the read was successful; otherwise NO.
 */
- (BOOL)readIntoBuffer:(AVAudioPCMBuffer * _Nullable)buffer
            frameCount:(AVAudioFrameCount)frames
                 error:(NSError * _Nullable * _Nullable)outError;
@end

#pragma mark -
#pragma mark AVAudioFileReadOnly Category

/**
 * A category to separate out the read-only properties of AVAudioFile
 *
 * The primary thing that we need from an AVAudioFile is the ability to
 * read file segments into a AVAudioPCMBuffer. The protocol allows us to
 * use a uniform pointer for multiple file implementation.
 */
@interface AVAudioFile (AVAudioFileReadOnly) <AVAudioFileSource>
@end

#pragma mark -
#pragma mark AVOggAudioFile Class

/**
 * Class to provide AVFoundation support for OGG (Vorbis) files.
 *
 * Unlike a lot of codecs, the ogg/vorbis codec is really very simple. We
 * can easily extract PCM data from the file in a format that is essentially
 * identical to the standard AVAudioPCMBuffer format. This class encapsulates
 * this process behind the AVAudioFileSource protocol
 */
@interface AVOggAudioFile : NSObject <AVAudioFileSource>

// Properties
/** The URL the file is reading */
@property(nonatomic, readonly, nullable) NSURL *url;

/**
 * The number of sample frames in the file.
 *
 * This can be computationally expensive to compute for the first time.
 */
@property(nonatomic, readonly) AVAudioFramePosition length;

/**
 * The position in the file at which the next read or write operation will occur.
 *
 * Set the framePosition property to perform a seek before a read. A read
 * operation advances the frame position value by the number of frames read.
 */
@property(nonatomic) AVAudioFramePosition framePosition;

/** The on-disk format of the file. */
@property(nonatomic, readonly, nullable) AVAudioFormat *fileFormat;

/** The processing format of the file. */
@property(nonatomic, readonly, nullable) AVAudioFormat *processingFormat;

// Methods
/**
 * Opens a file for reading.
 *
 * @param fileURL   The path of the file to read.
 * @param outError  Returns, by-reference, a description of the error, if an
 *                  error occurs.
 *
 * @return An initialized audio file object for reading.
 */
- (instancetype _Nullable)initForReading:(NSURL * _Nullable)fileURL
                         error:(NSError * _Nullable * _Nullable)outError;

/**
 * Reads an entire buffer.
 *
 * Reading sequentially from the framePosition property, this method attempts
 * to fill the buffer to its capacity. On return, the buffer’s length property
 * indicates the number of sample frames successfully read.
 *
 * @param buffer    The buffer from which to read the file. Its format must
 *                  match the file’s processing format.
 * @param outError  Returns, by-reference, a description of the error, if an
 *                  error occurs.
 *
 * @return YES, if the read was successful; otherwise NO.
 */
- (BOOL)readIntoBuffer:(AVAudioPCMBuffer * _Nullable)buffer
                 error:(NSError * _Nullable * _Nullable)outError;

/**
 * Reads a portion of a buffer.
 *
 * Reading sequentially from the framePosition property, this method attempts
 * to fill the buffer with the given number of frames. On return, the buffer’s
 * length property indicates the number of sample frames successfully read.
 *
 * @param buffer    The buffer from which to read the file. Its format must
 *                  match the file’s processing format.
 * @param frames    The number of frames to read.
 * @param outError  Returns, by-reference, a description of the error, if an
 *                  error occurs.
 *
 * @return YES, if the read was successful; otherwise NO.
 */
- (BOOL)readIntoBuffer:(AVAudioPCMBuffer * _Nullable)buffer
            frameCount:(AVAudioFrameCount)frames
                 error:(NSError * _Nullable * _Nullable)outError;
@end
