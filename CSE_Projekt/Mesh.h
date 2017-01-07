#pragma once
#include "MyVAO.h"
#include "Texture.h"
#include "vector"
#include <functional>

enum LOD
{
	LOW,
	MED,
	HIGH
};

class Mesh
{
public:
	Mesh();
	Mesh(MyVAO& low, MyVAO& med, MyVAO& high);
	Mesh& operator=(Mesh& other);
	void Rotate(float degrees, AXIS axis);
	void Translate(float x, float y, float z);
	void Scale(float x, float y, float z);

	void draw() const;
	void innerDraw(MyVAO myVAO) const;

	void calculateLOD(glm::vec3 position);
	void setAlpha(float alpha);
	void setTexture(Texture& texture);
	glm::vec3 getPosition();
private:
	float m_alpha;
	LOD m_lod;
	glm::mat4 m_transformation;
	std::vector<std::reference_wrapper<MyVAO>> vaos;
	std::vector<std::reference_wrapper<Texture>> textures;
};
