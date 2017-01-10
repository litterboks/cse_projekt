#include "Mesh.h"
#include <glm/gtc/matrix_transform.inl>
#include <glm/gtc/quaternion.hpp>

Mesh::Mesh(): m_alpha(1.f), m_lod(LOW)
{
}

Mesh::Mesh(MyVAO& low, MyVAO& med, MyVAO& high): m_alpha(1.0f), m_lod(LOW)
{
	vaos.push_back(low);
	vaos.push_back(med);
	vaos.push_back(high);
}

Mesh& Mesh::operator=(Mesh& other)
{
	if (this != &other)
	{
		this->m_transformation = other.m_transformation;
		this->m_alpha = other.m_alpha;
		this->m_lod = other.m_lod;
		this->vaos = other.vaos;
		this->textures = other.textures;
	}
	return *this;
}

void Mesh::Rotate(float degrees, AXIS axis)
{
	glm::vec3 vecAxis;
	switch (axis)
	{
	case x:
		vecAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		break;
	case y:
		vecAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		break;
	case z:
		vecAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		break;
	default:
		vecAxis = glm::vec3(0.0f, 0.0f, 0.0f);
		break;
	}
	m_transformation = glm::rotate(m_transformation, glm::radians(degrees), vecAxis);
}

void Mesh::Translate(float x, float y, float z)
{
	m_transformation = glm::translate(m_transformation, glm::vec3(x, y, z));
}

void Mesh::Scale(float x, float y, float z)
{
	m_transformation = glm::scale(m_transformation, glm::vec3(x, y, z));
}

void Mesh::innerDraw(MyVAO myVAO) const
{
	myVAO.setTransformation(m_transformation);
	if (!textures.empty())
	{
		myVAO.setTexture(textures[0].get().m_texture);
	}
	else
	{
		myVAO.setTexture(-1);
	}
	myVAO.setAlpha(m_alpha);
	myVAO.draw();
}

void Mesh::calculateLOD(glm::vec3 position)
{
	float fdistance = glm::distance(position, glm::vec3(m_transformation[3]));
	if (fdistance <= 2.f)
	{
		m_lod = HIGH;
	}
	else if (fdistance <= 3.f)
	{
		m_lod = MED;
	}
	else
	{
		m_lod = LOW;
	}
}

void Mesh::setAlpha(float alpha)
{
	m_alpha = alpha;
}

void Mesh::setTexture(Texture& texture)
{
	textures.push_back(texture);
}

glm::vec3 Mesh::getPosition()
{
	return glm::vec3(m_transformation[3]);
}

void Mesh::draw() const
{
	switch (m_lod)
	{
	case LOW:
		innerDraw(vaos[0]);
		break;
	case MED:
		innerDraw(vaos[1]);
		break;
	case HIGH:
		innerDraw(vaos[2]);
		break;
	default:
		innerDraw(vaos[0]);
		break;
	}
}
