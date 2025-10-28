#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texcor;
//layour (location = 3) in vec3 normal;
out vec3 verColor;
out vec2 TexCoord;
out vec3 actualLight;
uniform mat4 umat4Local;
uniform mat4 umat4Model;
uniform mat4 umat4View;
uniform mat4 umat4Proj;
uniform vec3 uvec3LightColor;
uniform vec3 uvec3EnvLight;
uniform vec3 uvec3LightPos;
uniform vec3 debug_ver_color;
uniform vec3 key_in;

void main()
{
	vec2 fixedcoord = vec2(texcor.x,1.0-texcor.y);
	gl_Position = umat4Proj*umat4View*umat4Model*umat4Local*vec4(position, 1.0f);
	vec4 augmented = vec4(position, 1.0f);
	vec4 pos = umat4Model*(umat4Local*augmented);
	verColor.x = 1.0f;
	verColor.y = 1.0f;
	verColor.z = 1.0f;
	//verColor.x = verColor.x*debug_ver_color.x;
	//verColor.y = verColor.y*debug_ver_color.y;
	//verColor.z = verColor.z*debug_ver_color.z;
	TexCoord = fixedcoord;
	float distance = (pos.x-uvec3LightPos.x)*(pos.x-uvec3LightPos.x)+(pos.y-uvec3LightPos.y)*(pos.y-uvec3LightPos.y)+(pos.z-uvec3LightPos.z)*(pos.z-uvec3LightPos.z);
	float k = key_in.x*15.0f/(distance);
	vec3 light_color;
	light_color = k*uvec3LightColor;
	float strength_light = 0.299f*light_color.x*light_color.x+0.201f*light_color.y*light_color.y+0.400f*light_color.z*light_color.z;
	float strength_uvec3EnvLight = 0.299f*uvec3EnvLight.x*uvec3EnvLight.x+0.201f*uvec3EnvLight.y*uvec3EnvLight.y+0.400f*uvec3EnvLight.z*uvec3EnvLight.z;
	float minus;
	if(strength_uvec3EnvLight>=strength_light)
	{
		minus = strength_uvec3EnvLight-strength_light;
		float power = 1.0f/(1.0f+minus);
		actualLight = power*light_color+(1.0f-power)*uvec3EnvLight;
	}
	else
	{
		minus = strength_light-strength_uvec3EnvLight;
		float power = 1.0f/(1.0f+minus);
		actualLight = power*uvec3EnvLight+(1.0f-power)*light_color;
	}
}