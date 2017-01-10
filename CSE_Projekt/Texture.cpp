#include "Texture.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define ID_DXT1   0x31545844
#define ID_DXT3   0x33545844
#define ID_DXT5   0x35545844

Texture::Texture(): m_texture(-1), m_height(0), m_width(0), m_channels(0)
{
}

GLuint loadDDS(const char* filename)
{
	unsigned char header[124];

	FILE* fp;

	/* try to open the file */
	fp = fopen(filename, "rb");
	if (fp == nullptr)
	{
		return 0;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0)
	{
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	unsigned char* buffer;
	unsigned int bufsize;

	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = static_cast<unsigned char*>(malloc(bufsize * sizeof(unsigned char)));
	fread(buffer, 1, bufsize, fp);

	/* close the file pointer */
	fclose(fp);

	unsigned int components = (fourCC == ID_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case ID_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case ID_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case ID_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
		                                    0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;
	}

	free(buffer);

	//Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

Texture::Texture(char* path): m_height(0), m_width(0), m_channels(0)
{
	m_texture = loadDDS(path);
}

Texture::~Texture()
{
}

bool Texture::isSet() const
{
	if (m_texture != -1)
	{
		return true;
	}
	return false;
}
