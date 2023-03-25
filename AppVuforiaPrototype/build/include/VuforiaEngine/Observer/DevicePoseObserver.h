/*===============================================================================
Copyright (c) 2021 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_DEVICEPOSEOBSERVER_H_
#define _VU_DEVICEPOSEOBSERVER_H_

/**
 * \file DevicePoseObserver.h
 * \brief Observer for tracking the device pose (positional device tracker)
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup DevicePoseObserverGroup Device Pose Feature
 * \{
 * An activated Device Pose observer will always output a reason for an observation with
 * a NO_POSE status in its status info.
 */

/// \brief Configuration error for device pose creation
VU_ENUM(VuDevicePoseCreationError)
{
    VU_DEVICE_POSE_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_DEVICE_POSE_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_DEVICE_POSE_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< An error occurred while auto-activating the observer
    VU_DEVICE_POSE_CREATION_ERROR_FEATURE_NOT_SUPPORTED = 0x3  ///< Device tracking functionality is not supported on the current device
};

/// \brief Configuration for creating a device pose observer
typedef struct VuDevicePoseConfig
{
    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Observer mode
    /**
     * Activate static usage mode of the device pose: in this case the pose will be set to identity.
     * You can change that after creation by calling vuDevicePoseObserverSetStaticMode().
     * \note The default value is VU_FALSE.
     *
     * \note When static mode is on, this configures the device pose for statically mounted devices,
     * e.g. a mobile device on a tripod looking at an object. In such a scenario, standard Vuforia Fusion tracking will
     * not succeed to initialize for lack of movement. Use the staticMode in this setting to configure the device tracker
     * to return static poses to stabilize tracking e.g. in case of the object being occluded.
     */
    VuBool staticMode;
} VuDevicePoseConfig;

/// \brief Default Device Pose configuration
/**
 * \note Use this function to initialize the VuDevicePoseConfig data structure with default values.
 */
VU_API VuDevicePoseConfig VU_API_CALL vuDevicePoseConfigDefault();

/// \brief Status info for device pose observation
VU_ENUM(VuDevicePoseObservationStatusInfo)
{
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_NORMAL = 0x1,                ///< Tracking working normally
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_NOT_OBSERVED = 0x2,          ///< Device pose is not detected
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_UNKNOWN = 0x3,               ///< Unknown reason for LIMITED pose status
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_INITIALIZING = 0x4,          ///< The tracking system is currently initializing
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_RELOCALIZING = 0x5,          ///< The tracking system is currently relocalizing
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_EXCESSIVE_MOTION = 0x6,      ///< The device is moving too fast. This will only appear when the
                                                                        ///< device pose is in Limited status
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_INSUFFICIENT_FEATURES = 0x7, ///< There are insufficient features available in the scene. This
                                                                        ///< will only appear when the device pose is in Limited status
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_INSUFFICIENT_LIGHT = 0x8 ///< Not enough light for accurate tracking. This will only appear when
                                                                    ///< the device pose is in Limited status
};

/*! \var VU_OBSERVER_DEVICE_POSE_TYPE VU_OBSERVER_DEVICE_POSE_TYPE
 * \brief Type identifier for device pose observers
 */
VU_CONST_INT(VU_OBSERVER_DEVICE_POSE_TYPE, 0x8);

/*! \var VU_OBSERVATION_DEVICE_POSE_TYPE VU_OBSERVATION_DEVICE_POSE_TYPE
 * \brief Type identifier for device pose observations
 */
VU_CONST_INT(VU_OBSERVATION_DEVICE_POSE_TYPE, 0x8);

/// \brief Create a device pose observer
VU_API VuResult VU_API_CALL vuEngineCreateDevicePoseObserver(VuEngine* engine, VuObserver** observer, const VuDevicePoseConfig* config,
                                                             VuDevicePoseCreationError* errorCode);

/// \brief Get all device pose observers
VU_API VuResult VU_API_CALL vuEngineGetDevicePoseObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get all device pose observations
VU_API VuResult VU_API_CALL vuStateGetDevicePoseObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get status info associated with a device pose observation
VU_API VuResult VU_API_CALL vuDevicePoseObservationGetStatusInfo(const VuObservation* observation,
                                                                 VuDevicePoseObservationStatusInfo* statusInfo);

/// \brief Set the device pose to static
/**
 * Configures the device pose for statically mounted devices, e.g. a mobile device on a tripod looking at an object.
 * In such a scenario, standard Vuforia Fusion tracking will not succeed to initialize for lack of movement.
 * This setting configures the device tracker to return static poses to stabilize tracking e.g. in case of the object being occluded.
 *
 * \note Changing the static mode will internally also trigger a device tracking reset and re-initialization. All device pose observations
 * are reset to VU_OBSERVATION_POSE_STATUS_NO_POSE and tracking is lost on any targets tracked by extended tracking. Any anchors created
 * during the session are destroyed.
 */
VU_API VuResult VU_API_CALL vuDevicePoseObserverSetStaticMode(VuObserver* observer, VuBool staticModeEnabled);

/// \brief Get if the device pose is set to static
VU_API VuResult VU_API_CALL vuDevicePoseObserverGetStaticMode(VuObserver* observer, VuBool* staticModeEnabled);

/// \brief Reset world tracking
/**
 * \note This resets and re-initializes device tracking. All device pose observations are reset to
 * VU_OBSERVATION_POSE_STATUS_NO_POSE and tracking is lost on any targets tracked by
 * extended tracking. Any anchors created during the session are destroyed.
 */
VU_API VuResult VU_API_CALL vuEngineResetWorldTracking(VuEngine* engine);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_DEVICEPOSEOBSERVER_H_
