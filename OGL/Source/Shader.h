#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>




class Shader
{
public:
    // State
    GLuint ID;
    // Constructor
    Shader() { }

   
    Shader& Use();

    void Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource);
    void SetFloat(const GLchar* name, GLfloat value, GLboolean useShader);
    void SetInteger(const GLchar* name, GLint value, GLboolean useShader);
    void SetVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader);
    void SetVector2f(const GLchar* name, const glm::vec2& value, GLboolean useShader);
    void SetVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader);
    void SetVector3f(const GLchar* name, const glm::vec3& value, GLboolean useShader);
    void SetVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader);
    void SetVector4f(const GLchar* name, const glm::vec4& value, GLboolean useShader);
    void SetMatrix4(const GLchar* name, const glm::mat4& matrix, GLboolean useShader);
    void checkCompileErrors(GLuint object, std::string type);
};

#endif