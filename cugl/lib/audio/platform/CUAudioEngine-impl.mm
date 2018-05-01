//
//  CUAudioEngine-impl.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides platform specific support for AudioEngine.  We have
//  two options.  The first is to use SDL Mixer, which works on all platforms
//  but is ancient.  It also gives major deprecation errors for iOS and OS X
//  (and is not safe to use on those platforms).  For those platforms, we use
//  the Apple-specific AVFoundation library.
//
//  On Apple platforms, you can switch between solutions by defining/undefining
//  the CU_AUDIO_AVFOUNDATION compiler variable.
//
//  This file is an internal header.  It is not accessible by general users
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
//  Version: 12/10/16
//
#include "CUAudioEngine-impl.h"
#include <cugl/audio/CUAudioEngine.h>

#if defined CU_AUDIO_AVFOUNDATION
    #include "CUAudioEngine-Apple.mm"
#else
    #include "CUAudioEngine-SDL.cpp"
#endif
