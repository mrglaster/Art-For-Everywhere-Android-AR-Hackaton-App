/*===============================================================================
Copyright (c) 2021 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_CYLINDERTARGETOBSERVER_H_
#define _VU_CYLINDERTARGETOBSERVER_H_

/**
 * \file CylinderTargetObserver.h
 * \brief Observer for the Cylinder Target feature
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup CylinderTargetObserverGroup Cylinder Target Feature
 * \{
 */

/// \brief Configuration error for Cylinder Target creation
VU_ENUM(VuCylinderTargetCreationError)
{
    VU_CYLINDER_TARGET_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_CYLINDER_TARGET_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_CYLINDER_TARGET_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< Observer auto-activation failed
    VU_CYLINDER_TARGET_CREATION_ERROR_DATABASE_LOAD_ERROR = 0x3, ///< Database file not found or an error occurred when reading data from it
                                                                 ///< (potentially unknown or corrupted file)
    VU_CYLINDER_TARGET_CREATION_ERROR_INVALID_TARGET_NAME = 0x4, ///< Invalid target name
    VU_CYLINDER_TARGET_CREATION_ERROR_TARGET_NOT_FOUND =
        0x5, ///< Target with the specified name AND matching target type for this observer not found in database
    VU_CYLINDER_TARGET_CREATION_ERROR_INVALID_SCALE = 0x6 ///< Invalid value passed to the scale parameter
};

/// \brief Configuration for creating a Cylinder Target observer
typedef struct VuCylinderTargetConfig
{
    /// \brief Path to database containing targets
    const char* databasePath;

    /// \brief Target name
    const char* targetName;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Scale multiplication factor
    /**
     * \note The default value is 1.0f.
     */
    float scale;

    /// \brief Offset from the origin of the target to the pose reported by an observation, relative to the target's frame of reference
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuCylinderTargetConfig;

/// \brief Default Cylinder Target configuration
/**
 * \note Use this function to initialize the VuCylinderTargetConfig data structure with default values.
 */
VU_API VuCylinderTargetConfig VU_API_CALL vuCylinderTargetConfigDefault();

/// \brief Target info for a Cylinder Target from its respective observation
typedef struct VuCylinderTargetObservationTargetInfo
{
    /// \brief Persistent system-wide unique ID associated with the Cylinder Target
    /**
     * \note The unique ID can't be changed.
     */
    const char* uniqueId;

    /// \brief Target name
    const char* name;

    /// \brief Side length of the observed target in meters
    float sideLength;

    /// \brief Top diameter of the observed target in meters
    float topDiameter;

    /// \brief Bottom diameter of the observed target in meters
    float bottomDiameter;

    /// \brief Axis-aligned bounding box of the observed target, relative to the target's frame of reference
    VuAABB bbox;

    /// \brief Pose offset used with the observed target
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     */
    VuMatrix44F poseOffset;
} VuCylinderTargetObservationTargetInfo;

/// \brief status info for Cylinder Target observation
VU_ENUM(VuCylinderTargetObservationStatusInfo)
{
    VU_CYLINDER_TARGET_OBSERVATION_STATUS_INFO_NORMAL = 0x1,       ///< Tracking working normally
    VU_CYLINDER_TARGET_OBSERVATION_STATUS_INFO_NOT_OBSERVED = 0x2, ///< Target is not detected
    VU_CYLINDER_TARGET_OBSERVATION_STATUS_INFO_RELOCALIZING = 0x3  ///< The tracking system is currently relocalizing
};

/*! \var VU_OBSERVER_CYLINDER_TARGET_TYPE VU_OBSERVER_CYLINDER_TARGET_TYPE
 * \brief Type identifier for Cylinder Target observers
 */
VU_CONST_INT(VU_OBSERVER_CYLINDER_TARGET_TYPE, 0x2);

/*! \var VU_OBSERVATION_CYLINDER_TARGET_TYPE VU_OBSERVATION_CYLINDER_TARGET_TYPE
 * \brief Type identifier for Cylinder Target observations
 */
VU_CONST_INT(VU_OBSERVATION_CYLINDER_TARGET_TYPE, 0x2);

/// \brief Create a Cylinder Target observer from a database
/**
 * \note Note that loading the database may take a significant amount of time, it is therefore recommended that this method is not called on
 * the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuEngineCreateCylinderTargetObserver(VuEngine* engine, VuObserver** observer,
                                                                 const VuCylinderTargetConfig* config,
                                                                 VuCylinderTargetCreationError* errorCode);

/// \brief Get all Cylinder Target observers
VU_API VuResult VU_API_CALL vuEngineGetCylinderTargetObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get the unique ID associated to the target from a Cylinder Target observer
VU_API VuResult VU_API_CALL vuCylinderTargetObserverGetTargetUniqueId(const VuObserver* observer, const char** targetId);

/// \brief Get the name associated to the target from a Cylinder Target observer
VU_API VuResult VU_API_CALL vuCylinderTargetObserverGetTargetName(const VuObserver* observer, const char** targetName);

/// \brief Get the side length in meters associated to the target from a Cylinder Target observer
VU_API VuResult VU_API_CALL vuCylinderTargetObserverGetTargetSideLength(const VuObserver* observer, float* sideLength);

/// \brief Set the side length in meters associated to the target from a Cylinder Target observer
VU_API VuResult VU_API_CALL vuCylinderTargetObserverSetTargetSideLength(VuObserver* observer, float sideLength);

/// \brief Get the top diameter in meters associated to the target from a Cylinder Target observer
VU_API VuResult VU_API_CALL vuCylinderTargetObserverGetTargetTopDiameter(const VuObserver* observer, float* topDiameter);

/// \brief Set the top diameter in meters associated to the target from a Cylinder Target observer
VU_API VuResult VU_API_CALL vuCylinderTargetObserverSetTargetTopDiameter(VuObserver* observer, float topDiameter);

/// \brief Get the bottom diameter in meters associated to the target from a Cylinder Target observer
VU_API VuResult VU_API_CALL vuCylinderTargetObserverGetTargetBottomDiameter(const VuObserver* observer, float* bottomDiameter);

/// \brief Set the bottom diameter in meters associated to the target from a Cylinder Target observer
VU_API VuResult VU_API_CALL vuCylinderTargetObserverSetTargetBottomDiameter(VuObserver* observer, float bottomDiameter);

/// \brief Get the axis-aligned bounding box associated to the target from a Cylinder Target observer, relative to the target's frame of
/// reference
VU_API VuResult VU_API_CALL vuCylinderTargetObserverGetAABB(const VuObserver* observer, VuAABB* bbox);

/// \brief Get the pose transformation offset associated to the target from a Cylinder Target observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuCylinderTargetObserverGetTargetPoseOffset(const VuObserver* observer, VuMatrix44F* poseOffset);

/// \brief Set the pose transformation offset associated to the target from a Cylinder Target observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuCylinderTargetObserverSetTargetPoseOffset(VuObserver* observer, const VuMatrix44F* poseOffset);

/// Set the tracking optimization of the target associated to the Cylinder Target observer.
/**
 * This setting modifies the internal target tracking parameters to optimize the tracking quality and robustness.
 *
 * \note Only VU_TRACKING_OPTIMIZATION_DEFAULT and VU_TRACKING_OPTIMIZATION_AR_CONTROLLER
 * are allowed. This function will fail if VU_TRACKING_OPTIMIZATION_LOW_FEATURE_OBJECTS is provided.
 *
 * \note This operation will reset any tracking operation for the Cylinder Target observer.
 * It is recommended to use this function before starting the Vuforia Engine.
 */
VU_API VuResult VU_API_CALL vuCylinderTargetObserverSetTrackingOptimization(VuObserver* observer, VuTrackingOptimization optimization);

/// Get the tracking optimization of the target associated to the Cylinder Target observer.
VU_API VuResult VU_API_CALL vuCylinderTargetObserverGetTrackingOptimization(const VuObserver* observer,
                                                                            VuTrackingOptimization* optimization);

/// \brief Get all Cylinder Target observations
VU_API VuResult VU_API_CALL vuStateGetCylinderTargetObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get status info associated with a Cylinder Target observation
VU_API VuResult VU_API_CALL vuCylinderTargetObservationGetStatusInfo(const VuObservation* observation,
                                                                     VuCylinderTargetObservationStatusInfo* statusInfo);

/// \brief Get target info associated with a Cylinder Target observation
VU_API VuResult VU_API_CALL vuCylinderTargetObservationGetTargetInfo(const VuObservation* observation,
                                                                     VuCylinderTargetObservationTargetInfo* targetInfo);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_CYLINDERTARGETOBSERVER_H_
