#pragma once
#include <GL/glew.h>

class MyVBO
{
public:
	MyVBO();
	MyVBO(GLfloat* vertices, int drawType, GLuint shaderProgram);
private:
	GLuint m_id;
};
