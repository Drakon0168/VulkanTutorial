#version 450
#extension GL_KHR_vulkan_glsl: enable

//Set the positions of the vertices
vec2 positions[3] = vec2[](
	vec2(0.0f, -0.5f),
	vec2(0.5f, 0.5f),
	vec2(-0.5f, 0.5f)
);

//Set the colors of the vertices
vec3 colors[3] = vec3[](
	vec3(1.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 0.0f, 1.0f)
);

layout(location = 0) out vec3 vertColor;

void main(){
	gl_Position = vec4(positions[gl_VertexIndex], 0.0f, 1.0f);
	vertColor = vec3(colors[gl_VertexIndex]);
}