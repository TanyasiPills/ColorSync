#include "VertexArray.h"
#include "NewRenderer.h"
#include <iostream>

VertexArray::VertexArray() 
{
	GLCall(glGenVertexArrays(1, &VAO));
}
VertexArray::~VertexArray() 
{
	GLCall(glDeleteVertexArrays(1, &VAO));
}

void VertexArray::SetLayout(const VertexBufferLayout& layoutIn)
{
	layout = layoutIn;
}

void VertexArray::SetBuffer(const VertexBuffer& vertexBuffer) 
{
	Bind();
	vertexBuffer.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++) 
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{

	GLCall(glBindVertexArray(VAO));
}
void VertexArray::UnBind() const
{
	GLCall(glBindVertexArray(0));
}