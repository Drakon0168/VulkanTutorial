#version 450
#extension GL_ARB_separate_shader_objects: enable

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 vertColor;

void main(){
	mat4 mvp = ubo.projection * ubo.view * ubo.model;
	gl_Position = mvp * vec4(inPosition, 0.0f, 1.0f);
	vertColor = inColor;
}