/*===============================================================================
Copyright (c) 2021 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_CORE_H_
#define _VU_CORE_H_

/**
 * \file Core.h
 * \brief Core data structures, data types, constants and functions used by the Vuforia Engine
 */

#include <VuforiaEngine/Core/System.h>

#include <stdint.h> // Required for standard types (int32_t, etc.)

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup BasicTypeGroup Basic Vuforia Types
 * \{
 */

/// \brief Boolean value for true
#define VU_TRUE 1

/// \brief Boolean value for false
#define VU_FALSE 0

/// \brief Boolean type
typedef uint32_t VuBool;

/// \brief Error code type
typedef uint32_t VuErrorCode;

/// \brief Bitflag code type
typedef uint32_t VuFlags;

/// \brief Error code result from an operation
/**
 * \note When a function fails by returning VU_FAILED, check the function-specific error code in the respective out parameter.
 */
VU_ENUM(VuResult)
{
    VU_FAILED = 0x0, ///< Failed operation
    VU_SUCCESS = 0x1 ///< Successful operation
};

/// \brief Vuforia Controller handle
typedef struct VuController_ VuController;

/** \} */

// ======== VECTOR, MATRIX AND ROTATION DATA TYPES ========

/** \addtogroup MathTypeGroup Math data types
 * \{
 * Data types store vector, matrix and rotation angle for math operation
 */

/// \brief 4x4 matrix (float)
/**
 * \note Elements are stored in column-major order:
 * data[ 0], data[ 4], data[ 8], data[12]
 * data[ 1], data[ 5], data[ 9], data[13]
 * data[ 2], data[ 6], data[10], data[14]
 * data[ 3], data[ 7], data[11], data[15]
 *
 * When the matrix represents a pose, the Vuforia Engine uses the OpenGL column-major
 * matrix convention with a right-handed coordinate system on all platforms, devices
 * and rendering backends as a cross-platform representation:
 *
 * - X axis points to the right
 * - Y axis points upwards
 * - Z axis points towards viewer
 *
 *      Y+
 *      |
 *      |
 *      O-----X+
 *     /
 *    /
 *   Z+
 *
 * Rendering solutions using a backend with the same matrix convention as OpenGL can
 * use the matrix as is, while those with a different convention (e.g. DirectX) require
 * conversion before use.
 */
typedef struct VuMatrix44F
{
    /// \brief Data member for storing matrix values
    float data[16];
} VuMatrix44F;

/// \brief 3x3 matrix (float)
/**
 * \note Elements are stored in column-major order:
 * data[0], data[3], data[6]
 * data[1], data[4], data[7]
 * data[2], data[5], data[8]
 */
typedef struct VuMatrix33F
{
    /// \brief Data member for storing matrix values
    float data[9];
} VuMatrix33F;

/// \brief 2D vector (integer)
typedef struct VuVector2I
{
    /// \brief Data member for storing vector values
    int32_t data[2];
} VuVector2I;

/// \brief 2D vector (float)
typedef struct VuVector2F
{
    /// \brief Data member for storing vector values
    float data[2];
} VuVector2F;

/// \brief 3D vector (integer)
typedef struct VuVector3I
{
    /// \brief Data member for storing vector values
    int32_t data[3];
} VuVector3I;

/// \brief 3D vector (float)
typedef struct VuVector3F
{
    /// \brief Data member for storing vector values
    float data[3];
} VuVector3F;

/// \brief 4D vector (integer)
typedef struct VuVector4I
{
    /// \brief Data member for storing vector values
    int32_t data[4];
} VuVector4I;

/// \brief 4D vector (float)
typedef struct VuVector4F
{
    /// \brief Data member for storing vector values
    float data[4];
} VuVector4F;

/// \brief 8D vector (float)
typedef struct VuVector8F
{
    /// \brief Data member for storing vector values
    float data[8];
} VuVector8F;

/// \brief Rotation angle for camera intrinsics, rendering, etc.
VU_ENUM(VuRotation)
{
    VU_ROTATION_ANGLE_0 = 0x1,   ///< 0 degrees
    VU_ROTATION_ANGLE_90 = 0x2,  ///< 90 degrees
    VU_ROTATION_ANGLE_180 = 0x3, ///< 180 degrees
    VU_ROTATION_ANGLE_270 = 0x4  ///< 270 degrees
};

/// \brief Coordinates of a 2D rectangle
typedef struct VuRectangle
{
    /// \brief Coordinates of the rectangle's top-left corner
    VuVector2F topLeftCorner;

    /// \brief Coordinates of the rectangle's bottom-right corner
    VuVector2F bottomRightCorner;
} VuRectangle;

/** \} */

// ======== IMAGE DATA ========

/** \addtogroup ImageGroup Image Handling
 * \{
 */

// IMAGE PIXEL FORMAT SUPPORT

/// \brief Pixel format types supported by Vuforia
VU_ENUM(VuImagePixelFormat)
{
    VU_IMAGE_PIXEL_FORMAT_UNKNOWN = 0x1,   ///< Unknown pixel format.
    VU_IMAGE_PIXEL_FORMAT_RGB565 = 0x2,    ///< A color pixel stored in 2 bytes using 5 bits for red,
                                           ///< 6 bits for green and 5 bits for blue.
    VU_IMAGE_PIXEL_FORMAT_RGB888 = 0x3,    ///< A color pixel stored in 3 bytes using 8 bits each
                                           ///< for red, green and blue.
    VU_IMAGE_PIXEL_FORMAT_GRAYSCALE = 0x4, ///< A grayscale pixel stored in one byte.
    VU_IMAGE_PIXEL_FORMAT_RGBA8888 = 0x5,  ///< A color pixel stored in 4 bytes using 8 bits each
                                           ///< for red, green and blue and 8 bits for alpha channel.
    VU_IMAGE_PIXEL_FORMAT_NV21 = 0x6,      ///< YUV 4:2:0 with a plane of 8 bit Y (luma) samples followed by
                                           ///< an interleaved plane of 8 bit, 2x2 subsampled, V/U (chroma) samples.
    VU_IMAGE_PIXEL_FORMAT_NV12 = 0x7,      ///< YUV 4:2:0 with a plane of 8 bit Y (luma) samples followed by
                                           ///< an interleaved plane of 8 bit, 2x2 subsampled, U/V (chroma) samples.
    VU_IMAGE_PIXEL_FORMAT_YV12 = 0x8,      ///< YUV 4:2:0 with a plane of 8 bit Y (luma) samples followed by
                                           ///< a plane of 8 bit, 2x2 subsampled, V (chroma) samples followed by
                                           ///< a plane of 8 bit, 2x2 subsampled, U (chroma) samples.
    VU_IMAGE_PIXEL_FORMAT_YUV420P = 0x9,   ///< YUV 4:2:0 with a plane of 8 bit Y (luma) samples followed by
                                           ///< a plane of 8 bit, 2x2 subsampled, U (chroma) samples followed by
                                           ///< a plane of 8 bit, 2x2 subsampled, V (chroma) samples.
                                           ///< Note that this format is also known as I420
    VU_IMAGE_PIXEL_FORMAT_YUYV = 0xA       ///< YUV 4:2:2 with a single plane of interleaved 8 bit samples in YUYV order
                                           ///< where each pixel has a Y value and U, V values 2x1 subsampled.
                                           ///< Note that this format is also known as YUY2
};

// IMAGE PIXEL FORMAT LIST SUPPORT

/// \brief List of image pixel formats
typedef struct VuImagePixelFormatList_ VuImagePixelFormatList;

/// \brief Create an image pixel format list
VU_API VuResult VU_API_CALL vuImagePixelFormatListCreate(VuImagePixelFormatList** list);

/// \brief Get number of elements in an image pixel format list
VU_API VuResult VU_API_CALL vuImagePixelFormatListGetSize(const VuImagePixelFormatList* list, int32_t* listSize);

/// \brief Get an element in an image pixel format list
VU_API VuResult VU_API_CALL vuImagePixelFormatListGetElement(const VuImagePixelFormatList* list, int32_t element,
                                                             VuImagePixelFormat* format);

/// \brief Destroy an image pixel format list
VU_API VuResult VU_API_CALL vuImagePixelFormatListDestroy(VuImagePixelFormatList* list);

// IMAGE SUPPORT

/// \brief Vuforia Image
typedef struct VuImage_ VuImage;

/// \brief Vuforia Image List
typedef struct VuImageList_ VuImageList;

/// \brief Data structure describing image data
typedef struct VuImageInfo
{
    /// \brief Width of the image in pixels
    int32_t width;

    /// \brief Height of the image in pixels
    int32_t height;

    /// \brief Stride of the image in bytes
    int32_t stride;

    /// \brief Buffer width of the image in pixels
    int32_t bufferWidth;

    /// \brief Buffer height of the image in pixels
    int32_t bufferHeight;

    /// \brief Buffer size of the image in bytes
    int32_t bufferSize;

    /// \brief Image pixel format
    VuImagePixelFormat format;

    /// \brief Pixel buffer
    /**
     * \note The lifetime of the buffer is bound to the lifetime of the VuImage that was
     * used to retrieve this data.
     */
    const void* buffer;
} VuImageInfo;

/// \brief Get Image data
VU_API VuResult VU_API_CALL vuImageGetImageInfo(const VuImage* image, VuImageInfo* imageInfo);

/// \brief Acquire a new reference to the given image
VU_API VuResult VU_API_CALL vuImageAcquireReference(const VuImage* image, VuImage** imageOut);

/// \brief Release the given image
VU_API VuResult VU_API_CALL vuImageRelease(VuImage* image);

// IMAGE LIST SUPPORT

/// \brief Create an image list
VU_API VuResult VU_API_CALL vuImageListCreate(VuImageList** list);

/// \brief Get number of elements in an image list
VU_API VuResult VU_API_CALL vuImageListGetSize(const VuImageList* list, int32_t* numElements);

/// \brief Get an element in an image list
VU_API VuResult VU_API_CALL vuImageListGetElement(const VuImageList* list, int32_t element, VuImage** image);

/// \brief Append an element to an image list
VU_API VuResult VU_API_CALL vuImageListAppendElement(VuImageList* list, const VuImage* image);

/// \brief Destroy an image list
VU_API VuResult VU_API_CALL vuImageListDestroy(VuImageList* list);

/** \} */

// ======== CAMERA-RELATED DATA ========

/** \addtogroup CameraParameterGroup Camera-related Data Structures
 * \{
 */

/// \brief Camera distortion model
VU_ENUM(VuCameraDistortionMode)
{
    VU_CAMERA_DISTORTION_MODE_LINEAR = 0x1,  ///< Linear model (no distortion or undistortion)
    VU_CAMERA_DISTORTION_MODE_3PARAMS = 0x2, ///< 3 radial parameters, no tangential parameters
    VU_CAMERA_DISTORTION_MODE_4PARAMS = 0x3, ///< 2 radial parameters, plus 2 tangential parameters
    VU_CAMERA_DISTORTION_MODE_5PARAMS = 0x4  ///< 3 radial parameters, plus 2 tangential parameters
};

/// \brief Vuforia camera intrinsics
typedef struct VuCameraIntrinsics
{
    /// \brief Camera frame resolution in pixels
    VuVector2F size;

    /// \brief Focal length in both the x and y directions
    VuVector2F focalLength;

    /// \brief Principal point
    VuVector2F principalPoint;

    /// \brief Camera distortion mode
    VuCameraDistortionMode distortionMode;

    /// \brief Radial distortion coefficients
    VuVector8F distortionParameters;
} VuCameraIntrinsics;

/// \brief Get the associated field-of-view of camera intrinsics in degrees
/**
 * \note The function returns a zero vector upon an error.
 */
VU_API VuVector2F VU_API_CALL vuCameraIntrinsicsGetFov(const VuCameraIntrinsics* intrinsics);

/// \brief Get a 3x3 matrix of the camera intrinsics using a pinhole camera model
/**
 * \note The function returns a zero matrix upon an error.
 */
VU_API VuMatrix33F VU_API_CALL vuCameraIntrinsicsGetMatrix(const VuCameraIntrinsics* intrinsics);

/// \brief Create a perspective projection matrix from camera intrinsics data that is immediately suitable for rendering in OpenGL
/**
 * \note The projection matrix uses an OpenGL-style column-major matrix with the following right-handed
 * coordinate system convention for the view space:

 * - The X coordinate system axis points to the right and the Y axis points downwards.
 * - The camera is positioned at the coordinate system origin and points in the positive Z direction.
 * - Normalized device coordinates are used where the Z coordinates are normalized to the range (-1, 1).
 *
 * \param intrinsics Camera intrinsics
 * \param nearPlane Near clipping plane
 * \param farPlane Far clipping plane
 * \param rotation Rotation to apply to the projection matrix (e.g. can be used for baking screen rotation into the projection matrix)
 * \returns Projection matrix
 */
VU_API VuMatrix44F VU_API_CALL vuCameraIntrinsicsGetProjectionMatrix(const VuCameraIntrinsics* intrinsics, float nearPlane, float farPlane,
                                                                     VuRotation rotation);

/** \} */

// ======== RENDERING-RELATED DATA ========

/** \addtogroup RenderParameterGroup Rendering-specific Data Structures
 * \{
 */

/// \brief Vuforia Mesh
/**
 * A simple mesh representation that holds per-vertex data and face indices.
 * The face indices consist of integer triplets, where each triplet defines a triangle.
 */
typedef struct VuMesh
{
    /// \brief Number of vertices for the mesh
    int32_t numVertices;

    /// \brief Buffer for position coordinates for the mesh
    /**
     * \note Each position consists of three subsequent floats per vertex.
     */
    const float* pos;

    /// \brief Buffer for texture coordinates for the mesh
    /**
     * \note Each texture coordinate consists of two subsequent floats per vertex.
     * This buffer must be set to NULL if the mesh has no texture coordinates.
     */
    const float* tex;

    /// \brief Buffer for normal coordinates for the mesh
    /**
     * \note Each normal consists of three subsequent floats per vertex.
     * This buffer must be set to NULL if the mesh has no normal coordinates.
     */
    const float* normal;

    /// \brief Number of triangle primitives for the mesh
    int32_t numFaces;

    /// \brief Buffer for face indices for the mesh
    const uint32_t* faceIndices;
} VuMesh;

/// \brief Axis-aligned bounding box
typedef struct VuAABB
{
    /// \brief Center of bounding box
    VuVector3F center;

    /// \brief Half-extent of bounding box (from center point to corner point)
    VuVector3F extent;
} VuAABB;

/// \brief Get minimum value from axis-aligned bounding box
VU_API VuVector3F VU_API_CALL vuAABBMin(const VuAABB* aabb);

/// \brief Get maximum value from axis-aligned bounding box
VU_API VuVector3F VU_API_CALL vuAABBMax(const VuAABB* aabb);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_CORE_H_
