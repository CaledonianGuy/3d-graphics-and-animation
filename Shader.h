// This code has mostly been taken from someone else's work, this code is open source and is part of learnopengl.com. Reference and licences are mentioned below.
// de Vries, J https://twitter.com/JoeyDeVriez (2014) shader.h source code (Version 1.0 +) [Source Code] https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
// Licences: Abridged: https://creativecommons.org/licenses/by-nc/4.0/ Full: https://creativecommons.org/licenses/by-nc/4.0/legalcode

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

class Shader
{
public:
	GLuint          ID;
    GLuint          texture1;
    void shaderLink(string vertexPath, string fragmentPath) 
    {
        // Read source code and compile shaders
        // ------------------------------------------------------------------------
        // vertex shader
        string vertexCode = readShader(vertexPath);
        const char* vShaderCode = vertexCode.c_str();
        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkShaderCompileErrors(vertex, "VERTEX");
        // fragment shader
        string fragmentCode = readShader(fragmentPath);
        const char* fShaderCode = fragmentCode.c_str();
        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkShaderCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkShaderCompileErrors(ID, "PROGRAM");
        // delete shaders as linked to program and no longer required
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // Activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // Utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // Load texture - only does one texture needs to be extended
    // ------------------------------------------------------------------------ 
    void loadTexture(string name)
    {
        // Load and create a texture
        // -----------------------------
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1); // All upcoming operations now have effect on this texture object
        // Set texture wrapping parameters
        // -----------------------------
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        // -----------------------------
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Load image Information
        // -----------------------------
        int iWidth, iHeight, iChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* iData = stbi_load(name.c_str(), &iWidth, &iHeight, &iChannels, 0);
        // Switch from RGB to RGBA in case there is a fourth channel
        // -----------------------------
        GLenum formatType = GL_RGB;
        if (iChannels == 4)
        {
            formatType = GL_RGBA;
        }
        if (iData)
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, formatType, GL_UNSIGNED_BYTE, iData);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            cout << "Failed to load texture" << endl;
        }
        // Bind textures and samplers - using 0 as I know there is only one texture - need to extend
        // -----------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        setInt("tex", texture1); // Send to GPU (Fragment Shader)
    }
private:
    // Read shader source code from file path
    // ------------------------------------------------------------------------
    string readShader(string name) {
        string vs_text;
        std::ifstream vs_file(name);

        string vs_line;
        if (vs_file.is_open()) {

            while (getline(vs_file, vs_line)) {
                vs_text += vs_line;
                vs_text += '\n';
            }
            vs_file.close();
        }
        return vs_text;
    }
    // Checking for compilation/linking errors
    // ------------------------------------------------------------------------
    void checkShaderCompileErrors(GLuint shader, string type) 
    {
        GLint success;
        GLchar infoLog[1024];
        //vector<GLchar> errorLog(maxLength);
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) 
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout << "--------------Shader compilation error-------------\n";
                cout << "Type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) 
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout << "---------------Program linking error------------\n";
                cout << "Type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
    }
};