/*===============================================================================
Copyright (c) 2021 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_PLATFORMCONTROLLER_ANDROID_H_
#define _VU_PLATFORMCONTROLLER_ANDROID_H_

/**
 * \file PlatformController_Android.h
 * \brief Android-specific functionality for the Vuforia Engine
 */

#include <VuforiaEngine/Core/Core.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup PlatformAndroidControllerGroup Android-specific Platform Controller
 * \{
 * Android platform-specific functionality accessed via the PlatformController
 */

/// \brief ARCore-specific info for the platform-based Vuforia Fusion Provider
/**
 * \note The pointers contained in this data structure are owned by Vuforia Engine and should
 * be used with caution by the developer. For example do not release the session, do not pause
 * the session, do not reconfigure it, doing so will cause Vuforia Engine's handling of the information
 * from the provider to fail in undefined ways.
 *
 * A valid value for the session will be available after Vuforia Engine has been started and it
 * will remain valid until Vuforia Engine is stopped.
 *
 * A valid value for the frame will be available after Vuforia Engine has been started and the
 * Vuforia State contains camera frame data. The value for the frame is valid only for the duration
 * of one frame, thus the caller needs to request the information for every new frame.
 */
typedef struct VuPlatformARCoreInfo
{
    /// \brief ARCore session, pointer of type "ArSession"
    /**
     * The caller needs to cast the arSession pointer to the appropriate type as follows:
     * ArSession* session = static_cast<ArSession*>(info.arSession);
     */
    void* arSession;

    /// \brief ARCore frame, pointer of type "ArFrame"
    /**
     * The caller needs to cast the arFrame pointer to the appropriate type as follows:
     * ArFrame* frame = static_cast<ArFrame*>(info.arFrame);
     *
     * Do not update the ArSession to get the ArFrame, doing so will cause
     * Vuforia Engine to enter an undefined state.
     */
    void* arFrame;
} VuPlatformARCoreInfo;

/// \brief Get information about the ARCore Fusion Provider Platform
/**
 * The information contained in the returned struct can be used to allow applications to interact with
 * the underlying ARCore session to gain access to functionality not currently available through the
 * Vuforia API. For example additional lighting information or plane boundaries.
 *
 * \note Call this function after Vuforia Engine has been started and the Vuforia State
 * contains a camera frame.
 *
 * \param controller Platform controller retrieved from Engine (see vuEngineGetPlatformController)
 * \param arCoreInfo ARCore-specific info for the platform-based Vuforia Fusion Provider
 * \returns VU_FAILED if Vuforia is not running with the ARCore Fusion Provider Platform, VU_SUCCESS otherwise
 */
VU_API VuResult VU_API_CALL vuPlatformControllerGetARCoreInfo(const VuController* controller, VuPlatformARCoreInfo* arCoreInfo);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_PLATFORMCONTROLLER_ANDROID_H_
