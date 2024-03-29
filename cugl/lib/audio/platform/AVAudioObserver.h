//
//  AVAudioObserver.h
//
//  This module provides an observer for AVAudioEngineConfigurationChangeNotification.
//  Without this, an app will crash if you pull out headphones.
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

@interface AVAudioObserver : NSObject

// Properties
/** A reference to the audio engine */
@property(assign, nonatomic, nullable) AVAudioEngine *engine;

// Methods
/**
 * Initializes an observer for the given engine.
 *
 * @param engine    The audio engine.
 *
 * @return An initialized audio file object for reading.
 */
- (instancetype _Nullable)init:(AVAudioEngine * _Nullable)engine;

/**
 * Handles a notification to reconfigure the audio engine.
 *
 * @param notice    The interruption notification
 *
 * @return An initialized audio file object for reading.
 */
- (void)handleInterruption:(NSNotification * _Nullable)notice;


@end
