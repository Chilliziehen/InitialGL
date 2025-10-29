#version 430

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;

struct Light{
    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

uniform int numLights;
uniform Light lights[16];

out vec4 fragColor;

in vec2 tCoord; // 屏幕空间采样坐标 [0,1]

void main() {
    vec3 fragPos = texture(gPosition, tCoord).rgb;
    vec3 normal = normalize(texture(gNormal, tCoord).rgb);
    vec3 albedo = texture(gAlbedoSpec, tCoord).rgb;
    float specularStrength = texture(gAlbedoSpec, tCoord).a;

    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0);
    for(int i = 0; i < numLights; i++){
        // Ambient
        vec3 ambient = 0.1 * albedo;

        // Diffuse
        vec3 lightDir = normalize(lights[i].position - fragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * albedo * lights[i].color;

        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * lights[i].color;

        // Attenuation
        float distance = length(lights[i].position - fragPos);
        float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * (distance * distance));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        result += ambient + diffuse + specular;
    }

    fragColor = vec4(result, 1.0);
}