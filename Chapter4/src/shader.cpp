#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "common/shader.hpp"

std::string ReadSourceFile(const char* a_path)
{
    std::string l_code;
    std::ifstream l_fileStream(a_path, std::ios::in);
    if (l_fileStream.is_open())
    {
        std::string l_line = "";
        while(getline(l_fileStream, l_line))
            l_code += "\n" + l_line;
        l_fileStream.close();
        return l_code;
    }
    else
    {
        printf("Failed to open \"%s\".\n", a_path);
        return "";
    }
}

bool CompileShader(const std::string& a_programCode, const GLuint a_shaderId)
{
    GLint l_result = GL_FALSE;
    int l_infologLength(0);
    char const * l_programCodePtr = a_programCode.c_str();
    glShaderSource(a_shaderId, 1, &l_programCodePtr, NULL);
    glCompileShader(a_shaderId);

    // check the shader for successful compile
    glGetShaderiv(a_shaderId, GL_COMPILE_STATUS, &l_result);
    glGetShaderiv(a_shaderId, GL_INFO_LOG_LENGTH, &l_infologLength);

    if (l_infologLength > 0)
    {
        std::vector<char> l_errMsg(l_infologLength+1);
        glGetShaderInfoLog(a_shaderId, l_infologLength, NULL, &l_errMsg[0]);
        printf("Error compiling shader [%s] error: `%s`\n", a_programCode.c_str(), &l_errMsg[0]);
        return false;
    }
    return true;
}

GLuint LoadShaders(const char* a_vertexShaderPath, const char* a_fragmentShaderPath)
{
    GLuint l_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint l_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    std::string l_vertexShaderCode = ReadSourceFile(a_vertexShaderPath);
    if (l_vertexShaderCode.empty())
    {
        return 0;
    }

    std::string l_fragmentShaderCode = ReadSourceFile(a_fragmentShaderPath);
    if (l_fragmentShaderCode.empty())
    {
        return 0;
    }

    printf("Compiling vertex shader %s\n", a_vertexShaderPath);
    CompileShader(l_vertexShaderCode, l_vertexShaderId);

    printf("Compiling fragment shader %s\n", a_fragmentShaderPath);
    CompileShader(l_fragmentShaderCode, l_fragmentShaderId);

    GLint l_result = GL_FALSE;
    int l_infologLength(0);
    printf("Linking program...\n");

    GLuint l_programId = glCreateProgram();
    glAttachShader(l_programId, l_vertexShaderId);
    glAttachShader(l_programId, l_fragmentShaderId);
    glLinkProgram(l_programId);

    // check for errors
    glGetProgramiv(l_programId, GL_LINK_STATUS, &l_result);
    glGetProgramiv(l_programId, GL_INFO_LOG_LENGTH, &l_infologLength);
    if (l_infologLength > 0)
    {
        std::vector<char> l_errMsg(l_infologLength+1);
        glGetProgramInfoLog(l_programId, l_infologLength, NULL, &l_errMsg[0]);
        printf("Error linking shaders error: `%s`\n", &l_errMsg[0]);
    }
    else
    {
        printf("Linked successfully\n");
    }

    // flag for delete, and will free all memories
    // when the attached program is deleted
    glDeleteShader(l_vertexShaderId);
    glDeleteShader(l_fragmentShaderId);
    return l_programId;
}
