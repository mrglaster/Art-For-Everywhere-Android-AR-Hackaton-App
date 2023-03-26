/*===============================================================================
Copyright (c) 2022 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_RENDERCONFIG_H_
#define _VU_RENDERCONFIG_H_

/**
 * \file RenderConfig.h
 * \brief Rendering-specific configuration data for the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup EngineRenderConfigGroup Rendering-related Engine Configuration
 * \{
 */

/// \brief Rendering configuration error code type for errors occurring when creating a Vuforia Engine instance
/**
 * \note The error code is reported via the \p errorCode parameter of the vuEngineCreate() function if an error
 * related to the rendering configuration occurs while initializing the new Engine instance.
 */
VU_ENUM(VuRenderConfigError)
{
    VU_ENGINE_CREATION_ERROR_RENDER_CONFIG_UNSUPPORTED_BACKEND = 0x300,            ///< Unsupported render backend
    VU_ENGINE_CREATION_ERROR_RENDER_CONFIG_FAILED_TO_SET_VIDEO_BG_VIEWPORT = 0x301 ///< Failed to set video background viewport
};


/// \brief Render video background backend configuration
VU_ENUM(VuRenderVBBackendType)
{
    VU_RENDER_VB_BACKEND_DEFAULT = 0x1,  ///< Select default rendering backend for each platform. Currently:
                                         ///< Android: OpenGL ES 3.x
                                         ///< iOS: Metal
                                         ///< UWP: DirectX 11
                                         ///< This is the default video background configuration.
    VU_RENDER_VB_BACKEND_HEADLESS = 0x2, ///< Deactivate usage of video background rendering support.
                                         ///< Note: if this is set you won't be able to turn it back later.
    VU_RENDER_VB_BACKEND_GLES3 = 0x4,    ///< OpenGL ES 3.x
    VU_RENDER_VB_BACKEND_DX11 = 0x5,     ///< DirectX 11
    VU_RENDER_VB_BACKEND_METAL = 0x6     ///< Metal
};

/// \brief Supported video background viewport modes
VU_ENUM(VuVideoBackgroundViewportMode)
{
    VU_VIDEOBG_VIEWPORT_MODE_SCALE_TO_FILL = 0x1, ///< Fill mode with aspect ratio (can crop or stretch the view). This is the default.
    VU_VIDEOBG_VIEWPORT_MODE_SCALE_TO_FIT = 0x2,  ///< Letter box mode with aspect ratio (will add black band around the image)
    VU_VIDEOBG_VIEWPORT_MODE_NATIVE_VIDEO = 0x3   ///< Use native video mode, don't apply any aspect ratio compensation
};

/// \brief Render configuration data structure
typedef struct VuRenderConfig
{
    ///\brief Choice of video background rendering configuration
    /**
     *  Default value is VU_RENDER_VB_BACKEND_DEFAULT
     */
    VuRenderVBBackendType vbRenderBackend;

    ///\brief Choice of video background rendering configuration
    /**
     *  Default value is VU_VIDEOBG_VIEWPORT_MODE_SCALE_TO_FILL
     */
    VuVideoBackgroundViewportMode vbViewportMode;
} VuRenderConfig;

/// \brief Default render configuration
/**
 * \note Use this function to initialize the VuRenderConfig data structure with default values.
 */
VU_API VuRenderConfig VU_API_CALL vuRenderConfigDefault();

/// \brief Add a render configuration to the engine config
VU_API VuResult VU_API_CALL vuEngineConfigSetAddRenderConfig(VuEngineConfigSet* configSet, const VuRenderConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_RENDERCONFIG_H_
