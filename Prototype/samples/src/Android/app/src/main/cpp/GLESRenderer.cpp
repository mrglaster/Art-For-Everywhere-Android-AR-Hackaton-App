/*===============================================================================
Copyright (c) 2020 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#include "GLESRenderer.h"

#include "GLESUtils.h"
#include "Shaders.h"

#include <MemoryStream.h>
#include <Models.h>

#include <android/asset_manager.h>

bool
GLESRenderer::init(AAssetManager* assetManager)
{
    // Setup for Video Background rendering
    mVbShaderProgramID = GLESUtils::createProgramFromBuffer(textureVertexShaderSrc, textureFragmentShaderSrc);
    mVbVertexPositionHandle = glGetAttribLocation(mVbShaderProgramID, "vertexPosition");
    mVbTextureCoordHandle = glGetAttribLocation(mVbShaderProgramID, "vertexTextureCoord");
    mVbMvpMatrixHandle = glGetUniformLocation(mVbShaderProgramID, "modelViewProjectionMatrix");
    mVbTexSampler2DHandle = glGetUniformLocation(mVbShaderProgramID, "texSampler2D");

    // Setup for augmentation rendering
    mUniformColorShaderProgramID = GLESUtils::createProgramFromBuffer(uniformColorVertexShaderSrc, uniformColorFragmentShaderSrc);
    mUniformColorVertexPositionHandle = glGetAttribLocation(mUniformColorShaderProgramID, "vertexPosition");
    mUniformColorMvpMatrixHandle = glGetUniformLocation(mUniformColorShaderProgramID, "modelViewProjectionMatrix");
    mUniformColorColorHandle = glGetUniformLocation(mUniformColorShaderProgramID, "uniformColor");

    // Setup for guide view rendering
    mTextureUniformColorShaderProgramID = GLESUtils::createProgramFromBuffer(textureColorVertexShaderSrc, textureColorFragmentShaderSrc);
    mTextureUniformColorVertexPositionHandle = glGetAttribLocation(mTextureUniformColorShaderProgramID, "vertexPosition");
    mTextureUniformColorTextureCoordHandle = glGetAttribLocation(mTextureUniformColorShaderProgramID, "vertexTextureCoord");
    mTextureUniformColorMvpMatrixHandle = glGetUniformLocation(mTextureUniformColorShaderProgramID, "modelViewProjectionMatrix");
    mTextureUniformColorTexSampler2DHandle = glGetUniformLocation(mTextureUniformColorShaderProgramID, "texSampler2D");
    mTextureUniformColorColorHandle = glGetUniformLocation(mTextureUniformColorShaderProgramID, "uniformColor");

    // Setup for axis rendering
    mVertexColorShaderProgramID = GLESUtils::createProgramFromBuffer(vertexColorVertexShaderSrc, vertexColorFragmentShaderSrc);
    mVertexColorVertexPositionHandle = glGetAttribLocation(mVertexColorShaderProgramID, "vertexPosition");
    mVertexColorColorHandle = glGetAttribLocation(mVertexColorShaderProgramID, "vertexColor");
    mVertexColorMvpMatrixHandle = glGetUniformLocation(mVertexColorShaderProgramID, "modelViewProjectionMatrix");

    mModelTargetGuideViewTextureUnit = -1;

    std::vector<char> data; // for reading model files

    // Load Astronaut model
    {
        if (!readAsset(assetManager, "Venus_01.obj", data))
        {
            return false;
        }
        if (!loadObjModel(data, mAstronautVertexCount, mAstronautVertices, mAstronautTexCoords))
        {
            return false;
        }
        data.clear();
        mAstronautTextureUnit = -1;
        if (!readAsset(assetManager, "plane.obj", data))
        {
            return false;
        }
        if (!loadObjModel(data, mPlaneVertexCount, mPlaneVertices, mPlaneTexCoords))
        {
            return false;
        }
        data.clear();
        mPlaneTextureUnit = -1;
    }

    // Load Lander model
    {
        if (!readAsset(assetManager, "VikingLander.obj", data))
        {
            return false;
        }
        if (!loadObjModel(data, mLanderVertexCount, mLanderVertices, mLanderTexCoords))
        {
            return false;
        }
        data.clear();
        mLanderTextureUnit = -1;
    }

    return true;
}


void
GLESRenderer::deinit()
{
    if (mModelTargetGuideViewTextureUnit != -1)
    {
        GLESUtils::destroyTexture(mModelTargetGuideViewTextureUnit);
        mModelTargetGuideViewTextureUnit = -1;
    }
    if (mAstronautTextureUnit != -1)
    {
        GLESUtils::destroyTexture(mAstronautTextureUnit);
        mAstronautTextureUnit = -1;
    }
    if (mLanderTextureUnit != -1)
    {
        GLESUtils::destroyTexture(mLanderTextureUnit);
        mLanderTextureUnit = -1;
    }
}


void
GLESRenderer::setAstronautTexture(int width, int height, unsigned char* bytes)
{
    createTexture(width, height, bytes, mAstronautTextureUnit);
}

void
GLESRenderer::setPlaneTexture(int width, int height, unsigned char* bytes)
{
    createTexture(width, height, bytes, mPlaneTextureUnit);
}


void
GLESRenderer::setLanderTexture(int width, int height, unsigned char* bytes)
{
    createTexture(width, height, bytes, mLanderTextureUnit);
}


void
GLESRenderer::renderVideoBackground(const VuMatrix44F& projectionMatrix, const float* vertices, const float* textureCoordinates,
                                    const int numTriangles, const unsigned int* indices, int textureUnit)
{
    GLboolean depthTest = GL_FALSE;
    GLboolean cullTest = GL_FALSE;

    glGetBooleanv(GL_DEPTH_TEST, &depthTest);
    glGetBooleanv(GL_CULL_FACE, &cullTest);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Load the shader and upload the vertex/texcoord/index data
    glUseProgram(mVbShaderProgramID);
    glVertexAttribPointer(static_cast<GLuint>(mVbVertexPositionHandle), 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(static_cast<GLuint>(mVbTextureCoordHandle), 2, GL_FLOAT, GL_FALSE, 0, textureCoordinates);

    glUniform1i(mVbTexSampler2DHandle, textureUnit);

    // Render the video background with the custom shader
    // First, we enable the vertex arrays
    glEnableVertexAttribArray(static_cast<GLuint>(mVbVertexPositionHandle));
    glEnableVertexAttribArray(static_cast<GLuint>(mVbTextureCoordHandle));

    // Pass the projection matrix to OpenGL
    glUniformMatrix4fv(mVbMvpMatrixHandle, 1, GL_FALSE, projectionMatrix.data);

    // Then, we issue the render call
    glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, indices);

    // Finally, we disable the vertex arrays
    glDisableVertexAttribArray(static_cast<GLuint>(mVbVertexPositionHandle));
    glDisableVertexAttribArray(static_cast<GLuint>(mVbTextureCoordHandle));

    if (depthTest)
        glEnable(GL_DEPTH_TEST);

    if (cullTest)
        glEnable(GL_CULL_FACE);

    GLESUtils::checkGlError("Render video background");
}


void
GLESRenderer::renderWorldOrigin(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix)
{
    VuVector3F axis10cmSize{ 0.1f, 0.1f, 0.1f };
    renderAxis(projectionMatrix, modelViewMatrix, axis10cmSize, 4.0f);
    VuVector4F cubeColor{ 0.8, 0.8, 0.8, 1.0 };
    renderCube(projectionMatrix, modelViewMatrix, 0.015f, cubeColor);
}


void
GLESRenderer::renderImageTarget(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, VuMatrix44F& scaledModelViewMatrix)
{
    VuMatrix44F scaledModelViewProjectionMatrix = vuMatrix44FMultiplyMatrix(projectionMatrix, scaledModelViewMatrix);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float stateLineWidth;
    glGetFloatv(GL_LINE_WIDTH, &stateLineWidth);

    glUseProgram(mUniformColorShaderProgramID);

    glVertexAttribPointer(mUniformColorVertexPositionHandle, 3, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)&squareVertices[0]);

    glEnableVertexAttribArray(mUniformColorVertexPositionHandle);

    glUniformMatrix4fv(mUniformColorMvpMatrixHandle, 1, GL_FALSE, &scaledModelViewProjectionMatrix.data[0]);

    // Draw translucent solid overlay
    // Color RGBA
    glUniform4f(mUniformColorColorHandle, 1.0, 0.0, 0.0, 0.1);
    glDrawElements(GL_TRIANGLES, NUM_SQUARE_INDEX, GL_UNSIGNED_SHORT, (const GLvoid*)&squareIndices[0]);

    // Draw solid outline
    glUniform4f(mUniformColorColorHandle, 1.0, 0.0, 0.0, 1.0);
    glLineWidth(4.0f);
    glDrawElements(GL_LINES, NUM_SQUARE_WIREFRAME_INDEX, GL_UNSIGNED_SHORT, (const GLvoid*)&squareWireframeIndices[0]);

    glDisableVertexAttribArray(mUniformColorVertexPositionHandle);

    GLESUtils::checkGlError("Render Image Target");

    glLineWidth(stateLineWidth);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    VuVector3F axis2cmSize{ 0.02f, 0.02f, 0.02f };
    renderAxis(projectionMatrix, modelViewMatrix, axis2cmSize, 4.0f);

    VuMatrix44F modelViewProjectionMatrix = vuMatrix44FMultiplyMatrix(projectionMatrix, modelViewMatrix);
    renderModel(modelViewProjectionMatrix, mAstronautVertexCount, mAstronautVertices.data(), mAstronautTexCoords.data(),
                mAstronautTextureUnit);
}


void
GLESRenderer::renderModelTarget(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, VuMatrix44F& /*scaledModelViewMatrix*/)
{
    VuMatrix44F modelViewProjectionMatrix = vuMatrix44FMultiplyMatrix(projectionMatrix, modelViewMatrix);

    renderModel(modelViewProjectionMatrix, mLanderVertexCount, mLanderVertices.data(), mLanderTexCoords.data(), mLanderTextureUnit);

    VuVector3F axis10cmSize{ 0.1f, 0.1f, 0.1f };
    renderAxis(projectionMatrix, modelViewMatrix, axis10cmSize, 4.0f);
}


void
GLESRenderer::renderModelTargetGuideView(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, const VuImageInfo& image,
                                         VuBool guideViewImageHasChanged)
{
    VuMatrix44F modelViewProjectionMatrix = vuMatrix44FMultiplyMatrix(projectionMatrix, modelViewMatrix);


    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);

    // The guide view image is updated if the device orientation changes.
    // This is indicated by the guideViewImageHasChanged flag. In that case,
    // recreate the texture with the latest content of the image.
    if (mModelTargetGuideViewTextureUnit == -1 || guideViewImageHasChanged == VU_TRUE)
    {
        // Free the previous texture
        if (mModelTargetGuideViewTextureUnit != -1)
        {
            GLESUtils::destroyTexture(mModelTargetGuideViewTextureUnit);
        }

        mModelTargetGuideViewTextureUnit = GLESUtils::createTexture(image);
    }
    glBindTexture(GL_TEXTURE_2D, mModelTargetGuideViewTextureUnit);

    glEnableVertexAttribArray(mTextureUniformColorVertexPositionHandle);
    glVertexAttribPointer(mTextureUniformColorVertexPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)&squareVertices[0]);

    glEnableVertexAttribArray(mTextureUniformColorTextureCoordHandle);
    glVertexAttribPointer(mTextureUniformColorTextureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)&squareTexCoords[0]);

    glUseProgram(mTextureUniformColorShaderProgramID);
    glUniformMatrix4fv(mTextureUniformColorMvpMatrixHandle, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix.data);
    glUniform4f(mTextureUniformColorColorHandle, 1.0f, 1.0f, 1.0f, 0.7f);
    glUniform1i(mTextureUniformColorTexSampler2DHandle, 0); // texture unit, not handle

    // Draw
    glDrawElements(GL_TRIANGLES, NUM_SQUARE_INDEX, GL_UNSIGNED_SHORT, (const GLvoid*)&squareIndices[0]);

    // disable input data structures
    glDisableVertexAttribArray(mTextureUniformColorTextureCoordHandle);
    glDisableVertexAttribArray(mTextureUniformColorVertexPositionHandle);
    glUseProgram(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLESUtils::checkGlError("Render guide view");

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}


void
GLESRenderer::createTexture(int width, int height, unsigned char* bytes, GLuint& textureId)
{
    if (textureId != -1)
    {
        GLESUtils::destroyTexture(textureId);
        textureId = -1;
    }
    textureId = GLESUtils::createTexture(width, height, bytes);
}


void
GLESRenderer::renderCube(const VuMatrix44F& projectionMatrix, const VuMatrix44F& modelViewMatrix, float scale, const VuVector4F& color)
{
    VuMatrix44F scaledModelViewMatrix;
    VuMatrix44F modelViewProjectionMatrix;
    VuVector3F scaleVec{ scale, scale, scale };

    scaledModelViewMatrix = vuMatrix44FScale(scaleVec, modelViewMatrix);
    modelViewProjectionMatrix = vuMatrix44FMultiplyMatrix(projectionMatrix, scaledModelViewMatrix);

    ///////////////////////////////////////////////////////////////
    // Render with const ambient diffuse light uniform color shader
    glEnable(GL_DEPTH_TEST);
    glUseProgram(mUniformColorShaderProgramID);

    glEnableVertexAttribArray(mUniformColorVertexPositionHandle);

    glVertexAttribPointer(mUniformColorVertexPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)&cubeVertices[0]);

    glUniformMatrix4fv(mUniformColorMvpMatrixHandle, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix.data);
    glUniform4f(mUniformColorColorHandle, color.data[0], color.data[1], color.data[2], color.data[3]);

    // Draw
    glDrawElements(GL_TRIANGLES, NUM_CUBE_INDEX, GL_UNSIGNED_SHORT, (const GLvoid*)&cubeIndices[0]);

    // disable input data structures
    glDisableVertexAttribArray(mUniformColorVertexPositionHandle);
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);

    GLESUtils::checkGlError("Render cube");
    ///////////////////////////////////////////////////////
}


void
GLESRenderer::renderAxis(const VuMatrix44F& projectionMatrix, const VuMatrix44F& modelViewMatrix, const VuVector3F& scale, float lineWidth)
{
    VuMatrix44F scaledModelViewMatrix;
    VuMatrix44F modelViewProjectionMatrix;

    scaledModelViewMatrix = vuMatrix44FScale(scale, modelViewMatrix);
    modelViewProjectionMatrix = vuMatrix44FMultiplyMatrix(projectionMatrix, scaledModelViewMatrix);

    ///////////////////////////////////////////////////////
    // Render with vertex color shader
    glEnable(GL_DEPTH_TEST);
    glUseProgram(mVertexColorShaderProgramID);

    glEnableVertexAttribArray(mVertexColorVertexPositionHandle);
    glVertexAttribPointer(mVertexColorVertexPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)&axisVertices[0]);

    glEnableVertexAttribArray(mVertexColorColorHandle);
    glVertexAttribPointer(mVertexColorColorHandle, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)&axisColors[0]);

    glUniformMatrix4fv(mVertexColorMvpMatrixHandle, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix.data);

    // Draw
    float stateLineWidth;
    glGetFloatv(GL_LINE_WIDTH, &stateLineWidth);

    glLineWidth(lineWidth);

    glDrawElements(GL_LINES, NUM_AXIS_INDEX, GL_UNSIGNED_SHORT, (const GLvoid*)&axisIndices[0]);

    // disable input data structures
    glDisableVertexAttribArray(mVertexColorVertexPositionHandle);
    glDisableVertexAttribArray(mVertexColorColorHandle);
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);

    glLineWidth(stateLineWidth);

    GLESUtils::checkGlError("Render axis");
    ///////////////////////////////////////////////////////
}


void
GLESRenderer::renderModel(VuMatrix44F modelViewProjectionMatrix, const int numVertices, const float* vertices,
                          const float* textureCoordinates, GLuint textureId)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(mTextureUniformColorShaderProgramID);

    glEnableVertexAttribArray(mTextureUniformColorVertexPositionHandle);
    glVertexAttribPointer(mTextureUniformColorVertexPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)vertices);

    glEnableVertexAttribArray(mTextureUniformColorTextureCoordHandle);
    glVertexAttribPointer(mTextureUniformColorTextureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)textureCoordinates);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glUniformMatrix4fv(mTextureUniformColorMvpMatrixHandle, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix.data);
    glUniform4f(mTextureUniformColorColorHandle, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform1i(mTextureUniformColorTexSampler2DHandle, 0); // texture unit, not handle

    // Draw
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    // disable input data structures
    glDisableVertexAttribArray(mTextureUniformColorTextureCoordHandle);
    glDisableVertexAttribArray(mTextureUniformColorVertexPositionHandle);
    glUseProgram(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLESUtils::checkGlError("Render model");

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}


bool
GLESRenderer::readAsset(AAssetManager* assetManager, const char* filename, std::vector<char>& data)
{
    LOG("Reading asset %s", filename);
    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_STREAMING);
    if (asset == nullptr)
    {
        LOG("Error opening asset file %s", filename);
        return false;
    }
    auto assetSize = AAsset_getLength(asset);
    data.reserve(assetSize);
    char buf[BUFSIZ];
    int nb_read = 0;
    while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
    {
        std::copy(&buf[0], &buf[BUFSIZ], std::back_inserter(data));
    }
    AAsset_close(asset);
    if (nb_read < 0)
    {
        LOG("Error reading asset file %s", filename);
        return false;
    }
    return true;
}


bool
GLESRenderer::loadObjModel(const std::vector<char>& data, int& numVertices, std::vector<float>& vertices, std::vector<float>& texCoords)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    MemoryInputStream aFileDataStream(data.data(), data.size());
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &aFileDataStream);
    if (!ret || !err.empty())
    {
        LOG("Error loading model (%s)", err.c_str());
        return false;
    }
    if (!warn.empty())
    {
        LOG("Warning loading model (%s)", warn.c_str());
    }

    numVertices = 0;
    vertices.clear();
    texCoords.clear();
    // Loop over shapes
    // s is the index into the shapes vector
    // f is the index of the current face
    // v is the index of the current vertex
    for (size_t s = 0; s < shapes.size(); ++s)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            int fv = shapes[s].mesh.num_face_vertices[f];
            numVertices += fv;

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; ++v)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);

                // The model may not have texture coordinates for every vertex
                // If a texture coordinate is missing we just set it to 0,0
                // This may not be suitable for rendering some OBJ model files
                if (idx.texcoord_index < 0)
                {
                    texCoords.push_back(0.f);
                    texCoords.push_back(0.f);
                }
                else
                {
                    texCoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    texCoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
            }
            index_offset += fv;
        }
    }
    return true;
}
