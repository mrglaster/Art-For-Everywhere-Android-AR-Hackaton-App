/*===============================================================================
Copyright (c) 2021 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_VIRTUALBUTTONOBSERVER_H_
#define _VU_VIRTUALBUTTONOBSERVER_H_

/**
 * \file VirtualButtonObserver.h
 * \brief Observer for the Virtual Button feature
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup VirtualButtonObserverGroup Virtual Button Feature
 * \{
 */

/// \brief Configuration error for Virtual Button creation
VU_ENUM(VuVirtualButtonCreationError)
{
    VU_VIRTUAL_BUTTON_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_VIRTUAL_BUTTON_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred during observer creation
    VU_VIRTUAL_BUTTON_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< Observer auto-activation failed
    VU_VIRTUAL_BUTTON_CREATION_ERROR_INVALID_OBSERVER =
        0x3, ///< Observer is null or of an invalid type, only Image Target observers are supported
    VU_VIRTUAL_BUTTON_CREATION_ERROR_INVALID_AREA = 0x4 ///< The provided area is invalid
};

/// \brief Sensitivity of press detection in Virtual Buttons
VU_ENUM(VuVirtualButtonSensitivity)
{
    VU_VIRTUAL_BUTTON_SENSITIVITY_HIGH = 0x1,   ///< Fast detection
    VU_VIRTUAL_BUTTON_SENSITIVITY_MEDIUM = 0x2, ///< Balanced between fast and robust
    VU_VIRTUAL_BUTTON_SENSITIVITY_LOW = 0x3     ///< Robust detection
};

/// \brief Virtual Button state
VU_ENUM(VuVirtualButtonState)
{
    VU_VIRTUAL_BUTTON_STATE_RELEASED = 0x1,    ///< Button is not pressed
    VU_VIRTUAL_BUTTON_STATE_PRESSED = 0x2,     ///< Button is pressed
    VU_VIRTUAL_BUTTON_STATE_NOT_OBSERVED = 0x3 ///< Button is not detected
};

/// \brief Configuration for Virtual Button creation
typedef struct VuVirtualButtonConfig
{
    /// \brief Observer the Virtual Button is associated with
    /**
     * \note Currently only ImageTarget observer is supported.
     * The Virtual Button observer will only report observations to the state while
     * the associated observer is active.
     */
    VuObserver* observer;

    /// \brief Rectangular 2D area of the Virtual Button
    /**
     * \note The available area is the size of the Image Target in meters.
     * (use vuImageTargetObserverGetTargetSize() to retrieve it)
     * The origin (0/0) of the coordinate system is the center of the Image Target and it
     * extends half width/height of it in each direction. (the top left corner is -x/+y)
     */
    VuRectangle area;

    /// \brief Observer activation
    /**
     * \note Default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Sensitivity of the Virtual Button
    /**
     * \note Sensitivity allows deciding between fast vs. robust button press measurements.
     * The default value is VU_VIRTUAL_BUTTON_SENSITIVITY_LOW.
     */
    VuVirtualButtonSensitivity sensitivity;
} VuVirtualButtonConfig;

/// \brief Default Virtual Button configuration
/**
 * \note Use this function to initialize the VuVirtualButtonConfig data structure with default values.
 */
VU_API VuVirtualButtonConfig VU_API_CALL vuVirtualButtonConfigDefault();

/// \brief Information for Virtual Button from observation
typedef struct VuVirtualButtonObservationInfo
{
    /// \brief State of the button
    VuVirtualButtonState state;
} VuVirtualButtonObservationInfo;

/*! \var VU_OBSERVER_VIRTUAL_BUTTON_TYPE VU_OBSERVER_VIRTUAL_BUTTON_TYPE
 * \brief Type identifier for Virtual Button observers
 */
VU_CONST_INT(VU_OBSERVER_VIRTUAL_BUTTON_TYPE, 0xD);

/*! \var VU_OBSERVATION_VIRTUAL_BUTTON_TYPE VU_OBSERVATION_VIRTUAL_BUTTON_TYPE
 * \brief Type identifier for Virtual Button observations
 */
VU_CONST_INT(VU_OBSERVATION_VIRTUAL_BUTTON_TYPE, 0xD);

/// \brief Create a Virtual Button observer
/**
 * \param engine The engine instance
 * \param observer Output argument for the created observer instance
 * \param config Virtual Button configuration
 * \param errorCode Optional argument to retrieve additional error information. Can be NULL.
 *
 * \returns VU_SUCCESS if the Virtual Button was created successfully, otherwise VU_FAILED
 */
VU_API VuResult VU_API_CALL vuEngineCreateVirtualButtonObserver(VuEngine* engine, VuObserver** observer,
                                                                const VuVirtualButtonConfig* config,
                                                                VuVirtualButtonCreationError* errorCode);

/// \brief Get all Virtual Button observers
/**
 * \param engine The engine instance
 * \param observerList Output argument for the observer list
 *
 * \returns VU_SUCCESS on success, otherwise VU_FAILED
 */
VU_API VuResult VU_API_CALL vuEngineGetVirtualButtonObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get the sensitivity of a Virtual Button
/**
 * \param observer The Virtual Button instance
 * \param sensitivity Output argument for the current sensitivity value
 *
 * \returns VU_SUCCESS on success, otherwise VU_FAILED
 */
VU_API VuResult VU_API_CALL vuVirtualButtonObserverGetSensitivity(const VuObserver* observer, VuVirtualButtonSensitivity* sensitivity);

/// \brief Set the sensitivity of a Virtual Button
/**
 * \param observer The Virtual Button instance
 * \param sensitivity The new sensitivity value
 *
 * \returns VU_SUCCESS on success, otherwise VU_FAILED
 */
VU_API VuResult VU_API_CALL vuVirtualButtonObserverSetSensitivity(VuObserver* observer, VuVirtualButtonSensitivity sensitivity);

/// \brief Get the area of a Virtual Button
/**
 * \param observer The Virtual Button instance
 * \param area Output argument for the current area
 *
 * \returns VU_SUCCESS on success, otherwise VU_FAILED
 */
VU_API VuResult VU_API_CALL vuVirtualButtonObserverGetArea(const VuObserver* observer, VuRectangle* area);

/// \brief Set the area of a Virtual Button
/**
 * \param observer The Virtual Button instance
 * \param area The new area
 *
 * \returns VU_SUCCESS on success, otherwise VU_FAILED
 */
VU_API VuResult VU_API_CALL vuVirtualButtonObserverSetArea(VuObserver* observer, const VuRectangle* area);

/// \brief Get the observer this Virtual Button is associated with
/**
 * \param observer The Virtual Button instance
 * \param associatedObserver Output argument for the associated observer
 *
 * \returns VU_SUCCESS on success or VU_FAILED if the associated observer was already destroyed
 */
VU_API VuResult VU_API_CALL vuVirtualButtonObserverGetAssociatedObserver(const VuObserver* observer, VuObserver** associatedObserver);

/// \brief Get all Virtual Button observations
VU_API VuResult VU_API_CALL vuStateGetVirtualButtonObservations(const VuState* state, VuObservationList* list);

/// \brief Get info associated with a Virtual Button observation
VU_API VuResult VU_API_CALL vuVirtualButtonObservationGetInfo(const VuObservation* observation,
                                                              VuVirtualButtonObservationInfo* virtualButtonInfo);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_VIRTUALBUTTONOBSERVER_H_
