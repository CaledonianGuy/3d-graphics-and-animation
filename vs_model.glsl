#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec4 FragPos;

uniform vec3 modelPos;
uniform vec3 modelRot;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

mat4 scaleMatrix(float scale) {
    
    return mat4(scale,  0.0,    0.0,    0.0,
                0.0,    scale,  0.0,    0.0,
                0.0,    0.0,    scale,  0.0,
                0.0,    0.0,    0.0,    1.0);
}

mat4 translationMatrix(vec3 axis) {

    //matrices need to put in as the transpose of the sought matrix (i.e. sought matrix M, in put M^T)
    return mat4(1.0,    0.0,    0.0,    0.0,
                0.0,    1.0,    0.0,    0.0,
                0.0,    0.0,    1.0,    0.0,
                axis.x, axis.y, axis.z, 1.0);
}

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y + axis.z * s,  oc * axis.z * axis.x - axis.y * s,  0.0,
                oc * axis.x * axis.y - axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z + axis.x * s,  0.0,
                oc * axis.z * axis.x + axis.y * s,  oc * axis.y * axis.z - axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main(void)
{
    mat4 tranXYZ = translationMatrix(vec3(((gl_InstanceID + 1) * 0.25), ((gl_InstanceID + 1) * 0.25), ((gl_InstanceID + 1) * 0.25)));
    mat4 tranY = translationMatrix(vec3(0.0, 0.0, 0.0));
    mat4 tranZ = translationMatrix(vec3(0.0, 0.0, 0.0));

    mat4 rotX = rotationMatrix(vec3(1.0, 0.0, 0.0), 0.0);
    mat4 rotY = rotationMatrix(vec3(0.0, 1.0, 0.0), (gl_InstanceID + 1));
    mat4 rotZ = rotationMatrix(vec3(0.0, 0.0, 1.0), 0.0);

    FragPos = model_matrix * vec4(aPos, 1.0);
    gl_Position = proj_matrix * view_matrix * FragPos;

    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(model_matrix))) * aNormal;
}
