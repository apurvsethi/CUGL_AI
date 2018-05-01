//
//  CUDisplay-impl.h
//  Cornell University Game Library (CUGL)
//
//  The SDL display information tools fail on OS X and iOS.  Therefore, we have
//  factored this information out into platform specific files.  This header
//  is the uniform header for all platforms.
//
//  This header is an internal header.  It is not accessible by general users
//  of the CUGL API.
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

#ifndef __CU_DISPLAY_IMPL_H__
#define __CU_DISPLAY_IMPL_H__

#include <cugl/math/CURect.h>
#include <cugl/math/CUVec2.h>

namespace cugl {
namespace impl {
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
    cugl::Rect DisplayBounds();
    
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
    cugl::Rect DisplayUsableBounds();

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
    cugl::Vec2 DisplayPixelDensity();
}
}
#endif /* __CU_DISPLAY_IMPL_H__ */
