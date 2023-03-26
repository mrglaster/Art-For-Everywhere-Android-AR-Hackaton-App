/*===============================================================================
Copyright (c) 2020 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VUFORIA_SHADERS_H_
#define _VUFORIA_SHADERS_H_

/////////////////////////////////////////////////////////////////////////////////////////
// texture shader: vertexTexCoord in vertex shader, texture2D sample
/////////////////////////////////////////////////////////////////////////////////////////
static const char* textureVertexShaderSrc = R"(
    attribute vec4 vertexPosition;
    attribute vec2 vertexTextureCoord;

    uniform mat4 modelViewProjectionMatrix;

    varying vec2 texCoord;

    void main()
    {
        gl_Position = modelViewProjectionMatrix * vertexPosition;
        texCoord = vertexTextureCoord;
    }
)";


static const char* textureFragmentShaderSrc = R"(
    precision mediump float;

    uniform sampler2D texSampler2D;

    varying vec2 texCoord;

    void main()
    {
        gl_FragColor = texture2D(texSampler2D, texCoord);
    }
)";


/////////////////////////////////////////////////////////////////////////////////////////
// texture color shader: vertexTexCoord in vertex shader, uniform color, texture2D sample
/////////////////////////////////////////////////////////////////////////////////////////
static const char* textureColorVertexShaderSrc = R"(
    attribute vec4 vertexPosition;
    attribute vec2 vertexTextureCoord;

    uniform mat4 modelViewProjectionMatrix;

    varying vec2 texCoord;

    void main()
    {
        gl_Position = modelViewProjectionMatrix * vertexPosition;
        texCoord = vertexTextureCoord;
    }
)";


static const char* textureColorFragmentShaderSrc = R"(
    precision mediump float;

    uniform sampler2D texSampler2D;

    varying vec2 texCoord;

    uniform vec4 uniformColor;

    void main()
    {
        vec4 texColor = texture2D(texSampler2D, texCoord);
        gl_FragColor = texColor * uniformColor;
    }
)";


/////////////////////////////////////////////////////////////////////////////////////////
// uniform color shader: uniform color in frag shader
/////////////////////////////////////////////////////////////////////////////////////////
static const char* uniformColorVertexShaderSrc = R"(
    attribute vec4 vertexPosition;

    uniform mat4 modelViewProjectionMatrix;

    void main()
    {
        gl_Position = modelViewProjectionMatrix * vertexPosition;
    }
)";


static const char* uniformColorFragmentShaderSrc = R"(
    precision mediump float;

    uniform vec4 uniformColor;

    void main()
    {
        gl_FragColor = uniformColor;
    }
)";


/////////////////////////////////////////////////////////////////////////////////////////
// vertex color shader: attribute color in vertex shader
/////////////////////////////////////////////////////////////////////////////////////////
static const char* vertexColorVertexShaderSrc = R"(
    attribute vec4 vertexPosition;
    attribute vec4 vertexColor;

    uniform mat4 modelViewProjectionMatrix;

    // Color to use per vertex, linear interpolated down at fragment shader
    varying vec4 color;

    void main()
    {
        gl_Position = modelViewProjectionMatrix * vertexPosition;
        color = vertexColor;
    }
)";

static const char* vertexColorFragmentShaderSrc = R"(
    precision mediump float;

    varying vec4 color;

    void main()
    {
        gl_FragColor = color;
    }
)";

#endif // _VUFORIA_SHADERS_H_
