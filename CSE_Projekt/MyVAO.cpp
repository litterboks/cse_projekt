#include "MyVAO.h"
#include <glm/gtc/type_ptr.hpp>


MyVAO::MyVAO(Shader& shader, GLfloat* vertices, GLsizei size) : m_size(size), m_shader(shader), m_texture(-1), m_alpha(1)
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(9 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
}

MyVAO& MyVAO::operator=(MyVAO& other)
{
	if (this != &other)
	{
		this->m_transformation = other.m_transformation;
		this->m_alpha = other.m_alpha;
		this->m_shader = other.m_shader;
		this->m_size = other.m_size;
		this->m_texture = other.m_texture;
		this->m_vao = other.m_vao;
		this->m_vbo = other.m_vbo;
	}
	return *this;
}

void MyVAO::setShaderProgram(Shader& shaderProgram) const
{
	this->m_shader = shaderProgram;
}

void MyVAO::setTexture(GLuint texture)
{
	this->m_texture = texture;
}

void MyVAO::setTransformation(glm::mat4 transformation)
{
	m_transformation = transformation;
}

void MyVAO::draw() const
{
	GLint textureIsSet = glGetUniformLocation(m_shader.m_program, "textureIsSet");
	glActiveTexture(GL_TEXTURE0);
	if (m_texture != -1)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glUniform1i(textureIsSet, 1);
	}
	else
	{
		glUniform1i(textureIsSet, 0);
	}

	GLuint transformLoc = glGetUniformLocation(m_shader.m_program, "transform");
	GLuint alphaLoc = glGetUniformLocation(m_shader.m_program, "alpha");
	glUniform1f(alphaLoc, m_alpha);
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(m_transformation));
	glUniform1i(glGetUniformLocation(m_shader.m_program, "ourTexture"), 0);
	m_shader.Use();
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_size/11);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyVAO::setAlpha(const float alpha)
{
	m_alpha = alpha;
}
