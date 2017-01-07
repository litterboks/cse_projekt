#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texCoord;

out vec3 ourColor;
out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transform * vec4(position, 1.0f);
	FragPos = vec3(transform * vec4(position, 1.0f));
	mat3 normalTrans;
	normalTrans = mat3(transform);
	Normal = normalTrans * normals;
    ourColor = color;
    TexCoord = texCoord;
}