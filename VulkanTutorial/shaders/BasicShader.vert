#version 450
#extension GL_ARB_separate_shader_objects: enable

struct Light{
	vec3 position;
	vec3 color;
	float range;
};

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 vertColor;
layout(location = 2) out vec2 uv;
layout(location = 3) out Light lights[1];

void main(){
	mat4 mvp = ubo.projection * ubo.view * ubo.model;
	gl_Position = mvp * vec4(inPosition, 1.0f);
	position = (ubo.model * vec4(inPosition, 1.0f)).xyz;
	lights = Light[](Light(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f,1.0f,1.0f), 2.0f));
	vertColor = inColor;
	uv = texCoord;
}