#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include <glm/detail/type_mat4x4.hpp>

enum AXIS
{
	x,
	y,
	z
};

class MyVAO
{
public:
	MyVAO(Shader& shader, GLfloat* vertices, GLsizei size);
	MyVAO& operator=(MyVAO& other);
	void setTransformation(glm::mat4 transformation);
	void setShaderProgram(Shader& shaderProgram) const;
	void setTexture(GLuint texture);
	void draw() const;
	void setAlpha(const float alpha);
private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_texture;
	GLfloat m_alpha;
	GLsizei m_size;
	Shader& m_shader;

	glm::mat4 m_transformation;
};
