#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  

out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform float alpha;
uniform bool textureIsSet;

void main()
{
vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);  

float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * lightColor;

if(textureIsSet){
    color = texture(ourTexture, TexCoord) * vec4(ourColor *diffuse, alpha);
	}
	else{
	color = vec4(ourColor* lightColor, alpha);
}
}