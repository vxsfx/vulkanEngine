#version 450

layout (binding = 0) uniform Matrices {
	mat4 model;
	mat4 view;
	mat4 projection;
} mats;


layout (location = 0) in vec3 inPosition; //devc3 takes up double slot (+2 location)
//layout (location = 1) in vec3 inColor;
layout (location = 1) in vec2 inTexCoord;

//layout(location = 0) out vec3 fragColor;
layout(location = 0) out vec2 fragTexCoord;


void main(){
gl_Position = mats.projection * mats.view * mats.model * vec4(inPosition, 1.0);

//fragColor = inColor;
fragTexCoord = inTexCoord;
}