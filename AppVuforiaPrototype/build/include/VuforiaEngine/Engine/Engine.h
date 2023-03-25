/*===============================================================================
Copyright (c) 2021 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_ENGINE_H_
#define _VU_ENGINE_H_

/**
 * \file Engine.h
 * \brief Data structures, data types, constants and functions related to the configuration and lifecycle management of core Engine objects
 */

#include <VuforiaEngine/Core/Core.h>

#ifdef __cplusplus
extern "C"
{
#endif

// ======== FUNDAMENTAL VUFORIA API CONCEPTS ========

/** \addtogroup APIConceptsGroup API Concepts
 * \{
 *
 * \b General
 *
 * Vuforia does not promise any ABI compatibility between releases. Developers are expected to recompile the code with the new
 * headers. We do not support just exchanging the binaries.
 *
 * Developers are advised to follow best practices documented in our API and sample application code. As an example, developers
 * should use the struct initializer functions such as vuConfigDefault() to initialize observer configuration
 * instead of directly initializing these structs through aggregate initialization.
 *
 * \b Syntax
 *
 * All types, data structures, functions start with a \b Vu prefix using the following notation:
 * - \b vuFunctionName for functions and procedures
 * - \b VuType for type (e.g. public typedef, opaque type, enum, struct, etc)
 * - \b VU_VALUE for any enum values or define values
 *
 * \b Type
 *
 * Most objects in the Vuforia Engine API are represented by pointers (handles) to opaque struct types. The majority of functions
 * in Vuforia Engine operate on these opaque pointers to objects. The general pattern is thereby that the opaque pointer
 * is the first parameter for the respective functions related to this type (equivalent to the 'this' pointer in C++).
 * The function will be prefixed with the respective type. For example a \b vuEngine...() function will always take a opaque
 * pointer to a VuEngine instance as the first parameter.
 *
 * Additionally the API also contains data structures in the form of structs which are mainly used for providing configuration
 * or for retrieving properties of the opaque types.
 *
 * For container data types such as a list, you need to use a dedicated API for creating, destroying, accessing elements,
 * e.g. for a list containing elements of type "Object" use
 *
 * - vuObjectListCreate(ObjectList** list)
 * - vuObjectListDestroy(ObjectList* list)
 * - vuMyObjectListGetSize(const ObjectList* list, int32_t* listSize)
 * - vuObjectListGetElement(const ObjectList* list, int32_t elementIndex, Object** element)
 *
 * Please be aware of padding when using a Vuforia structure directly if you want to wrap the API in another
 * language.
 *
 * \b Functions
 *
 * All functions use the \b VU_API preprocessor macro for export and \b VU_API_CALL for the calling convention.
 * Function syntax uses the following convention:
 *
 * \b return_value \b vuObjectTypeAction(ObjectType* \b , \b [param]) when you want to execute the action on an object of type ObjectType.
 *
 * Common actions are Create, Destroy, Get, Set, Acquire, Release.
 *
 * Return value:
 * - Most functions return VuResult to indicate execution success / failure
 * - Some utility functions (e.g. math) will return a value that allows chaining such calls easily (e.g. mathematical expression)
 *
 * Parameters:
 * - First parameter is usually a handle to an object (exceptions include utility functions)
 * - Output parameters are at the end of the parameter list
 *
 * Please note that the API does not perform any sanity checks to catching passing NULL or invalid values (e.g. already
 * freed memory).
 *
 * Some of the Get() functions have an associated Has() function that should be used prior to Get() to test whether
 * there is a valid object instance to retrieve in case the returned object is optional or not yet available. If there is
 * no valid instance, then the getter function will return VU_FAILED. For examples, see e.g. vuObservationHasPoseInfo() and
 * vuObservationGetPoseInfo().
 *
 * \b Ownership
 *
 * The objects managed by Engine generally have a hierarchical ownership relation. Objects retrieved via Get() functions
 * are owned by the object they are retrieved from.
 *
 * For example, in order to get the image data for the current camera frame, you need to first retrieve a VuCameraFrame
 * from a VuState by calling vuStateGetCameraFrame(), and then retrieve a VuImageList from the VuCameraFrame
 * via vuCameraFrameGetImages() which provides access the VuImage instances of the frame. The lifetime of the VuImage
 * instances retrieved via the VuImageList will be bound to the lifetime of the VuCameraFrame instance, which in
 * turn will be bound to the lifetime of the VuState instance. If the lifetime of the VuState ends, all pointers
 * to objects directly or indirectly owned by the VuState instance will be invalidated throughout this ownership
 * hierarchy and should not be accessed after this point.
 *
 * Similarly, output buffer data retrieved via Get() functions, such as strings (const char*) or general memory buffers
 * (e.g. void*), will be bound to the lifetime of the object they have been retrieved from.
 *
 * Note that the ownership relation also extends to string or buffer pointers returned as members of non-opaque structs.
 * For example, when retrieving information about a VuImage via VuImageInfo, the lifetime of the buffer pointer member of
 * the VuImageInfo struct is bound to the lifetime of the VuImage instance the VuImageInfo was retrieved from.
 *
 * In general, const char* string data provided as an input parameter will be copied, meaning the buffer can be freed up
 * again after the call. For general buffer data provided as input please refer to the documentation of the respective
 * function on how the data will be handled.
 *
 * Objects created with \b vuCreate..() are owned by the application and should be explicitly destroyed
 * with the corresponding \b vuDestroy..() function as soon as they are not needed any more.
 *
 * For some objects owned by the Engine instance the API provides ways to acquire an explicit reference via \b vuAcquire..()
 * to keep the data beyond the usual lifetime. These objects need to be explicitly released again via a call to
 * \b vuRelease..() to free up the resources.
 *
 * It is strongly recommended to explicitly destroy/release objects as soon as they are not needed any more
 * and not to hold on to these longer than required. This is particular important for data from the state, like e.g.
 * frame image data, to allow Engine to free up resources internally. If the data is required for an extended amout of time,
 * the client should instead create a copy of the data needed.
 *
 * Note that when Vuforia Engine is destroyed it will free up any resources internally and will also invalidate pointers owned
 * by Engine. Additionally it will invalidate pointers to some object types explicitly created/acquired by the client application,
 * such as observers and state references, that hold on to important internal resources. In general, the client should not access
 * any pointers to objects after Engine destruction and should explicitly destroy/release any pointers prior to destroying
 * the Engine instance in order to avoid any resource leaks.
 *
 * See also the documentation of the respective API functions for information on ownership and lifetime.
 *
 * \b Versioning
 *
 * At compilation time, you can access the \b VU_VERSION_MAJOR|MINOR|PATCH preprocessor values defined in the
 * VuforiaEngine.h umbrella header file. At runtime, you can query the Vuforia Engine version by calling the function
 * vuEngineGetLibraryVersionInfo().
 *
 * \b Multithreading and thread safety
 *
 * The Vuforia API is NOT thread-safe and API functions should NOT be called concurrently from multiple threads.
 * It is the responsibility of the client code to ensure that concurrent calls from multiple threads to Vuforia API
 * functions are synchronized. Failing to protect these API calls from concurrent access in multiple threads might
 * result in undefined behavior including memory corruption, instability or crashes.
 *
 * \b Vuforia callbacks
 *
 * Vuforia Engine callbacks are executed on a dedicated internal thread. Callback implementations in client code need
 * to be synchronized with other threads in your client code. Note that Vuforia Engine uses several other threads
 * internally to operate the AR pipeline.
 *
 * The Vuforia Engine internally synchronizes all API calls made from a client thread with the callback thread.
 * All API calls are performed in a blocking manner and the internal state is guaranteed to be updated once control
 * returns to user code. As a consequence, changes to the internal AR pipeline (e.g. observer manipulation via
 * functions such as vuObserverActivate(), vu*ObserverSetTargetScale(), etc.) may take one camera frame to process
 * until they return. API calls are executed immediately when Vuforia is not running or called from within the state
 * handler callback on the camera thread.
 *
 * Due to the synchronous nature of the Vuforia API internals, it is possible that the Vuforia State obtained via
 * the registered state handler or via vuEngineAcquireLatestState() may not reflect changes made during the last camera
 * frame. Observations from an observer deactivated or destroyed during the last frame might still be reported.
 *
 * \b Callbacks and reentrancy
 *
 * The Vuforia API allows reentrant calls to be performed during callbacks such as the camera state handler or the
 * Cloud Image Target observation handler, except calls controlling the Vuforia Engine's lifecycle such as vuEngineStop()
 * or vuEngineDestroy(), as well as functions modifying the callback handler like vuEngineRegisterStateHandler().
 * Functions that cannot be invoked during a callback are highlighted explicitly in the respective API documentation.
 *
 * Client code also need to ensure that reentrant calls from the callback are synchronized with calls from other
 * application threads as mentioned earlier in the section on "Multithreading and thread safety".
 *
 * \b Documentation
 *
 * Inline documentation is using Doxygen formatting with the C++ convention.
 */

/** \} */


// ======== ENGINE LIFECYCLE ========

/** \addtogroup EngineLifecycleGroup Engine
 * \{
 */

/// \brief Vuforia Engine instance creation error codes
/**
 * \note Additional errors are related to engine configuration,
 * see the respective error code for each engine configuration type
 */
VU_ENUM(VuEngineCreationError)
{
    VU_ENGINE_CREATION_ERROR_NONE = 0x0,                 ///< No error
    VU_ENGINE_CREATION_ERROR_DEVICE_NOT_SUPPORTED = 0x1, ///< The device is not supported
    VU_ENGINE_CREATION_ERROR_PERMISSION_ERROR = 0x2, ///< One or more permissions required by Vuforia Engine are missing or not granted by
                                                     ///< user (e.g. the user may have denied camera access to the App)
    VU_ENGINE_CREATION_ERROR_LICENSE_ERROR = 0x3,    ///< A valid license configuration is required
    VU_ENGINE_CREATION_ERROR_INITIALIZATION =
        0x4 ///< An error occurred during initialization of the Vuforia Engine instance (e.g. an instance already exists)
};

/** \} */

// ======== VUFORIA ENGINE CONFIGURATION ========

/** \addtogroup EngineConfigGroup Engine Configuration
 * \{
 */

/// \brief Vuforia EngineConfig handle
typedef struct VuEngineConfigSet_ VuEngineConfigSet;

/// \brief Create a container to hold a list of EngineConfig configuration data
VU_API VuResult VU_API_CALL vuEngineConfigSetCreate(VuEngineConfigSet** configSet);

/// \brief Destroy a container holding a list of EngineConfig configuration data
VU_API VuResult VU_API_CALL vuEngineConfigSetDestroy(VuEngineConfigSet* configSet);

/// \brief Return the number of elements in the list of EngineConfig configuration data in a container
VU_API VuResult VU_API_CALL vuEngineConfigSetGetSize(const VuEngineConfigSet* configSet, int32_t* setSize);

/** \} */

// ======== VUFORIA ENGINE LIFECYCLE MANAGEMENT ========

/** \addtogroup EngineLifecycleGroup Engine
 * \{
 * Engine is the main entry point for interacting with the Vuforia SDK. It is an instance of the Vuforia Engine.
 *
 * A Vuforia Engine instance can be created, destroyed and should be coupled with the lifecycle of your application.
 *
 * A Vuforia Engine instance maintains a list of Observers that are used to observe the real world and report
 * Observations (see Observers section).
 *
 * A Vuforia Engine instance can be configured with a set of EngineConfig data structures to define the general
 * behavior of the Engine (see EngineConfig section).
 */

// ENGINE INSTANCE LIFECYCLE MANAGEMENT

/// \brief Vuforia Engine handle
typedef struct VuEngine_ VuEngine;

/// \brief Create a Vuforia Engine instance
/**
 * \param engine Will be set to the created Engine instance on success, otherwise stays unmodified
 * \param configSet Set of configurations for creating the Engine instance
 * \param errorCode Optional output error code (can be NULL). The error code will only be set if the call fails,
 * otherwise stays unmodified
 * \returns VU_SUCCESS if the Engine instance was created successfully, otherwise VU_FAILED to indicate an error.
 * The value of \p errorCode provides the specific reason for the error.
 *
 * \details The parameter \p errorCode has the generic type VuErrorCode and not the more specific
 * VuEngineCreationError type because Engine creation may also fail because of configuration errors.
 * Engine creation error codes have 2 main categories:
 *    - VuEngineCreationError: General Engine creation error type not related to configuration errors
 *    - Vu*ConfigError: Engine creation errors related to a specific configuration, e.g. VuLicenseConfigError
 *      for license configuration-specific errors of the VuLicenseConfig
 * All error types are converted to the generic VuErrorCode when an error is reported. Each error code has a unique
 * value across the various error types. You can compare the returned error code with the values of the general
 * VuEngineCreationError type as well as the respective error types for the configurations used when creating the
 * Engine instance, e.g. VuLicenseConfigError, to determine the reason for the failure.
 *
 * \parblock
 * \note Engine creation is a potentially long running operation and should not be called on the main/UI thread.
 * \endparblock
 *
 * \parblock
 * \note Only one Engine instance can exist at any given time. The call will fail if another engine instance already
 * exists. You need to destroy the first instance before creating a new Engine instance.
 * \endparblock
 *
 * \par Permissions
 *
 * Your App must fulfill platform-specific permission / configuration requirements in order to successfully create an Engine instance.
 * Your App is responsible for executing the following platform-specific actions prior to invoking vuEngineCreate(), otherwise
 * the Engine creation call may fail or Engine may not work properly:
 *
 * - Android:
 *     - Camera permission: Add the "android.permission.CAMERA" permission to the AndroidManifest.xml and also request it at run-time in
 * your App's code, otherwise Engine fails to initialize with the error VU_ENGINE_CREATION_ERROR_PERMISSION_ERROR.
 *     - Network-related permissions: Add the "android.permission.INTERNET" and "android.permission.ACCESS_NETWORK_STATE" permissions to the
 * AndroidManifest.xml to allow Engine to leverage various cloud services and to ensure an optimal AR experience on all devices by
 * downloading device-specific Engine settings, otherwise Engine fails to initialize with the error
 * VU_ENGINE_CREATION_ERROR_PERMISSION_ERROR.
 *     - High sensor sampling rate: When targeting Android 12 or above, add the "android.permission.HIGH_SAMPLING_RATE_SENSORS" permission
 * to the AndroidManifest.xml to allow Engine to receive IMU sensor readings at a high rate, otherwise device tracking may not work
 * correctly.
 *
 * - iOS:
 *     - Camera usage description: Add the key "NSCameraUsageDescription" to the Info.plist with an explanation why your App requires camera
 * access, otherwise the App will be rejected by the App Store submission process
 *     - Camera access: Request access to the camera by calling the "AVCaptureDevice requestAccessForMediaType" API with the media type
 * "AVMediaTypeVideo", otherwise Engine fails to initialize with VU_ENGINE_CREATION_ERROR_PERMISSION_ERROR
 *
 * - UWP:
 *     - Video capturing and network device capability: Add the following snippet to the \b \<Capabilities\> tag in your App's
 * Package.appxmanifest to enable camera capturing and full network capabilities, otherwise Engine will not work properly:
 *
 *       \code{.xml}
 *       <Capabilities>
 *           <Capability Name="internetClient" />
 *           <DeviceCapability Name="webcam" />
 *       </Capabilities>
 *       \endcode
 *
 * - Lumin:
 *     - Add the following permissions to your App's manifest.xml, otherwise Engine fails to initialize with the error
 * VU_ENGINE_CREATION_ERROR_PERMISSION_ERROR:
 *         - Camera permissions: Add "CameraCapture" and "ComputerVision" permissions to the manifest.xml
 *         - Network-related permissions: Add the "Internet" permission to the manifest.xml to allow Engine to leverage various cloud
 * services and ensure an optimal AR experience on all devices by downloading device-specific Engine settings
 *         - Perception system permissions: Add "LowLatencyLightwear", "PcfRead" and "WorldReconstruction" permissions to the manifest.xml
 */
VU_API VuResult VU_API_CALL vuEngineCreate(VuEngine** engine, const VuEngineConfigSet* configSet, VuErrorCode* errorCode);

/// \brief Destroy a Vuforia Engine instance
/**
 * \details This will release all resources used by the Engine instance and implicitly stop the Engine instance if it
 * is still running. It is recommended to explicitly shutdown/free any resources prior to destroying the Engine
 * instance.
 *
 * \parblock
 * \note Destroying the engine instance is a potentially long running operation and should not be called on the
 * main/UI thread. The time also depends on the features being used when destroying the Engine instance.
 * Please also see the documentation of the respective Engine features in use.
 * \endparblock
 *
 * \note This function will fail if called from a Vuforia Engine callback. Vuforia Engine does not allow certain
 * API functions to be called reentrant from a callback, i.e. to enter the API again on the same callstack.
 * See also section "Callbacks and reentrancy" for more information.
 */
VU_API VuResult VU_API_CALL vuEngineDestroy(VuEngine* engine);

/// \brief Start a Vuforia Engine instance
/**
 * \details This may implicitly start any internal resources of observers and controllers that have been activated
 * or started prior to starting the Engine instance.
 *
 * \parblock
 * \note Starting the engine instance is a potentially long running operation and should not be called on the
 * main/UI thread. The time also depends on the observer and controller resources being started implicitly.
 * Please also see the documentation of the respective Engine features.
 * \endparblock
 *
 * \note This function will fail if the Engine instance is already running
 */
VU_API VuResult VU_API_CALL vuEngineStart(VuEngine* engine);

/// \brief Stop a Vuforia Engine instance
/**
 * \details This will also deactivate all observers and stop any running controllers
 *
 * \parblock
 * \note Stopping the engine instance is a potentially long running operation and should ideally not be called on the
 * main/UI thread. The time also depends on the features being used when stopping the Engine instance.
 * Please also see the documentation of the respective Engine features in use.
 * \endparblock
 *
 * \note This function will fail if called from a Vuforia Engine callback. Vuforia Engine does not allow certain
 * API functions to be called reentrant from a callback, i.e. to enter the API again on the same callstack.
 * See also section "Callbacks and reentrancy" for more information.
 */
VU_API VuResult VU_API_CALL vuEngineStop(VuEngine* engine);

/// \brief Return VU_TRUE if the given Vuforia Engine instance has been started
VU_API VuBool VU_API_CALL vuEngineIsRunning(const VuEngine* engine);

// ENGINE VERSION

/// \brief Vuforia Engine library version information
typedef struct VuLibraryVersionInfo
{
    /// \brief Library version as a string in "major.minor.patch+build" format
    const char* versionString;

    /// \brief Major version
    int32_t major;

    /// \brief Minor version
    int32_t minor;

    /// \brief Patch version
    int32_t patch;

    /// \brief Build ID / metadata
    const char* build;
} VuLibraryVersionInfo;

/// \brief Get the Vuforia Engine library version information
/**
 * \returns A data structure with the Vuforia Engine library version information
 */
VU_API VuLibraryVersionInfo VU_API_CALL vuEngineGetLibraryVersionInfo();

/** \} */

// ======== VUFORIA OBSERVER AND OBSERVATION MANAGEMENT ========

/** \addtogroup ObserverObservationManagementGroup Observer and Observation Management
 * \{
 * Observers are created by observer-specific creation functions consuming an observer-specific
 * configuration data stucture Vu<ObserverType>Config, typically with the following signature:
 *
 * VuResult vuEngineCreate<ObserverType>Observer(VuEngine* engine, VuObserver** observer,
 *    const Vu<ObserverType>Config* config, Vu<ObserverType>CreationError* errorCode);
 *
 * The configuration data structure can be initialized with default parameters with a dedicated
 * function vu<ObserverType>ConfigDefault(), and can be customized with desired settings.
 * When creation fails, the creation function outputs an observer-specific error code of type
 * Vu<ObserverType>CreationError.
 *
 * The observer-specific configurations have a field of type VuBool called \p activate, which
 * determines whether observer-internal resources should be activated automatically upon
 * creation, or they require explicit activation by calling vuObserverActivate(). All observers
 * are automatically deactivated when stopping the Vuforia Engine via vuEngineStop(), or they
 * can be deactivated individually by calling vuObserverDeactivate().
 *
 * If an observer is configured to be automatically activated on creation and the activation
 * fails, then the \p errorCode parameter supplied to the observer creation function (e.g.
 * vuEngineCreate<ObserverType>Observer()) is set to VU_<OBSERVER_TYPE>_CREATION_ERROR_AUTOACTIVATION_FAILED
 * for all observer types where applicable.
 *
 * Individual observers can be destroyed with the general vuObserverDestroy() function. A list
 * of observers can be destroyed by calling vuObserversDestroy() with a VuObserverList.
 * Neither of the destruction functions are observer-specific, unlike the observer creation functions.
 *
 * Note that for modification of observers (vu<ObserverType>ObserverSet<Property>()) or for
 * creation of observers with non-default optional arguments (for example "scale") it is
 * recommended that all observers from the same database are deactivated as these operations
 * can be expensive otherwise - especially when the database contains a large amount of
 * targets.
 */

/// \brief Tracking optimization types
VU_ENUM(VuTrackingOptimization)
{
    VU_TRACKING_OPTIMIZATION_DEFAULT = 0x1,             ///< Best tracking performance for the majority of use cases and targets.
    VU_TRACKING_OPTIMIZATION_LOW_FEATURE_OBJECTS = 0x2, ///< Optimize tracking for objects with smooth, untextured surfaces, that
                                                        ///< lack features and detailed structures, including cars, sheet metal parts
                                                        ///< and many industrial objects.
                                                        ///< \note Do not use for objects that move while being tracked.
    VU_TRACKING_OPTIMIZATION_AR_CONTROLLER = 0x3,       ///< Optimize tracking for use cases where small, handheld fast moving
                                                        ///< targets are used like an AR/VR controller. Tracking will be less stable if
                                                        ///< the object is stationary.
};

// OBSERVER MANAGEMENT

/// \brief Vuforia Observer handle
typedef struct VuObserver_ VuObserver;

/// \brief Observer type
typedef int32_t VuObserverType;

/// \brief Get a unique ID associated with an observer
/**
 * The ID is a positive number and is unique within a Vuforia session. It is generated at
 * runtime and is not persistent across Vuforia sessions
 */
VU_API int32_t VU_API_CALL vuObserverGetId(const VuObserver* observer);

/// \brief Get the type of an observer
VU_API VuResult VU_API_CALL vuObserverGetType(const VuObserver* observer, VuObserverType* observerType);

/// \brief Check the type of an observer
VU_API VuBool VU_API_CALL vuObserverIsType(const VuObserver* observer, VuObserverType observerType);

// OBSERVER LIST MANAGEMENT

/// \brief Vuforia ObserverList handle
typedef struct VuObserverList_ VuObserverList;

/// \brief Create an observer list
VU_API VuResult VU_API_CALL vuObserverListCreate(VuObserverList** list);

/// \brief Get number of elements in an observer list
VU_API VuResult VU_API_CALL vuObserverListGetSize(const VuObserverList* list, int32_t* listSize);

/// \brief Get an element in an observer list
VU_API VuResult VU_API_CALL vuObserverListGetElement(const VuObserverList* list, int32_t element, VuObserver** observer);

/// \brief Destroy an observer list
VU_API VuResult VU_API_CALL vuObserverListDestroy(VuObserverList* list);

// OBSERVER RETRIEVAL

/// \brief Get an observer from Vuforia Engine using its unique ID
VU_API VuResult VU_API_CALL vuEngineGetObserver(const VuEngine* engine, int32_t observerId, VuObserver** observer);

/// \brief Get all observers from Vuforia Engine
VU_API VuResult VU_API_CALL vuEngineGetObservers(const VuEngine* engine, VuObserverList* observerList);

// OBSERVER LIFECYCLE MANAGEMENT

/// \brief Destroy an observer
VU_API VuResult VU_API_CALL vuObserverDestroy(VuObserver* observer);

/// \brief Destroy multiple observers
VU_API VuResult VU_API_CALL vuObserversDestroy(VuObserverList* observerList);

/// \brief Destroy all observers in Vuforia Engine
VU_API VuResult VU_API_CALL vuEngineDestroyObservers(VuEngine* engine);

/// \brief Activate an observer
VU_API VuResult VU_API_CALL vuObserverActivate(VuObserver* observer);

/// \brief Deactivate an observer
VU_API VuResult VU_API_CALL vuObserverDeactivate(VuObserver* observer);

/// \brief Check whether an observer is activated
/**
 * \returns VU_TRUE if the observer is activated, VU_FALSE if the observer is not activated
 */
VU_API VuBool VU_API_CALL vuObserverIsActivated(const VuObserver* observer);

// OBSERVATION MANAGEMENT

/// \brief Vuforia Observation handle
typedef struct VuObservation_ VuObservation;

/// \brief Observation type
typedef int32_t VuObservationType;

/// \brief Get the type of an observation
VU_API VuResult VU_API_CALL vuObservationGetType(const VuObservation* observation, VuObservationType* observationType);

/// \brief Check the type of an observation
VU_API VuBool VU_API_CALL vuObservationIsType(const VuObservation* observation, VuObservationType observationType);

/// \brief Get ID of the observer which has created given observation
VU_API int32_t VU_API_CALL vuObservationGetObserverId(const VuObservation* observation);

// OBSERVATION DATA MANAGEMENT

/// \brief Observation pose status
VU_ENUM(VuObservationPoseStatus)
{
    VU_OBSERVATION_POSE_STATUS_NO_POSE = 0x1,         ///< No valid pose available. For details refer to the status info. The
                                                      ///< NOT_OBSERVED status info is returned if an observed object is currently
                                                      ///< not being tracked. Some observers may provide additional information for
                                                      ///< NO_POSE in their status info.
    VU_OBSERVATION_POSE_STATUS_LIMITED = 0x2,         ///< Observed object is being tracked in a limited form, and so the pose may
                                                      ///< be unreliable or degraded. For details refer to the status info
    VU_OBSERVATION_POSE_STATUS_TRACKED = 0x3,         ///< Observed object is being tracked with a valid pose
    VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED = 0x4 ///< Observed object is being tracked using extended tracking
};

/// \brief Information about pose-specific data in an observation with a pose
/**
 * \note Use the vuObservationHasPoseInfo function to check whether a VuObservation has pose information
 * that can be retrieved with vuObservationGetPoseInfo.
 */
typedef struct VuPoseInfo
{
    /// \brief Pose status
    /**
     * \note See documentation for VuObservationPoseStatus about possible values.
     */
    VuObservationPoseStatus poseStatus;

    /// \brief Pose
    /**
     * \note The pose is represented as a pose matrix immediately suitable for rendering in OpenGL.
     * For details about the convention used, see the documentation of VuMatrix44F.
     */
    VuMatrix44F pose;
} VuPoseInfo;

/// \brief Check whether an observation contains pose information
VU_API VuBool VU_API_CALL vuObservationHasPoseInfo(const VuObservation* observation);

/// \brief Get pose info associated to an observation (for observation having pose, see availability for each observer)
/**
 * \note This call fails if the observation does not contain pose information. You can verify this by calling
 * vuObservationHasPoseInfo() and checking its return value: VU_TRUE for an observation with pose information,
 * VU_FALSE without.
 */
VU_API VuResult VU_API_CALL vuObservationGetPoseInfo(const VuObservation* observation, VuPoseInfo* poseInfo);

// OBSERVATION LIST MANAGEMENT

/// \brief Vuforia ObservationList handle
typedef struct VuObservationList_ VuObservationList;

/// \brief Create an observation list
VU_API VuResult VU_API_CALL vuObservationListCreate(VuObservationList** list);

/// \brief Get number of elements in an observation list
VU_API VuResult VU_API_CALL vuObservationListGetSize(const VuObservationList* list, int32_t* listSize);

/// \brief Get an element in an observation list
VU_API VuResult VU_API_CALL vuObservationListGetElement(const VuObservationList* list, int32_t element, VuObservation** observation);

/// \brief Destroy an observation list
VU_API VuResult VU_API_CALL vuObservationListDestroy(VuObservationList* list);

/** \} */

// ======== VUFORIA ENGINE STATE MANAGEMENT ========

/** \addtogroup EngineStateUpdateGroup State Management
 * \{
 * The Vuforia State represents a snapshot of what Vuforia Engine "knows" about the user's environment.
 * It contains all information Vuforia clients need to render an AR scene, including observations, camera
 * and rendering information. Individual state information elements can be retrieved from VuState with
 * the respective vuStateGet* getter functions, e.g. vuStateGetObservations() acquires the current list of
 * observations generated by observers. VuState objects are immutable and their data cannot be changed after
 * creation.
 *
 * There are 2 ways to obtain the latest Vuforia State from the Vuforia Engine:
 *
 *   - Pull mechanism: The function vuEngineAcquireLatestState() allows the acquisition of the latest state
 *     on demand while Engine is running. State objects acquired with this mechanism must be released by
 *     calling vuStateRelease() when no longer needed to avoid leaking memory.
 *
 *   - Push mechanism: The vuEngineRegisterStateHandler() allows the registration of a callback function of type
 *     VuStateHandler, which informs the registered client about the latest state as it becomes available. Only
 *     one callback function can be registered at one time. Callbacks keep coming from Engine as long as it is
 *     running or the client unregisters the callback by calling vuEngineRegisterStateHandler() with a nullptr
 *     as the handler function. State objects obtained with this mechanism do not need to (and cannot) be released,
 *     and are valid only during the callback scope.
 *
 * The VuState object acquired via the pull mechanism remains valid as long as the Engine instance is valid or
 * until vuStateRelease() is called to release the internal state data. The VuState object received via the push
 * mechanism remains valid during the scope of the callback.
 *
 * If you wish to extend the lifetime beyond its original scope (e.g. to preserve state data outside the state
 * handler callback), you need to create a state reference. You can create any number of references to a VuState
 * instance by calling vuStateAcquireReference(). A state reference contains the same immutable information as
 * the VuState it has been created from but its lifetime becomes independent from it. This means that even if you
 * call vuStateRelease() on a given VuState, a reference created from it beforehand remains valid until
 * vuStateRelease() is called on the state reference.
 */

// STATE TYPE

/// \brief Vuforia State handle
typedef struct VuState_ VuState;

// STATE MANAGEMENT

/// \brief Return new state from Vuforia Engine (make sure to call release if you use this function)
/**
 * \note This call will fail if Vuforia is not running.
 * \note The state does not contain any camera frame or render state data if it is acquired
 * after calling vuEngineStart() but before the first camera frame is retrieved by Vuforia
 */
VU_API VuResult VU_API_CALL vuEngineAcquireLatestState(const VuEngine* engine, VuState** state);

/// \brief Release the given state
VU_API VuResult VU_API_CALL vuStateRelease(VuState* state);

/// \brief Acquire a new reference to the given state
VU_API VuResult VU_API_CALL vuStateAcquireReference(const VuState* state, VuState** stateOut);

// STATE UPDATE CALLBACK

/// \brief Vuforia State handler function type
typedef void(VU_API_CALL VuStateHandler)(const VuState* state, void* clientData);

/// \brief Register a state handler to get Vuforia State updates
/**
 * The registered handler function will report the Vuforia State including observations, camera and
 * rendering information at the camera frame rate.
 *
 * \note This function will fail if called from a Vuforia Engine callback. Vuforia Engine does not allow certain
 * API functions to be called reentrant from a callback, i.e. to enter the API again on the same callstack.
 * See also section "Callbacks and reentrancy" for more information.
 *
 * \note Only one handler can be registered at a given time. A handler can be unregistered by providing
 * NULL to the function.
 *
 * \param engine Engine instance
 * \param handler The handler for receiving state updates
 * \param clientData The client data to pass with the state updates
 */
VU_API VuResult VU_API_CALL vuEngineRegisterStateHandler(VuEngine* engine, VuStateHandler* handler, void* clientData);

// STATE DATA RETRIEVAL

/// \brief Get a list of observations from the state
VU_API VuResult VU_API_CALL vuStateGetObservations(const VuState* state, VuObservationList* list);

/// \brief Get all observations with pose info from State
/**
 * \note This call returns observations that can be fed into vuObservationGetPoseInfo to retrieve the pose.
 */
VU_API VuResult VU_API_CALL vuStateGetObservationsWithPoseInfo(const VuState* state, VuObservationList* list);

/// \brief Get all observations from the state associated to a given observer.
/**
 * Returns all observations from the state that have been created by a given observer.
 *
 * \note The user has to make sure that state, observer and list are valid during the time of the call,
 * otherwise the behavior is undefined.
 *
 * \note Any previous content of the given list will be removed if the operation was successful. On failure the list will not be modified.
 *
 * \param state The state containing the observations.
 * \param observer The observer to use as a filter for the observations.
 * \param list The list to fill with the observations found.
 * \returns VU_SUCCESS on success, VU_FAILED on failure.
 */
VU_API VuResult VU_API_CALL vuStateGetObservationsByObserver(const VuState* state, const VuObserver* observer, VuObservationList* list);

/// \brief Get the camera intrinsics from the state
VU_API VuResult VU_API_CALL vuStateGetCameraIntrinsics(const VuState* state, VuCameraIntrinsics* cameraIntrinsics);

// CAMERA FRAME IN STATE

/// \brief Vuforia Camera Frame
typedef struct VuCameraFrame_ VuCameraFrame;

/// \brief Return true if the state contains camera frame data
/**
 * \note The state does not contain a camera frame if it is acquired after calling
 * vuEngineStart() but before the first camera frame is retrieved by Vuforia. There is always a
 * valid camera frame when using a VuStateHandler callback
 */
VU_API VuBool VU_API_CALL vuStateHasCameraFrame(const VuState* state);

/// \brief Get the camera frame from the state
/**
 * \note This call will fail if vuStateHasCameraFrame() returns VU_FALSE which happens if the
 * state is acquired after calling vuEngineStart() but before the first camera frame is retrieved by
 * Vuforia.
 */
VU_API VuResult VU_API_CALL vuStateGetCameraFrame(const VuState* state, VuCameraFrame** cameraFrame);

/// \brief Get index from a camera frame
VU_API VuResult VU_API_CALL vuCameraFrameGetIndex(const VuCameraFrame* cameraFrame, int64_t* index);

/// \brief Get timestamp from a camera frame
VU_API VuResult VU_API_CALL vuCameraFrameGetTimestamp(const VuCameraFrame* cameraFrame, int64_t* timestamp);

/// \brief Get camera image list from a camera frame
VU_API VuResult VU_API_CALL vuCameraFrameGetImages(const VuCameraFrame* cameraFrame, VuImageList* list);

// RENDER STATE

/// \brief Vuforia Render State
typedef struct VuRenderState
{
    // VIDEO BACKGROUND
    /// \brief Viewport settings
    VuVector4I viewport;

    /// \brief Video background projection matrix immediately suitable for rendering in OpenGL
    VuMatrix44F vbProjectionMatrix;

    /// \brief Video background mesh
    /**
     * \note Set to NULL if the state is acquired before the first camera frame is retrieved by Vuforia or
     * if running on a see-through eyewear device
     *
     * \note The video background mesh is bound to the lifetime of the VuState it was retrieved from.
     * Do not use this pointer or the data it points to beyond the lifetime of the respective VuState.
     */
    VuMesh* vbMesh;

    // AUGMENTATION
    /// \brief View matrix (inverse of device pose) immediately suitable for rendering in OpenGL
    /**
     * \note This is set to the identity matrix if there is no active device pose observer.
     * For details about the convention used, see the documentation of VuMatrix44F.
     */
    VuMatrix44F viewMatrix;

    /// \brief Projection matrix (using near/far plane values set in render controller)
    /**
     * Vuforia Engine creates a projection matrix from the latest intrinsic camera calibration information
     * available on the current device, which you can retrieve from the Vuforia State via vuStateGetCameraIntrinsics().
     * The projection matrix uses an OpenGL-style column-major matrix with the following right-handed
     * coordinate system convention for the view space:
     *
     * - The X coordinate system axis points to the right and the Y axis points downwards.
     * - The camera is positioned at the coordinate system origin and points in the positive Z direction.
     * - Normalized device coordinates are used where the Z coordinates are normalized to the range (-1, 1).
     *
     * Rendering solutions using a backend with the same matrix convention as OpenGL can use the matrix as is,
     * while those with a different convention (e.g. DirectX) require conversion before use.
     *
     * Vuforia Engine's projection matrix convention is described as follows. Let's assume that the pose matrix
     * retrieved from an observation via vuObservationGetPoseInfo in VuPoseInfo.pose is set to identity, and
     * the near/far clipping planes have been set up accordingly (e.g. from 0.01f to 100.0f) via
     * vuRenderControllerSetProjectionMatrixNearFar().
     *
     * - An object with coordinates (0,0,1) will show up in the middle of the viewport, being one scene unit away
     *   from the camera center.
     * - If we place the object at (0,0,2), it will still be in the middle of the viewport but at half the size as before.
     * - If we move the object to (1,0,1), then it will move to the right of the viewport center.
     * - If we move the object to (0,1,1), then it will move to lower than the viewport center.
     *
     * Since the Vuforia Engine API provides access to the intrinsic camera calibration, you can always build your
     * own projection matrix if your application uses a rendering solution that requires a different convention.
     * In this case, the observations' pose matrix must be also converted accordingly.
     */
    VuMatrix44F projectionMatrix;
} VuRenderState;

/// \brief Get the render state frame from the state
/**
 * \note All members of the render state are 0 if the state is acquired after calling
 * vuEngineStart() but before the first camera frame is retrieved by Vuforia
 *
 * \note Viewport and video background rendering data may be 0 on some platforms when
 * Vuforia cannot auto-detect a default viewport size at engine creation time and
 * vuRenderControllerSetRenderViewConfig() was not called by the user
 *
 * \warning The behaviour of this function will change in an upcoming release.
 */
VU_API VuResult VU_API_CALL vuStateGetRenderState(const VuState* state, VuRenderState* renderState);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_ENGINE_H_
