//
//  CUDisplay-iOS.mm
//  Cornell University Game Library (CUGL)
//
//  The SDL display information tools fail on OS X and iOS.  Therefore, we have
//  factored this information out into platform specific files.  This module
//  is the iOS implementation.
//
//  Note the .mm suffix.  That suffix is necessary in XCode for any module that
//  combines C++ and Objective-C code.
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
//  Version: 7/12/16

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIKit.h>

#include "CUDisplay-impl.h"

/**
 * Returns the full screen resolution for this display
 *
 * This method returns the bounds for the current resolution, not the
 * maximum resolution.  You should never change the resolution of a display.
 * Allow the user to have their preferred resolution.  Instead, you should
 * adjust your camera to scale the viewport.
 *
 * The value returned represents points, not pixels.  If you are using a
 * traditional display, these are the same.  However, on Retina displays
 * and other high DPI monitors, these may be different.  Regardless, you
 * should always work with points, not pixels, when computing the screen
 * size.  In particular, this is what you should assign the OpenGL viewport
 * when using fullscreen.
 */
cugl::Rect cugl::impl::DisplayBounds() {
    cugl::Rect result;
    CGRect orientation = [[UIScreen mainScreen] bounds];
    bool portrait = orientation.size.width < orientation.size.height;
    
    CGRect screenRect = [[UIScreen mainScreen] nativeBounds];
    result.origin.x = screenRect.origin.x;
    result.origin.y = screenRect.origin.y;
    result.size.width  = (portrait ? screenRect.size.width : screenRect.size.height);
    result.size.height = (portrait ? screenRect.size.height : screenRect.size.width);

    return result;
}

/**
 * Returns the usable full screen resolution for this display
 *
 * Usable is a subjective term defined by the operating system.  In general,
 * it means the full screen minus any space used by important user interface
 * elements.  For this implementation, it subtracts the staus bar from the
 * height.
 *
 * This method computes the bounds for the current resolution, not the
 * maximum resolution.  You should never change the resolution of a display.
 * Allow the user to have their preferred resolution.  Instead, you should
 * adjust your camera to scale the viewport.
 *
 * The value returned represents points, not pixels.  If you are using a
 * traditional display, these are the same.  However, on Retina displays
 * and other high DPI monitors, these may be different.  Regardless, you
 * should always work with points, not pixels, when computing the screen
 * size.
 */
cugl::Rect cugl::impl::DisplayUsableBounds() {
    cugl::Rect result;
    CGRect orientation = [[UIScreen mainScreen] bounds];
    bool portrait = orientation.size.width < orientation.size.height;
    
    CGRect screenRect = [[UIScreen mainScreen] nativeBounds];
    result.origin.x = screenRect.origin.x;
    result.origin.y = screenRect.origin.y;
    result.size.width  = (portrait ? screenRect.size.width : screenRect.size.height);
    result.size.height = (portrait ? screenRect.size.height : screenRect.size.width);
    CGFloat statusbar = [UIApplication sharedApplication].statusBarFrame.size.height;
    result.size.height -= statusbar;
    return result;
}

/**
 * Returns the number of pixels for each point.
 *
 * A point is a logical screen pixel.  If you are using a traditional
 * display, points and pixels are the same.  However, on Retina displays
 * and other high dpi monitors, they may be different.  In particular,
 * the number of pixels per point is a scaling factor times the point.
 *
 * You should never need to use these scaling factor for anything, as it
 * is not useful for determining anything other than whether a high DPI
 * display is present. It does not necessarily refer to physical pixel on
 * the screen. 
 *
 * @return the number of pixels for each point.
 */
cugl::Vec2 cugl::impl::DisplayPixelDensity() {
    CGRect screenRect  = [[UIScreen mainScreen] nativeBounds];
    CGRect displayRect = [[UIScreen mainScreen] bounds];

    if (displayRect.size.width > displayRect.size.height) {
        CGFloat temp = screenRect.size.width;
        screenRect.size.width = screenRect.size.height;
        screenRect.size.height= temp;
    }
    
    return cugl::Vec2((float)screenRect.size.width/displayRect.size.width,
                      (float)screenRect.size.height/displayRect.size.height);
}