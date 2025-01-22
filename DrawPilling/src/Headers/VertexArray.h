#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include <GLFW/glfw3.h> 

class VertexArray {
private:
	unsigned int VAO;
public:
	VertexBufferLayout layout;
	VertexArray();
	~VertexArray();

	void SetLayout(const VertexBufferLayout& layoutIn);
	void SetBuffer(const VertexBuffer& vertexBuffer);
	void Bind() const;
	void UnBind() const;
};