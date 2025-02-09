#pragma once


class VertexBuffer
{
private:
	unsigned int VBO;
public:
	VertexBuffer(const void* data, unsigned int size, int debug = 0);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;
};

