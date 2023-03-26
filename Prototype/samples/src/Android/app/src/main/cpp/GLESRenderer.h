/*===============================================================================
Copyright (c) 2022 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VUFORIA_GLESRENDERER_H_
#define _VUFORIA_GLESRENDERER_H_

// clang-format off
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
// clang-format on

#include <android/asset_manager.h>

#include <tiny_obj_loader.h>

#include <VuforiaEngine/VuforiaEngine.h>

#include <vector>


/// Class to encapsulate OpenGLES rendering for the sample
class GLESRenderer
{
public:
    /// Initialize the renderer ready for use
    bool init(AAssetManager* assetManager);
    /// Clean up objects created during rendering
    void deinit();

    void setAstronautTexture(int width, int height, unsigned char* bytes);
    void setPlaneTexture(int width, int height, unsigned char* bytes);
    void setLanderTexture(int width, int height, unsigned char* bytes);

    /// Render the video background
    void renderVideoBackground(const VuMatrix44F& projectionMatrix, const float* vertices, const float* textureCoordinates,
                               const int numTriangles, const unsigned int* indices, int textureUnit);

    /// Render augmentation for the world origin
    void renderWorldOrigin(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix);

    /// Render a bounding box augmentation on an Image Target
    void renderImageTarget(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, VuMatrix44F& scaledModelViewMatrix);

    /// Render a bounding cube augmentation on a Model Target
    void renderModelTarget(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, VuMatrix44F& scaledModelViewMatrix);

    /// Render the Guide View for a Model Target
    void renderModelTargetGuideView(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, const VuImageInfo& Image,
                                    VuBool guideViewImageHasChanged);

private: // methods
    /// Attempt to create a texture from bytes
    /// If the value of textureId is not -1 it is assumed that it refers to an existing texture
    /// that should be destroyed and replaced with a new one.
    void createTexture(int width, int height, unsigned char* bytes, GLuint& textureId);

    /// Render a filled 3D cube
    /*
     * by default the cube is centered in 0.0 and has a unit size ([-0.5;0.5] on every axis)
     * projection and modelViewMatrix define the transformation of the model
     * scale defines the size of the cube (implemented as pre-transformation)
     * color will be used for rendering the model
     */
    void renderCube(const VuMatrix44F& projectionMatrix, const VuMatrix44F& modelViewMatrix, float scale, const VuVector4F& color);

    /// Render 3D Axes
    /*
     * red line is x unit vector, green line is y unit vector, blue line is z unit vector
     * projection and modelViewMatrix define the transformation of the model
     * scale defines a 3D scale of the model (implemented as pre-transformation)
     * lineWidth defines the width of the rendering line style
     */
    void renderAxis(const VuMatrix44F& projectionMatrix, const VuMatrix44F& modelViewMatrix, const VuVector3F& scale,
                    float lineWidth = 2.0f);

    /// Render a 3D model
    void renderModel(VuMatrix44F modelViewProjectionMatrix, const int numVertices, const float* vertices, const float* textureCoordinates,
                     GLuint textureId);

    /// Read an asset file into a byte vector
    bool readAsset(AAssetManager* assetManager, const char* filename, std::vector<char>& data);

    /// Load a model from an OBJ file
    /*
     * The model is input as the data array, the vertex and texture coordinate
     * vectors are populated by this method as it reads the input.
     */
    bool loadObjModel(const std::vector<char>& data, int& numVertices, std::vector<float>& vertices, std::vector<float>& texCoords);

private: // data members
    // For video background rendering
    GLuint mVbShaderProgramID = 0;
    GLint mVbVertexPositionHandle = 0;
    GLint mVbTextureCoordHandle = 0;
    GLint mVbMvpMatrixHandle = 0;
    GLint mVbTexSampler2DHandle = 0;

    // For augmentation rendering
    GLuint mUniformColorShaderProgramID = 0;
    GLint mUniformColorVertexPositionHandle = 0;
    GLint mUniformColorMvpMatrixHandle = 0;
    GLint mUniformColorColorHandle = 0;

    // For Model Target guide view rendering
    GLuint mTextureUniformColorShaderProgramID = 0;
    GLint mTextureUniformColorVertexPositionHandle = 0;
    GLint mTextureUniformColorTextureCoordHandle = 0;
    GLint mTextureUniformColorMvpMatrixHandle = 0;
    GLint mTextureUniformColorTexSampler2DHandle = 0;
    GLint mTextureUniformColorColorHandle = 0;
    GLuint mModelTargetGuideViewTextureUnit = -1;

    // For axis rendering
    GLuint mVertexColorShaderProgramID = 0;
    GLint mVertexColorVertexPositionHandle = 0;
    GLint mVertexColorColorHandle = 0;
    GLint mVertexColorMvpMatrixHandle = 0;

    // For rendering the Astronaut, loaded from the obj file
    int mAstronautVertexCount;
    std::vector<float> mAstronautVertices;
    std::vector<float> mAstronautTexCoords;
    GLuint mAstronautTextureUnit = -1;

    int mPlaneVertexCount;
    std::vector<float> mPlaneVertices;
    std::vector<float> mPlaneTexCoords;
    GLuint mPlaneTextureUnit = -1;

    // For rendering the Lander, loaded from the obj file
    int mLanderVertexCount;
    std::vector<float> mLanderVertices;
    std::vector<float> mLanderTexCoords;
    GLuint mLanderTextureUnit = -1;
};

#endif //_VUFORIA_GLESRENDERER_H_
