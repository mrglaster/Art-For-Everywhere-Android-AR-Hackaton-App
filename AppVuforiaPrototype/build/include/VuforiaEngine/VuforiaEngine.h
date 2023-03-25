/*===============================================================================
Copyright (c) 2022 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_VUFORIAENGINE_H_
#define _VU_VUFORIAENGINE_H_

/**
 * \file VuforiaEngine.h
 * \brief Umbrella header for the Vuforia Engine
 */

// ======== VUFORIA ENGINE VERSION INFORMATION ========

/// \brief Utility macro to support the comparison of Vuforia Engine versions
#define VU_MAKE_VERSION(major, minor, patch) \
    (((major) << 22) | ((minor) << 12) | (patch))

/// \brief Vuforia Engine major version
#define VU_VERSION_MAJOR 10

/// \brief Vuforia Engine minor version
#define VU_VERSION_MINOR 13

/// \brief Vuforia Engine patch version
#define VU_VERSION_PATCH 3

/// \brief Vuforia Engine version as a single integer
/**
 * \note You can use the version-related macros for conditional code compilation,
 * for example to build code only when the Engine version is greater than or
 * equals to 10.8.0, use
 *
 *     #if VU_VERSION >= VU_MAKE_VERSION(10, 8, 0)
 *         // Vuforia Engine version-specific code
 *     #endif
 */
#define VU_VERSION VU_MAKE_VERSION(VU_VERSION_MAJOR, VU_VERSION_MINOR, VU_VERSION_PATCH)

// ======== VUFORIA ENGINE API HEADER FILES ========

// Core data types and defines
#include <VuforiaEngine/Core/Core.h>

// Database content information
#include <VuforiaEngine/Core/Database.h>

// Linear algebra functions
#include <VuforiaEngine/Core/MathUtils.h>

// Core Engine lifecycle management and configuration
#include <VuforiaEngine/Engine/Engine.h>

// Engine configuration
#include <VuforiaEngine/Engine/DriverConfig.h>
#include <VuforiaEngine/Engine/FusionProviderConfig.h>
#include <VuforiaEngine/Engine/LicenseConfig.h>
#include <VuforiaEngine/Engine/RenderConfig.h>

// Platform-specific Engine configuration
#if defined(VU_PLATFORM_ANDROID)
#   include <VuforiaEngine/Engine/Android/PlatformConfig_Android.h>
#elif defined(VU_PLATFORM_LUMIN)
#   include <VuforiaEngine/Engine/Lumin/PlatformConfig_Lumin.h>
#endif

// Observers
#include <VuforiaEngine/Observer/AnchorObserver.h>
#include <VuforiaEngine/Observer/AreaTargetObserver.h>
#include <VuforiaEngine/Observer/CloudImageTargetObserver.h>
#include <VuforiaEngine/Observer/CylinderTargetObserver.h>
#include <VuforiaEngine/Observer/DevicePoseObserver.h>
#include <VuforiaEngine/Observer/IlluminationObserver.h>
#include <VuforiaEngine/Observer/ImageTargetObserver.h>
#include <VuforiaEngine/Observer/MeshObserver.h>
#include <VuforiaEngine/Observer/ModelTargetObserver.h>
#include <VuforiaEngine/Observer/MultiTargetObserver.h>
#include <VuforiaEngine/Observer/VirtualButtonObserver.h>
#include <VuforiaEngine/Observer/VuMarkObserver.h>
#include <VuforiaEngine/Observer/BarcodeObserver.h>

// Controllers
#include <VuforiaEngine/Controller/CameraController.h>
#include <VuforiaEngine/Controller/PlatformController.h>
#include <VuforiaEngine/Controller/RenderController.h>
#include <VuforiaEngine/Controller/SessionRecorderController.h>
#include <VuforiaEngine/Controller/AreaTargetCaptureController.h>

// Platform-specific controllers
#if defined(VU_PLATFORM_ANDROID)
#   include <VuforiaEngine/Controller/Android/PlatformController_Android.h>
#elif defined(VU_PLATFORM_IOS)
#   include <VuforiaEngine/Controller/iOS/PlatformController_iOS.h>
#elif defined(VU_PLATFORM_UWP)
#   include <VuforiaEngine/Controller/UWP/PlatformController_UWP.h>
#endif

#endif // _VU_VUFORIAENGINE_H_
