#version 300 es
precision mediump float;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    // sampler2D normal;
    float shininess;
};

struct Light {
    vec3 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec4 ambient = texture(material.diffuse, TexCoords);
    if(ambient.a < 0.01)
        discard;
    ambient *= light.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = light.specular * spec * texture(material.specular, TexCoords);

    // vec4 emission = texture(material.normal, TexCoords);

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance
                + light.quadratic * (distance * distance));

    attenuation = max(attenuation, 0.2);
    ambient.xyz  *= attenuation;
    diffuse.xyz  *= attenuation;
    specular.xyz *= attenuation;
    // emission.xyz *= attenuation;

    vec4 result = ambient + diffuse + specular;
    FragColor = result;
}
