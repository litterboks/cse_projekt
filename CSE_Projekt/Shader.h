#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>

class Shader
{
public:
	Shader();
	// Constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Shader();
	// Use the program
	void Use() const;
	GLuint m_program;
};

#endif
