//
//  AVOggAudioFile.m
//
//  This module provides partial AVFoundation support for OGG (Vorbis) files.
//  It creates a protocol with all of the read-only features of an AVAudioFile,
//  and then adds those to the interface of an OGG file.  This protocol gives
//  us a uniform interface for our AudioEngine.
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
#import "AVOggAudioFile.h"

#include <vorbis/vorbisfile.h>
#include <string.h>

/** The read size of ogg packet (in our tests, this is too large) */
#define OGG_WINDOW 1024

/**
 * Class to provide AVFoundation support for OGG (Vorbis) files.
 *
 * Unlike a lot of codecs, the ogg/vorbis codec is really very simple. We
 * can easily extract PCM data from the file in a format that is essentially
 * identical to the standard AVAudioPCMBuffer format. This class encapsulates
 * this process behind the AVAudioFileSource protocol
 */
@implementation AVOggAudioFile {
    /** The OGG file pointer for decoder support */
    OggVorbis_File ogg;
    /** Format information such as sample rate and number of channels */
    vorbis_info*  info;
    /** Reference to the logical bitstream for decoding */
    int bitstream;
    /** Amount that we over-read from the last packet and must store for later */
    int64_t overspill;
    /** Buffer to store the data that is over-read from the last packet */
    float** readbuffer;
}

/**
 * Opens a file for reading.
 *
 * @param fileURL   The path of the file to read.
 * @param outError  Returns, by-reference, a description of the error, if an
 *                  error occurs.
 *
 * @return An initialized audio file object for reading.
 */
- (instancetype)initForReading:(NSURL *)fileURL
                         error:(NSError * _Nullable *)outError {
    if ( self = [super init] ) {
        // Load the OGG and look for errors
        bitstream = -1;
        int error = ov_fopen([[fileURL path] UTF8String],&ogg);

        if (error) {
            if (outError != NULL) {
                NSString *description;
                int errCode;
                
                if (error == OV_EREAD) {
                    description = NSLocalizedString(@"The file could not be read", nil);
                    errCode = OV_EREAD;
                } else if (error == OV_ENOTVORBIS) {
                    description = NSLocalizedString(@"The file does not contain any Vorbis data", nil);
                    errCode = OV_ENOTVORBIS;
                } else if (error == OV_EVERSION) {
                    description = NSLocalizedString(@"The Vorbis header version does not match", nil);
                    errCode = OV_EVERSION;
                } else if (error == OV_EVERSION) {
                    description = NSLocalizedString(@"The Vorbis header version does not match", nil);
                    errCode = OV_EVERSION;
                } else if (errno == OV_EBADHEADER) {
                    description = NSLocalizedString(@"The Vorbis bitstream header is invalid", nil);
                    errCode = OV_EBADHEADER;
                } else if (errno == OV_EFAULT) {
                    description = NSLocalizedString(@"The Vorbis codec has an internal logic fault", nil);
                    errCode = OV_EBADHEADER;
                } else {
                    description = NSLocalizedString(@"There was an unknown error", nil);
                    errCode = errno;
                }
                
                // Create and return the custom domain error.
                NSDictionary *errorDictionary = @{ NSLocalizedDescriptionKey : description,
                                                   NSFilePathErrorKey : fileURL.absoluteString };
                
                *outError = [NSError errorWithDomain:@"OggDecoderError"
                                                code:errCode
                                            userInfo:errorDictionary];
            }
            return nil;
        }
        
        info = ov_info(&ogg, bitstream);
        
        // Create the overspill buffers
        overspill = 0;
        readbuffer = malloc(info->channels*sizeof(float*));
        for(int ii = 0; ii < info->channels; ii++) {
            readbuffer[ii] = malloc(OGG_WINDOW*sizeof(float));
        }
        
        // Initialize all the properties
        _framePosition = 0;
        _fileFormat = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:info->rate
                                                                     channels:info->channels];

        _processingFormat = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32
                                                             sampleRate:info->rate
                                                               channels:info->channels
                                                            interleaved:NO];
        
        _length = ov_pcm_total(&ogg, bitstream);
        _url = [fileURL retain];
    }
    return self;
}

/**
 * Deletes this OGG file, releasing resources upon garbage collection.
 */
-(void)dealloc {
    for(int ii = 0; ii < info->channels; ii++) {
        free(readbuffer[ii]);
    }
    free(readbuffer);
    readbuffer = nil;
    ov_clear(&ogg);
    
    [_url release];
    [_processingFormat release];
    [_fileFormat release];
    [super dealloc];
}

/**
 * Sets the position in the file at which the next read will occur.
 *
 * @param framePosition The new audio frame
 */
- (void)setFramePosition:(AVAudioFramePosition)framePosition {
    _framePosition = framePosition;
    ov_pcm_seek(&ogg, framePosition);
    overspill = 0;
}

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
- (BOOL)readIntoBuffer:(AVAudioPCMBuffer *)buffer
                 error:(NSError * _Nullable *)outError {
    AVAudioFrameCount count = self.length < buffer.frameLength ? (AVAudioFrameCount)self.length : buffer.frameCapacity;
    return [self readIntoBuffer:buffer frameCount:count error:outError];
}

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
- (BOOL)readIntoBuffer:(AVAudioPCMBuffer *)buffer
            frameCount:(AVAudioFrameCount)frames
                 error:(NSError * _Nullable *)outError {
    uint64_t read = 0;
    AVAudioFormat* format = self.processingFormat;
    
    // First read the overspill
    int next = overspill < frames ? (int)overspill : frames;
    if (next > 0) {
        for (AVAudioChannelCount ch = 0; ch < format.channelCount; ++ch) {
            size_t amount = next * format.streamDescription->mBytesPerFrame;
            memcpy(&(buffer.floatChannelData[ch][read]), readbuffer[ch], amount);
        }
        read += next;
        overspill = 0;
    }

    // Now read from the stream
    float** pcmb;
    while (read < frames) {
        next = (int)ov_read_float(&ogg, &pcmb, OGG_WINDOW, &bitstream);
        if (next < 0) {
            if (outError != NULL) {
                NSString *description;
                int errCode;
                
                if (next == OV_HOLE) {
                    description = NSLocalizedString(@"There was an interruption in data", nil);
                    errCode = OV_HOLE;
                } else if (next == OV_EBADLINK) {
                    description = NSLocalizedString(@"There was an invalid stream section", nil);
                    errCode = OV_EBADLINK;
                } else if (next == OV_EINVAL) {
                    description = NSLocalizedString(@"The file headers cannot be read", nil);
                    errCode = OV_EBADLINK;
                } else {
                    description = NSLocalizedString(@"No error", nil);
                    errCode = 0;
                }
                
                // Create and return the custom domain error.
                NSDictionary *errorDictionary = @{ NSLocalizedDescriptionKey : description,
                                                   NSFilePathErrorKey : self.url.absoluteString };
                
                *outError = [NSError errorWithDomain:@"OggDecoderError"
                                                code:errCode
                                            userInfo:errorDictionary];
            }
            return NO;
        }
        
        // Figure out how much can go in buffer
        int actual = next < frames-read ? next : (int)(frames-read);
        for (AVAudioChannelCount ch = 0; ch < format.channelCount; ++ch) {
            size_t amount = actual * format.streamDescription->mBytesPerFrame;
            memcpy(&(buffer.floatChannelData[ch][read]), pcmb[ch], amount);
        }
        read += actual;
        
        // Write to overspill if anything left over
        if (next-actual > 0) {
            for (AVAudioChannelCount ch = 0; ch < format.channelCount; ++ch) {
                size_t amount = (next-actual) * format.streamDescription->mBytesPerFrame;
                memcpy(readbuffer[ch],&(pcmb[ch][actual]),amount);
            }
            overspill = next-actual;
        }
    }
    
    buffer.frameLength = (unsigned int)read;
    _framePosition += read;
    return YES;
}
@end
