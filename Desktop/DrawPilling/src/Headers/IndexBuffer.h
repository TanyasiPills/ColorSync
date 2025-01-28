#pragma once

class IndexBuffer
{
private:
	unsigned int IBO;
	unsigned int count;
public:
	IndexBuffer();
	~IndexBuffer();

	void Init(const unsigned int* data, unsigned int count);

	void Bind() const;
	void UnBind() const;

	inline unsigned int GetCount() const { return count; }

	//void UpdateData(const void* data, unsigned int size);
};
