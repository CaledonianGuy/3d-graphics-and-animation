#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec4 FragPos;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    FragPos = model_matrix * vec4(aPos, 1.0);
    gl_Position = proj_matrix * view_matrix * FragPos;

    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(model_matrix))) * aNormal;
}
