#version 430

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform sampler2D shadowTexture;
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

vec4 calcLightedColor(){
    vec3 fragPos = texture(gPosition, tCoord).rgb;
    vec3 normal = normalize(texture(gNormal, tCoord).rgb);
    vec3 albedo = texture(gAlbedoSpec, tCoord).rgb;
    float specularStrength = texture(gAlbedoSpec, tCoord).a;

    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0);
    vec3 vamb = vec3(0.0);
    vec3 vdiff = vec3(0.0);
    vec3 vspec = vec3(0.0);
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

        //result += ambient + diffuse + specular;
        vamb +=ambient;
        vdiff +=diffuse;
        vspec +=specular;
    }
    result = vamb + vdiff + vspec;
    vec3 fragPosWorldSpace = texture(gPosition, tCoord).rgb;
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
    float shadow = currentDepth - 0.0005f> closestDepth  ? 1.0f : 0.0f;
    //result = mix(result, result * 0.2, shadow); // In shadow, reduce light to 20%

    if(shadow>0.5f){
        result = vamb+vdiff;
        result = mix(result, result * 0.2, shadow); // In shadow, reduce light to 20%
    }
    else{
        result = vamb + vdiff + vspec;
        result = mix(result, result * 0.2, shadow); // In shadow, reduce light to 20%
    }

    return vec4(result, 1.0);
}

void main() {
    fragColor = calcLightedColor();
}