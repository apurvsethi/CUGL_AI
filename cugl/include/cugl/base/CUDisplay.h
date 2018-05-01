//
//  CUDisplay.h
//  Cornell University Game Library (CUGL)
//
//  This module is a singleton providing display information about the device.
//  Originally, we had made this part of Application.  However, we discovered
//  that we needed platform specfic code for this, so we factored it out.
//
//  Because this is a singleton, there are no publicly accessible constructors
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
// TODO: Orientation detection for mobile devices
#ifndef __CU_DISPLAY_H__
#define __CU_DISPLAY_H__
#include <cugl/math/CURect.h>

namespace cugl {

/**
 * This class is a singleton representing the native display.
 *
 * The static methods of this class {@link start()} and {@link stop()} the
 * SDL video system.  Without it, you cannot draw anything.  This should be
 * the first and last methods called in any application. The {@link Application}
 * class does this for you automatically.
 *
 * The singleton display object also has several methods to get the (current)
 * screen resolution and aspect ratio. The most important of these two is the
 * the aspect ratio.  Aspect ratio is one of the most unportable parts of 
 * cross-platform development.  Everything else can be scaled to the screen,
 * but the aspect ratio is fixed from the very beginning.
 *
 * If the device has multiple displays, this singleton will only refer to the
 * main display.
 */
class Display {
public:
    /**
     * The display aspect ratio.
     *
     * This enum includes support for almost every shipping aspect rations.
     * For information on your device, see
     *
     *      http://mydevice.io/devices/
     */
    enum class Aspect : unsigned int {
        /**
         * Aspect ratio of 1:1
         *
         * This is the aspect ratio of many early Blackberry devices.
         */
        SQUARE = 0,
        /**
         * Portrait aspect ratio of 3:4
         *
         * This is the portrait aspect ratio of most Apple iPad's except for the
         * iPad Pro.
         */
        PORTRAIT_3_4 = 1,
        /**
         * Portrait aspect ratio of 2:3
         *
         * This is the portrait aspect ratio of older iPhones (before the 4s)
         * and Microsoft Surface 3.
         */
        PORTRAIT_2_3 = 2,
        /**
         * Portrait aspect ratio of 10:16
         *
         * This is the portrait aspect ratio of 8" and 10" Samsung tablets.
         */
        PORTRAIT_10_16 = 3,
        /**
         * Portrait aspect ratio of 3:5
         *
         * This is the portrait aspect ratio of Samsung Galaxy S tablets.
         */
        PORTRAIT_3_5  = 4,
        /**
         * Portrait aspect ratio of 9:16
         *
         * This is the portrait aspect ratio of almost all modern smart phones.
         * This includes newer iPhones and Samsung phones.
         */
        PORTRAIT_9_16 = 5,
        /**
         * Portrait aspect ratio of 600:1024
         *
         * This is an unusual portrait aspect ratio for the Kindle Fire tablet
         * and the Samsung Galaxy 7.
         */
        PORTRAIT_600_1024 = 6,
        /**
         * Portrait aspect ratio of 512:683
         *
         * This is a unique portrait aspect ratio for the Apple iPad Pro.
         */
        PORTRAIT_IPAD_PRO = 7,
        /**
         * Landscape aspect ratio of 4:L
         *
         * This is the landscape aspect ratio of most Apple iPad's except for the
         * iPad Pro.  It is also the standard definition TV aspect ratio.
         */
        LANDSCAPE_4_3 = 8,
        /**
         * Landscape aspect ratio of 3:2
         *
         * This is the landscape aspect ratio of older iPhones (before the 4s)
         * and Microsoft Surface 3.
         */
        LANDSCAPE_3_2 = 9,
        /**
         * Landscape aspect ratio of 16:10
         *
         * This is the landscape aspect ratio of 8" and 10" Samsung tablets. It
         * is also the aspect ratio of Apple desktop and notebook displays.
         */
        LANDSCAPE_16_10 = 10,
        /**
         * Landscape aspect ratio of 5:3
         *
         * This is the landscape aspect ratio of Samsung Galaxy S tablets.
         */
        LANDSCAPE_5_3 = 11,
        /**
         * Landscape aspect ratio of 16:9
         *
         * This is the landscape aspect ratio of almost all modern smart phones.
         * This includes newer iPhones and Samsung phones.  It is also the high
         * definition TV aspect ratio.
         */
        LANDSCAPE_16_9 = 12,
        /**
         * Landscape aspect ratio of 1024:600
         *
         * This is an unusual landscape aspect ratio for the Kindle Fire tablet
         * and the Samsung Galaxy 7.
         */
        LANDSCAPE_1024_600 = 13,
        /**
         * Landscape aspect ratio of 683:512
         *
         * This is a unique landscape aspect ratio for the Apple iPad Pro.
         */
        LANDSCAPE_IPAD_PRO = 14,
        /**
         * The landscape aspect ratio is unknown
         *
         * This is an error value for type safety.  It evaluates to an aspect
         * ration of 0.
         */
        UKNOWN = 15
    };

#pragma mark Values
protected:
    /** The display singleton */
    static Display* _thedisplay;
    
    /** The aspect ratio (coded as the enum) */
    Aspect _aspect;

    /** The full screen resolution of this device */
    Rect _bounds;
    /** The full screen resolution minus menu bars and other features */
    Rect _usable;
    /** The pixel density of the device */
    Vec2 _scale;
    

#pragma mark -
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
    Display();

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
    bool init();
    
    /**
     * Uninitializes this object, releasing all resources.
     *
     * This method quits the SDL video system, effectively exitting shutting
     * down the entire program.
     *
     * WARNING: This class is a singleton.  You should never access this
     * method directly.  Use the {@link stop()} method instead.
     */
    void dispose();

    /**
     * Deletes this object, releasing all resources.
     *
     * This method quits the SDL video system, effectively exitting shutting
     * down the entire program.
     *
     * WARNING: This class is a singleton.  You should never access this
     * destructor directly.  Use the {@link stop()} method instead.
     */
    ~Display() { dispose(); }

#pragma mark -
#pragma mark Static Accessors
public:
    /**
     * Starts up the SDL display and video system.
     *
     * This static method needs to be the first line of any application, though
     * it is handled automatically in the {@link Application} class.
     *
     * Once this method is called, the {@link get()} method will no longer
     * return a null value.
     */
    static bool start();

    /**
     * Shuts down the SDL display and video system.
     *
     * This static method needs to be the last line of any application, though
     * it is handled automatically in the {@link Application} class.
     *
     * Once this method is called, the {@link get()} method will return nullptr.
     * More importantly, no SDL function calls will work anymore.
     */
    static void stop();

    /**
     * Returns the singleton instance for the display
     *
     * You must call this static method first to get information about your
     * specific display.  This method will return nullptr until {@link start()}
     * is called first.
     *
     * @return the singleton instance for the display
     */
    static Display* get() { return _thedisplay; }

#pragma mark -
#pragma mark Attributes
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
    const Rect& getBounds() const { return _bounds;   }

    /**
     * Returns the full screen resolution for this display
     *
     * This method returns the bounds for the current resolution, not the
     * maximum resolution.  You should never change the resolution of a display.
     * Allow the user to have their preferred resolution.  Instead, you should
     * adjust your camera to scale the viewport.
     *
     * The value returned represents the value pixels, not points.  This is to
     * help align the results with input devices on Retina displays
     * and other high DPI monitors.
     */
    Rect getPixelBounds() const { return Rect(_bounds.origin*_scale,_bounds.size*_scale);   }
    
    /**
     * Returns the usable full screen resolution for this display
     *
     * Usable is a subjective term defined by the operating system.  In 
     * general, it means the full screen minus any space used by important
     * user interface elements, like a status bar (iPhone), menu bar (OS X),
     * or task bar (Windows).
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
    const Rect& getUsableBounds() const { return _usable;   }
    
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
     * the screen.  In some cases (OS X Retina displays), it refers to the 
     * pixel density of the backing framebuffer, which may be different from
     * the physical framebuffer.
     *
     * @return the number of pixels for each point.
     */
    const Vec2& getPixelDensity() const { return _scale;    }
    
    /**
     * Returns the aspect of this monitor.
     *
     * The aspect is returned as an enum, not a ratio.  Round off error might
     * cause devices with very similar aspect ratios to have slightly different
     * ratio values.  Therefore, the enum is a way of normalizing device aspects.
     *
     * If you would like to know the actual ratio, use the method 
     * {@link getAspectRatio()} instead.  In addition, there are methods for
     * computing width from height and vice versa.
     *
     * Device aspects are relatively standardized.  For information on your
     * device, see
     *
     *      http://mydevice.io/devices/
     *
     * @return the aspect of this monitor.
     */
    Aspect getAspect() const { return _aspect;   }

    /**
     * Returns true if this device has a landscape orientation
     *
     * @return true if this device has a landscape orientation
     */
    bool isLandscape() const {
        return (int)_aspect >= (int)Aspect::LANDSCAPE_4_3;
    }
    
    /**
     * Returns true if this device has a portrait orientation
     *
     * @return true if this device has a portrait orientation
     */
    bool isPortrait() const {
        return ((int)_aspect < (int)Aspect::LANDSCAPE_4_3 &&
                _aspect != Aspect::SQUARE);
    }

#pragma mark -
#pragma mark Aspect Utilities
    /**
     * Returns the device aspect ratio
     *
     * The value is computed width/height.
     *
     * @return the device aspect ratio
     */
    float getAspectRatio() const { return getAspectRatio(_aspect); }

    /**
     * Returns a string representation of the device aspect ratio
     *
     * This value is useful for debugging.  The first part of the string,
     * before the space, is guaranteed to be in the format x:y
     *
     * @return a string representation of the aspect ratio
     */
    const std::string getAspectName() const { return getAspectName(_aspect); }

    /**
     * Returns the closest width value for the device aspect ratio.
     *
     * This value is used when you want to scale a viewpoint to match the
     * display.  The value returned is rounded up to the nearest int, assuming
     * that you want the viewport in points.
     *
     * @param height    The height in points
     *
     * @return the closest width value for the device aspect ratio.
     */
    int widthForHeight(int height) const {
        return (int)(ceilf(getAspectRatio(_aspect)/height));
    }
    
    /**
     * Returns the closest height value for the device aspect ratio.
     *
     * This value is used when you want to scale a viewpoint to match the
     * display.  The value returned is rounded up to the nearest int, assuming
     * that you want the viewport in points.
     *
     * @param width     The width in points
     *
     * @return the closest height value for the device aspect ratio.
     */
    int heightForWidth(int width) const {
        return (int)(ceilf(width/getAspectRatio(_aspect)));
    }

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
    static Aspect getAspect(float ratio);
    
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
    static float getAspectRatio(Aspect aspect);

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
    static const std::string getAspectName(Aspect aspect);

    /**
     * Returns the closest width value for the given aspect
     *
     * This value is used when you want to scale a viewpoint to match the
     * display.  The value returned is rounded up to the nearest int, assuming
     * that you want the viewport in points.
     *
     * @param height    The height in points
     * @param aspect    The device aspect value
     *
     * @return the closest width value for the given aspect
     */
    static int widthForHeight(int height, Aspect aspect) {
        return (int)(ceilf(getAspectRatio(aspect)/height));
    }

    /**
     * Returns the closest height value for the given aspect
     *
     * This value is used when you want to scale a viewpoint to match the
     * display.  The value returned is rounded up to the nearest int, assuming
     * that you want the viewport in points.
     *
     * @param width     The width in points
     * @param aspect    The device aspect value
     *
     * @return the closest height value for the given aspect
     */
    static int heightForWidth(int width, Aspect aspect) {
        return (int)(ceilf(width/getAspectRatio(aspect)));
    }

};

}

#endif /* __CU_DISPLAY_H__ */
