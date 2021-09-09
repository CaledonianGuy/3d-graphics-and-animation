#version 430 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec4 FragPos;

layout (binding = 0) uniform sampler2D tex;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ka;                   // Ambient constant
uniform float kd;                   // Diffuse constant
uniform float ks;                   // Specular constant
uniform float shine;                // Shine constant
uniform vec3 lightColor;            // Light Colour

void main(void)
{
    // Ambient
    vec3 ambient = ka * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec4 lightDir = normalize(vec4(lightPos, 1.0) - FragPos);
    float diff = max(dot(vec4(norm, 1.0), lightDir), 0.0);
    vec3 diffuse = kd * diff * lightColor;

    // Specular
    vec4 viewDir = normalize(vec4(viewPos, 1.0) - FragPos);
    vec4 reflectDir = reflect(-lightDir, vec4(norm, 1.0));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shine);
    vec3 specular = ks * spec * lightColor;

    // Result
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0) * (texture(tex, TexCoord) * 1.0);
}
