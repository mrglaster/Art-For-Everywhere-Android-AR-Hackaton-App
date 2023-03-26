/*===============================================================================
Copyright (c) 2021 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_RENDERCONTROLLER_H_
#define _VU_RENDERCONTROLLER_H_

/**
 * \file RenderController.h
 * \brief Controller to access rendering-specific functionality in the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>
#include <VuforiaEngine/Engine/RenderConfig.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup RenderControllerGroup Render Controller
 * \{
 * This controller allows you to control the rendering functionalities of the Vuforia SDK
 * include video background configuration.
 */

/// \brief configure the current view
typedef struct VuRenderViewConfig
{
    /// \brief Resolution of the current view/render target used
    VuVector2I resolution;
} VuRenderViewConfig;

/// \brief Video background view information
typedef struct VuVideoBackgroundViewInfo
{
    /// \brief Current viewport used for augmentation and video background rendering
    VuVector4I viewport;

    /// \brief Image size used for rendering
    VuVector2I cameraImageSize;

    /// \brief Size of the video background rendered on screen (can be stretched)
    VuVector2I vBTextureSize;
} VuVideoBackgroundViewInfo;

/// \brief Video background texture input
typedef struct VuRenderVideoBackgroundData
{
    /// \brief Render data used for video background
    /**
     * \note  OpenGL ES: NULL
     *        DX11: ID3D11Device*
     *        Metal: id<MTLRenderCommandEncoder>
     */
    const void* renderData;

    /// \brief Texture data used for video background
    /**
     * \note  OpenGL ES: NULL
     *        DX11: ID3D11Texture2D*
     *        Metal: id<MTLTexture>
     */
    const void* textureData;

    /// \brief Texture unit used for video background
    /**
     * \note  OpenGL ES: int* pointing to a variable holding the texture unit ID
     *        DX11: NULL
     *        Metal: int* pointing to a variable holding the texture unit ID
     */
    const void* textureUnitData;
} VuRenderVideoBackgroundData;

/// \brief Retrieve Render Controller to get access to rendering-specific functionality in Engine
VU_API VuResult VU_API_CALL vuEngineGetRenderController(const VuEngine* engine, VuController** controller);

/// \brief Set the current view information
VU_API VuResult VU_API_CALL vuRenderControllerSetRenderViewConfig(VuController* controller, const VuRenderViewConfig* renderViewConfig);

/// \brief Get the current view information
/**
 * \returns VU_FAILED if no render view config is set
 */
VU_API VuResult VU_API_CALL vuRenderControllerGetRenderViewConfig(const VuController* controller, VuRenderViewConfig* renderViewConfig);

/// \brief Get the video background viewport (its location/size on screen)
/**
 * \returns VU_FAILED if no viewport is available
 */
VU_API VuResult VU_API_CALL vuRenderControllerGetVideoBackgroundViewport(const VuController* controller, VuVector4I* vbViewport);

/// \brief Configure the video background viewport (its location/size on screen)
/**
 * \note A VuRenderViewConfig has to be set first, otherwise this call will fail.
 *
 * \note The last viewport configuration applied with either vuRenderControllerSetVideoBackgroundViewport() or
 * vuRenderControllerSetVideoBackgroundViewportMode() will override any previously set viewport
 */
VU_API VuResult VU_API_CALL vuRenderControllerSetVideoBackgroundViewport(VuController* controller, const VuVector4I* vbViewport);

/// \brief Configure the video background viewport mode
/**
 * \note A VuRenderViewConfig has to be set first, otherwise this call will fail
 *
 * \note The last viewport configuration applied with either vuRenderControllerSetVideoBackgroundViewport() or
 * vuRenderControllerSetVideoBackgroundViewportMode() will override any previously set viewport
 */
VU_API VuResult VU_API_CALL vuRenderControllerSetVideoBackgroundViewportMode(VuController* controller,
                                                                             VuVideoBackgroundViewportMode vbMode);

/// \brief Get video background view information for rendering view background
/**
 * \note This function can only be called after Engine has started, otherwise it fails.
 * \note Before calling this function, the render view must be configured by calling vuRenderControllerSetRenderViewConfig, otherwise this
 * call may fail.
 *
 * \warning The behaviour of this function will change in an upcoming release.
 */
VU_API VuResult VU_API_CALL vuRenderControllerGetVideoBackgroundViewInfo(const VuController* controller,
                                                                         VuVideoBackgroundViewInfo* viewInfo);

/// \brief Update the texture data to use for rendering the video background
VU_API VuResult VU_API_CALL vuRenderControllerUpdateVideoBackgroundTexture(VuController* controller, const VuState* state,
                                                                           const VuRenderVideoBackgroundData* renderVBData);

/// \brief Set the current near/far plane to be used for the projection matrix reported in RenderState
VU_API VuResult VU_API_CALL vuRenderControllerSetProjectionMatrixNearFar(VuController* controller, float nearPlane, float farPlane);

/// \brief Get the current near/far values reported in RenderState
VU_API VuResult VU_API_CALL vuRenderControllerGetProjectionMatrixNearFar(const VuController* controller, float* nearPlane, float* farPlane);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_RENDERCONTROLLER_H_
