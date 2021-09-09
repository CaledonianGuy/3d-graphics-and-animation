#version 430 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec4 FragPos;

//layout (binding = 0) uniform sampler2D tex;

uniform mat4 model_matrix;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 materialColor;

//uniform vec3 ia;                    // Ambient colour
uniform float ka;                   // Ambient constant
//uniform vec3 id;                    // Diffuse colour
//uniform float kd;                   // Diffuse constant
//uniform vec3 is;                    // Specular colour
uniform float ks;                   // Specular constant
uniform float shine;
uniform vec3 lightColor;

void main(void)
{
    // Ambient
    vec3 ambient = ka * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec4 lightDir = normalize(vec4(lightPos, 1.0) - FragPos);
    float diff = max(dot(vec4(norm, 1.0), lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec4 viewDir = normalize(vec4(viewPos, 1.0) - FragPos);
    vec4 reflectDir = reflect(-lightDir, vec4(norm, 1.0));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shine);
    vec3 specular = ks * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * materialColor; //texture(tex, TexCoord);
    FragColor = vec4(result, 1.0);
}
