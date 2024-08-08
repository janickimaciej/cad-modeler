#pragma once

class PointMesh
{
public:
	PointMesh();
	~PointMesh();
	void render() const;

private:
	unsigned int m_VAO{};
};
