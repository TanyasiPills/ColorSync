#pragma once

#include <string>
#include <vector>

class MyTexture 
{
private:
	unsigned int TO;
	std::string filePath;
	unsigned char* localBuffer;
	int width, height, bpp;
public:
	MyTexture();
	~MyTexture();

	void Init(const std::string& path);
	void Init(unsigned int widthIn, unsigned int heightIn, int transparent = 0);
	void Init(std::vector<unsigned char> data, unsigned int canvasWidth, unsigned int canvasHeight);

	void Bind(unsigned int slot = 0) const;
	void UnBind() const;

	void TransparencyCorrection();
	void BlendCorrection();

	unsigned int GetId() const;

	inline int GetWidht() const { return width; }
	inline int GetHeight() const { return height; }
};