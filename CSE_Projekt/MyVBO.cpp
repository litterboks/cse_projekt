#include "MyVBO.h"

MyVBO::MyVBO(GLfloat* vertices, int drawType, GLuint shaderProgram): shaderProgram(shaderProgram)
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, drawType);
}

void MyVBO::draw() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr));
	glEnableVertexAttribArray(0);
	glUseProgram(shaderProgram);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, nullptr);
	glUseProgram(0);
}
