//
//  CUDisplay.cpp
//  Cornell University Game Library (CUGL)
//
//  This module is a singleton providing display information about the device.
//  Originally, we had made this part of Application.  However, we discovered
//  that we needed platform specfic code for this, so we factored it out.
//
//  Becuase this is a singleton, there are no publicly accessible constructors
//  or intializers.  Use the static methods instead.
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

#include <cugl/base/CUDisplay.h>
#include <cugl/util/CUDebug.h>
#include "platform/CUDisplay-impl.h"

using namespace cugl;
using namespace cugl::impl;

/** The display singleton */
Display* Display::_thedisplay = nullptr;

#pragma mark Constructors
/**
 * Creates a new, unitialized Display.
 *
 * All of the values are set to 0 (except _aspect, which is 16:9). You
 * must initialize the Display to access its values
 *
 * WARNING: This class is a singleton.  You should never access this
 * constructor directly.  Use the {@link start()} method instead.
 */
Display::Display() :
_aspect(Aspect::LANDSCAPE_16_9) {}

/**
 * Initializes the display with the current screen information.
 *
 * This method gathers the native resolution bounds and pixel density
 * using platform-specific tools.
 *
 * WARNING: This class is a singleton.  You should never access this
 * initializer directly.  Use the {@link start()} method instead.
 *
 * @return true if initialization was successful.
 */
bool Display::init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        CULogError("Could not initialize display: %s",SDL_GetError());
        return false;
    }
    
    _bounds = DisplayBounds();
    _usable = DisplayUsableBounds();
    _aspect = Display::getAspect(_bounds.size.width/_bounds.size.height);
    _scale  = DisplayPixelDensity();
    return true;
}

/**
 * Uninitializes this object, releasing all resources.
 *
 * This method quits the SDL video system, effectively exitting shutting
 * down the entire program.
 *
 * WARNING: This class is a singleton.  You should never access this
 * method directly.  Use the {@link stop()} method instead.
 */
void Display::dispose() {
    _bounds.size.set(0,0);
    _usable.size.set(0,0);
    _scale.setZero();
    _aspect = Aspect::LANDSCAPE_16_9;
    SDL_Quit();
}

#pragma mark -
#pragma mark Static Accessors
/**
 * Starts up the SDL display and video system.
 *
 * This static method needs to be the first line of any application, though
 * it is handled automatically in the {@link Application} class.
 *
 * Once this method is called, the {@link get()} method will no longer
 * return a null value.
 */
bool Display::start() {
    if (_thedisplay != nullptr) {
        CUAssertLog(false, "The display is already initialized");
        return false;
    }
    _thedisplay = new Display();
    return _thedisplay->init();
}

/**
 * Shuts down the SDL display and video system.
 *
 * This static method needs to be the last line of any application, though
 * it is handled automatically in the {@link Application} class.
 *
 * Once this method is called, the {@link get()} method will return nullptr.
 * More importantly, no SDL function calls will work anymore.
 */
void Display::stop() {
    if (_thedisplay == nullptr) {
        CUAssertLog(false, "The display is not initialized");
    }
    delete _thedisplay;
    _thedisplay = nullptr;
}

#pragma mark -
#pragma mark Aspect Utilities
/**
 * Returns the aspect for the given aspect ratio.
 *
 * It is safest to represent aspects as an enum, not a ratio.  Round off
 * error might cause devices with very similar aspect ratios to have
 * slightly different ratio values.  Therefore, the enum is a way of
 * normalizing device aspects.
 *
 * Device aspects are relatively standardized.  For information on your
 * device, see
 *
 *      http://mydevice.io/devices/
 *
 * This method is guaranteed to match every aspect ratio on that page.
 * If the aspect ratio is not on that page, it will return UNKNOWN.
 *
 * @param ratio The aspect ratio in the form width/height
 *
 * @return the aspect for the given aspect ratio.
 */
Display::Aspect Display::getAspect(float ratio) {
    const float tolerance = 0.001f;
    if (CU_MATH_APPROX(ratio, 1.0f, tolerance)) {
        return Aspect::SQUARE;
    } else if (ratio < 1) {
        if (CU_MATH_APPROX(ratio, 9.0f/16.0f, tolerance)) {
            return Aspect::PORTRAIT_9_16;
        } else if (CU_MATH_APPROX(ratio, 3.0f/4.0f, tolerance)) {
            return Aspect::PORTRAIT_3_4;
        } else if (CU_MATH_APPROX(ratio, 2.0f/3.0f, tolerance)) {
            return Aspect::PORTRAIT_2_3;
        } else if (CU_MATH_APPROX(ratio, 10.0f/16.0f, tolerance)) {
            return Aspect::PORTRAIT_10_16;
        } else if (CU_MATH_APPROX(ratio, 3.0f/5.0f, tolerance)) {
            return Aspect::PORTRAIT_3_5;
        } else if (CU_MATH_APPROX(ratio, 600.0f/1024.0f, tolerance)) {
            return Aspect::PORTRAIT_600_1024;
        } else if (CU_MATH_APPROX(ratio, 512.0f/683.0f, tolerance)) {
            return Aspect::PORTRAIT_IPAD_PRO;
        }
    } else {
        if (CU_MATH_APPROX(ratio, 16.0f/9.0f, tolerance)) {
            return Aspect::LANDSCAPE_16_9;
        } else if (CU_MATH_APPROX(ratio, 4.0f/2.0f, tolerance)) {
            return Aspect::LANDSCAPE_4_3;
        } else if (CU_MATH_APPROX(ratio, 3.0f/2.0f, tolerance)) {
            return Aspect::LANDSCAPE_3_2;
        } else if (CU_MATH_APPROX(ratio, 16.0f/10.0f, tolerance)) {
            return Aspect::LANDSCAPE_16_10;
        } else if (CU_MATH_APPROX(ratio, 5.0f/2.0f, tolerance)) {
            return Aspect::LANDSCAPE_5_3;
        } else if (CU_MATH_APPROX(ratio, 1024.0f/600.0f, tolerance)) {
            return Aspect::LANDSCAPE_1024_600;
        } else if (CU_MATH_APPROX(ratio, 683.0f/512.0f, tolerance)) {
            return Aspect::LANDSCAPE_IPAD_PRO;
        }
    }
    
    return Aspect::UKNOWN;
}

/**
 * Returns the aspect ratio for the given aspect.
 *
 * The value is computed width/height. If the aspect is UNKNOWN, it will
 * return 0.
 *
 * @param aspect    The device aspect value
 *
 * @return the aspect ratio for the given aspect.
 */
float Display::getAspectRatio(Aspect aspect) {
    switch (aspect) {
        case Aspect::SQUARE:
            return 1.0f;
        case Aspect::PORTRAIT_3_4:
            return 3.0f/4.0f;
        case Aspect::PORTRAIT_2_3:
            return 2.0f/3.0f;
        case Aspect::PORTRAIT_10_16:
            return 10.0f/16.0f;
        case Aspect::PORTRAIT_3_5:
            return 3.0f/5.0f;
        case Aspect::PORTRAIT_9_16:
            return 9.0f/16.0f;
        case Aspect::PORTRAIT_600_1024:
            return 600.0f/1024.0f;
        case Aspect::PORTRAIT_IPAD_PRO:
            return 512.0f/683.0f;
        case Aspect::LANDSCAPE_4_3:
            return 4.0f/3.0f;
        case Aspect::LANDSCAPE_3_2:
            return 3.0f/2.0f;
        case Aspect::LANDSCAPE_16_10:
            return 16.0f/10.0f;
        case Aspect::LANDSCAPE_5_3:
            return 5.0f/3.0f;
        case Aspect::LANDSCAPE_16_9:
            return 16.0f/9.0f;
        case Aspect::LANDSCAPE_1024_600:
            return 1024.0f/600.0f;
        case Aspect::LANDSCAPE_IPAD_PRO:
            return 683.0f/512.0f;
        default:
            break;
    }
    return 0;
}

/**
 * Returns a string representation of the given aspect
 *
 * This value is useful for debugging.  The first part of the string,
 * before the space, is guaranteed to be in the format x:y
 *
 * @param aspect    The device aspect value
 *
 * @return a string representation of the given aspect
 */
const std::string Display::getAspectName(Aspect aspect) {
    switch (aspect) {
        case Aspect::SQUARE:
            return "1:1 Square";
        case Aspect::PORTRAIT_3_4:
            return "3:4 Portrait";
        case Aspect::PORTRAIT_2_3:
            return "2:3 Portrait";
        case Aspect::PORTRAIT_10_16:
            return "10:16 Portrait";
        case Aspect::PORTRAIT_3_5:
            return "3:5 Portrait";
        case Aspect::PORTRAIT_9_16:
            return "9:16 Portrait";
        case Aspect::PORTRAIT_600_1024:
            return "600:1024 Portrait";
        case Aspect::PORTRAIT_IPAD_PRO:
            return "iPad Pro Portrait";
        case Aspect::LANDSCAPE_4_3:
            return "4:3 Landscape";
        case Aspect::LANDSCAPE_3_2:
            return "3:2 Landscape";
        case Aspect::LANDSCAPE_16_10:
            return "16:10 Landscape";
        case Aspect::LANDSCAPE_5_3:
            return "5:3 Landscape";
        case Aspect::LANDSCAPE_16_9:
            return "16:9 Landscape";
        case Aspect::LANDSCAPE_1024_600:
            return "1024:600 Landscape";
        case Aspect::LANDSCAPE_IPAD_PRO:
            return "iPad Pro Landscape";
        default:
            break;
    }
    return "Unknown";
}
