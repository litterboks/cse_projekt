#pragma once
#include <GL/glew.h>

class Texture
{
public:
	explicit Texture(char* path);
	Texture();
	~Texture();
	bool isSet() const;
	GLuint m_texture;
private:
	int m_height;
	int m_width;
	int m_channels;
};
