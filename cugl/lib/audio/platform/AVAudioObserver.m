//
//  AVAudioObserver.m
//  CUGL
//
//  Created by Walker White on 4/27/17.
//  Copyright © 2017 Game Design Initiative at Cornell. All rights reserved.
//

#import "AVAudioObserver.h"

@implementation AVAudioObserver

/**
 * Initializes an observer for the given engine.
 *
 * @param engine    The audio engine.
 *
 * @return An initialized audio file object for reading.
 */
- (instancetype _Nullable)init:(AVAudioEngine * _Nullable)engine {
    if ( self = [super init] ) {
        self.engine = engine;
        return self;
    }
    return nil;
}

/**
 * Handles a notification to reconfigure the audio engine.
 *
 * @param notice    The interruption notification
 *
 * @return An initialized audio file object for reading.
 */
- (void)handleInterruption:(NSNotification * _Nullable)notice {
    if (!self.engine.running) {
        NSError* error = nil;
        [self.engine startAndReturnError:&error];
        if (error != nil) {
            NSLog(@"Unable to reset audio %@",[error localizedDescription]);
        }
    }
}



@end
