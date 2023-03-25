/*===============================================================================
Copyright (c) 2022 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_AREATARGETOBSERVER_H_
#define _VU_AREATARGETOBSERVER_H_

/**
 * \file AreaTargetObserver.h
 * \brief Observer for the AreaTarget feature
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup AreaTargetObserverGroup Area Target Feature
 * \{
 */

/// \brief Configuration error for Area Target creation
VU_ENUM(VuAreaTargetCreationError)
{
    VU_AREA_TARGET_CREATION_ERROR_NONE = 0x0,     ///< No error
    VU_AREA_TARGET_CREATION_ERROR_INTERNAL = 0x1, ///< An internal error occurred while creating the observer
    VU_AREA_TARGET_CREATION_ERROR_AUTOACTIVATION_FAILED =
        0x2, ///< Observer auto-activation failed (probably due to the total target size limit)
    VU_AREA_TARGET_CREATION_ERROR_FEATURE_NOT_SUPPORTED = 0x3,        ///< Feature not supported (e.g. if Vuforia is not running on a
                                                                      ///< platform-based Fusion Provider such as ARKit, ARCore)
    VU_AREA_TARGET_CREATION_ERROR_INVALID_DEVICE_POSE_OBSERVER = 0x4, ///< Device pose observer is null or invalid
    VU_AREA_TARGET_CREATION_ERROR_DATABASE_LOAD_ERROR = 0x5, ///< Database file not found or an error occurred when reading data from it
                                                             ///< (potentially unknown or corrupted file)
    VU_AREA_TARGET_CREATION_ERROR_INVALID_TARGET_NAME = 0x6, ///< Invalid target name
    VU_AREA_TARGET_CREATION_ERROR_TARGET_NOT_FOUND =
        0x7, ///< Target with the specified name AND matching target type for this observer not found in database
    VU_AREA_TARGET_CREATION_ERROR_REQUIRES_EXTERNAL_POSITIONS = 0x8 ///< This target does not support the default loading and detection
                                                                    ///< behavior. The requireExternalPositions option in VuAreaTargetConfig
                                                                    ///< must be set to VU_TRUE to use this target.
};

/// \brief Configuration for creating an Area Target observer
typedef struct VuAreaTargetConfig
{
    /// \brief Pointer to a device pose observer
    /**
     * The Area Target observer will only report observations to the state while there
     * is an active device pose observer.
     */
    VuObserver* devicePoseObserver;

    /// \brief Path to database containing targets
    const char* databasePath;

    /// \brief Target name
    const char* targetName;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Offset from the origin of the target to the pose reported by an observation, relative to the target's frame of reference
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is identity matrix.
     */
    VuMatrix44F poseOffset;

    /// \brief Require external positions to be set before the observer will begin
    /// tracking or recover from loss of tracking
    /**
     * If this option is set to VU_TRUE, then the target is forced to wait for an
     * external position before loading any target data. This enables simultaneous
     * activation of many targets without the usual restrictions on the total size of
     * the activated targets, since the external position can be used as a hint to
     * limit the data loaded from each target.
     *
     * The limitation of this setting is that it will not be possible to detect the
     * target until a valid external position has been received. This is also the case
     * after an extended loss of tracking, e.g. after pausing the app and moving to a
     * new location.
     *
     * \note The external position should be set using either
     * vuAreaTargetObserverSetExternalPositionFrom2D() or
     * vuAreaTargetObserverSetExternalPositionFrom3D().
     *
     * \note Activation of multiple Area Targets is only possible if this setting is
     * consistent for all the targets being activated.
     *
     * \note Some targets do not support setting this option to VU_FALSE, e.g. targets
     * that are too large to be fully loaded into memory. This will result in a
     * VU_AREA_TARGET_CREATION_ERROR_REQUIRES_EXTERNAL_POSITIONS creation error.
     *
     * \note Area Targets created with Vuforia Engine 9.4 or earlier do not support
     * setting this option to VU_TRUE. This will result in a
     * VU_AREA_TARGET_CREATION_ERROR_INTERNAL creation error.
     *
     * \note This option defaults to VU_FALSE.
     */
    VuBool requireExternalPositions;
} VuAreaTargetConfig;

/// \brief Default Area Target configuration
/**
 * \note Use this function to initialize the VuAreaTargetConfig data structure with default values.
 */
VU_API VuAreaTargetConfig VU_API_CALL vuAreaTargetConfigDefault();

/// \brief Target info for Area Target from its respective observation
typedef struct VuAreaTargetObservationTargetInfo
{
    /// \brief Persistent system-wide unique ID associated with the Area Target
    /**
     * \note The unique ID can't be changed.
     */
    const char* uniqueId;

    /// \brief Target name
    const char* name;

    /// \brief Size (dimensions) of the Area Target in meters
    VuVector3F size;

    /// \brief Axis-aligned bounding box of the observed target, relative to the target's frame of reference
    VuAABB bbox;

    /// \brief Whether the target requires external positions for initial detection
    VuBool requiresExternalPositions;

    /// \brief Pose offset used with the Area Target
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     */
    VuMatrix44F poseOffset;
} VuAreaTargetObservationTargetInfo;

/// \brief status info for an Area Target observation
VU_ENUM(VuAreaTargetObservationStatusInfo)
{
    VU_AREA_TARGET_OBSERVATION_STATUS_INFO_NORMAL = 0x1,       ///< Tracking working normally
    VU_AREA_TARGET_OBSERVATION_STATUS_INFO_NOT_OBSERVED = 0x2, ///< Target is not detected
    VU_AREA_TARGET_OBSERVATION_STATUS_INFO_RELOCALIZING = 0x3, ///< The tracking system is currently relocalizing
    VU_AREA_TARGET_OBSERVATION_STATUS_INFO_MISSING_DATA =
        0x4 ///< Asynchronous loading of the tracking data failed. Tracking quality might decrease or the tracking might be lost.
};

/*! \var VU_OBSERVER_AREA_TARGET_TYPE VU_OBSERVER_AREA_TARGET_TYPE
 * \brief Type identifier for Area Target observers
 */
VU_CONST_INT(VU_OBSERVER_AREA_TARGET_TYPE, 0x7);

/*! \var VU_OBSERVATION_AREA_TARGET_TYPE VU_OBSERVATION_AREA_TARGET_TYPE
 * \brief Type identifier for Area Target observations
 */
VU_CONST_INT(VU_OBSERVATION_AREA_TARGET_TYPE, 0x7);

/// \brief Create an Area Target observer from database
/**
 * \note Note that loading the database may take a significant amount of time, it is therefore recommended that this method is not called on
 * the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuEngineCreateAreaTargetObserver(VuEngine* engine, VuObserver** observer, const VuAreaTargetConfig* config,
                                                             VuAreaTargetCreationError* errorCode);

/// \brief Get all Area Target observers
VU_API VuResult VU_API_CALL vuEngineGetAreaTargetObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get the unique ID associated to the target from an Area Target observer
VU_API VuResult VU_API_CALL vuAreaTargetObserverGetTargetUniqueId(const VuObserver* observer, const char** targetId);

/// \brief Get the name associated to the target from an Area Target observer
VU_API VuResult VU_API_CALL vuAreaTargetObserverGetTargetName(const VuObserver* observer, const char** targetName);

/// \brief Get the size in meters associated to the target from an Area Target observer
VU_API VuResult VU_API_CALL vuAreaTargetObserverGetTargetSize(const VuObserver* observer, VuVector3F* size);

/// \brief Get the pose transformation offset associated to the target from an Area Target observer
/**
 * \note The offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuAreaTargetObserverGetTargetPoseOffset(const VuObserver* observer, VuMatrix44F* offset);

/// \brief Set the pose transformation offset associated to the target from an Area Target observer
/**
 * \note The offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuAreaTargetObserverSetTargetPoseOffset(VuObserver* observer, const VuMatrix44F* poseOffset);

/// \brief Get the axis-aligned bounding box associated to the target from an Area Target observer, relative to the target's frame of
/// reference
VU_API VuResult VU_API_CALL vuAreaTargetObserverGetAABB(const VuObserver* observer, VuAABB* bbox);

/// \brief Set approximate 2D position within an Area Target from an external localization source (DEPRECATED)
/**
 * \deprecated This function has been deprecated. It will be removed in an
 * upcoming %Vuforia release. Please use vuAreaTargetObserverSetExternalPositionFrom2D()
 * instead.
 *
 * Setting up the external position only succeeds when the target is activated.
 *
 * \note The provided external position is valid until a successful localization,
 * target deactivation, or updating the external position by a repeated call to
 * this function or any of the other vuAreaTargetObserverSetExternalPosition
 * functions (whichever happens first).
 *
 * \param observer Observer handle
 * \param position Position (x, z) in target coordinate system (which is y-up).
 * \param horizontalAccuracy Horizontal accuracy (radial) of the position in meters. Must be positive.
 * \returns VU_SUCCESS on success, VU_FAILED on failure.
 */
VU_API VuResult VU_API_CALL vuAreaTargetObserverSetExternalPosition(VuObserver* observer, VuVector2F position, float horizontalAccuracy);

/// \brief Set approximate 2D position within an Area Target from an external localization source
/**
 * Setting up the external position only succeeds when the target is activated.
 *
 * \note The provided external position is valid until a successful localization,
 * target deactivation, or updating the external position by a repeated call to
 * this function or any of the other vuAreaTargetObserverSetExternalPosition
 * functions (whichever happens first).
 *
 * \param observer Observer handle
 * \param position Position (x, z) in target coordinate system (which is y-up).
 * \param horizontalAccuracy Horizontal accuracy (radial) of the position in meters. Must be positive.
 * \returns VU_SUCCESS on success, VU_FAILED on failure.
 */
VU_API VuResult VU_API_CALL vuAreaTargetObserverSetExternalPositionFrom2D(VuObserver* observer, VuVector2F position,
                                                                          float horizontalAccuracy);

/// \brief Set approximate 3D position within an Area Target from an external localization source
/**
 * Setting up the external position only succeeds when the target is activated.
 *
 * \note The provided external position is valid until a successful localization,
 * target deactivation, or updating the external position by a repeated call to
 * this function or any of the other vuAreaTargetObserverSetExternalPosition
 * functions (whichever happens first).
 *
 * \param observer Observer handle
 * \param position Position (x, y, z) in target coordinate system (which is y-up).
 * \param horizontalAccuracy Horizontal accuracy (radial) of the position in meters. Must be positive.
 * \param verticalAccuracy Vertical accuracy of the position in meters. Must be positive.
 * \returns VU_SUCCESS on success, VU_FAILED on failure.
 */
VU_API VuResult VU_API_CALL vuAreaTargetObserverSetExternalPositionFrom3D(VuObserver* observer, VuVector3F position,
                                                                          float horizontalAccuracy, float verticalAccuracy);

/// \brief Check whether the target requires external positions for initial detection
/**
 * Checks the value of the requireExternalPositions option in VuAreaTargetConfig. If
 * a target requires external positions, then it will not be detected until an external
 * position is provided. This is also the case after an extended loss of tracking, e.g.
 * after pausing the app and moving to a new location.
 *
 * \note The external position should be set using either
 * vuAreaTargetObserverSetExternalPositionFrom2D() or
 * vuAreaTargetObserverSetExternalPositionFrom3D().
 *
 * \param observer Observer handle
 * \param requiresExternalPositions Gives VU_TRUE if the target needs external positions
 * for initial detection or after extended loss of tracking, VU_FALSE otherwise.
 * \returns VU_SUCCESS on success, VU_FAILED on failure.
 */
VU_API VuResult VU_API_CALL vuAreaTargetObserverRequiresExternalPositions(const VuObserver* observer, VuBool* requiresExternalPositions);

/// \brief Get all Area Target observations
VU_API VuResult VU_API_CALL vuStateGetAreaTargetObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get status info associated with an Area Target observation
VU_API VuResult VU_API_CALL vuAreaTargetObservationGetStatusInfo(const VuObservation* observation,
                                                                 VuAreaTargetObservationStatusInfo* statusInfo);

/// \brief Get target info associated with an Area Target observation
VU_API VuResult VU_API_CALL vuAreaTargetObservationGetTargetInfo(const VuObservation* observation,
                                                                 VuAreaTargetObservationTargetInfo* targetInfo);

/** \} */

/** \addtogroup MeshObserverGroup Mesh Feature
 * \{
 */

/// \brief Configuration error for Mesh observer creation with Area Target observer
VU_ENUM(VuMeshAreaTargetCreationError)
{
    VU_MESH_AREA_TARGET_CREATION_ERROR_NONE = 0x00,                  ///< No error
    VU_MESH_AREA_TARGET_CREATION_ERROR_INTERNAL = 0x01,              ///< An internal error occurred while creating the observer
    VU_MESH_AREA_TARGET_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x02, ///< An error occurred while auto-activating the observer
    VU_MESH_AREA_TARGET_CREATION_ERROR_INVALID_OBSERVER = 0x03,      ///< Area Target observer is NULL or invalid
    VU_MESH_AREA_TARGET_CREATION_ERROR_MESH_FILE_LOAD_ERROR =
        0x04, ///< External occlusion mesh file not found or an error occurred when reading data from it
              ///< (potentially unknown or corrupted file).
              ///< \deprecated This error is DEPRECATED, use VU_MESH_AREA_TARGET_CREATION_ERROR_MESH_LOAD_ERROR.
    VU_MESH_AREA_TARGET_CREATION_ERROR_MESH_LOAD_ERROR = 0x04,          ///< Failed to load mesh data
    VU_MESH_AREA_TARGET_CREATION_ERROR_SAME_SOURCE_NOT_SUPPORTED = 0x05 ///< A mesh observer is already attached to the Area Target observer
};

/// \brief Configuration for creating a Mesh observer associated with an Area Target observer
typedef struct VuMeshAreaTargetConfig
{
    /// \brief Pointer to Area Target observer
    /**
     * The Area Target observer is the exclusive source of the Mesh observations
     * that are reported by a Mesh observer created with this configuration.
     * The reported Mesh obervations provide an appropriate level of detail for the
     * current position within the tracked Area Target. Therefore, the Mesh observations
     * might change depending on the device's position within the target.
     */
    VuObserver* areaTargetObserver;

    /// \brief Path to the external occlusion mesh artifact (occlusion.3dt)
    /**
     * The external occlusion mesh artifact **must** originate from the Area Target
     * that is used for the Area Target observer.
     *
     * \note The provided string is copied, and can be freed, after the Mesh observer is created.
     */
    const char* occlusionMeshPath;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;
} VuMeshAreaTargetConfig;

/// \brief Default Mesh observer configuration with Area Target observer
/**
 * \note Use this function to initialize the VuMeshAreaTargetConfig data structure with default values.
 */
VU_API VuMeshAreaTargetConfig VU_API_CALL vuMeshAreaTargetConfigDefault();

/// \brief Create a Mesh observer with an Area Target observer as source
/**
 * The Mesh observer will deliver Mesh observations that represent a mesh reconstruction of the target
 * tracked by the Area Target observer. Because the Mesh observer is dependent on the
 * existence of the Area Target observer, one has to make sure to destroy the Mesh observer before
 * the associated Area Target observer is destroyed.
 */
VU_API VuResult VU_API_CALL vuEngineCreateMeshObserverFromAreaTargetConfig(VuEngine* engine, VuObserver** observer,
                                                                           const VuMeshAreaTargetConfig* config,
                                                                           VuMeshAreaTargetCreationError* errorCode);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_AREATARGETOBSERVER_H_
