#version 430

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform sampler2D shadowTexture;

uniform sampler2D gainTexture;

uniform mat4 umat4LightView;
uniform mat4 umat4LightProj;

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

vec2 calcShadowAndGain(vec3 fragPosWorldSpace){
    vec4 fragPosLightSpace = umat4LightProj * umat4LightView * vec4(fragPosWorldSpace, 1.0);
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowTexture, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float shadow = currentDepth - 0.005 > closestDepth  ? 1.0f : 0.0f;

    float gain = currentDepth - 0.005 < closestDepth  ? 0.2f : 0.0f;

    return vec2(shadow, gain);
}

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

    vec2 shadowAndGain = calcShadowAndGain(fragPos);
    float shadow = shadowAndGain.x;
    float gain = shadowAndGain.y;

    result = mix(result, result * 0.3, shadow); // In shadow, reduce light to 30%

    vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 gainVec = white;
    gainVec.x = gainVec.x*gain;
    gainVec.y = gainVec.y*gain;
    gainVec.z = gainVec.z*gain;

    fragColor = vec4(result, 1.0);
}