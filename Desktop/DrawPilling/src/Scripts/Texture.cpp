#include "Texture.h"
#include "NewRenderer.h"
#include "stb_image/stb_image.h"
#include "GLEW/glew.h"

MyTexture::MyTexture() : TO(0), localBuffer(nullptr), width(0), height(0), bpp(0)
{
}

void MyTexture::Init(std::vector<unsigned char> data, unsigned int canvasWidth, unsigned int canvasHeight)
{
	width = canvasWidth;
	height = canvasHeight;

	glGenTextures(1, &TO);
	glBindTexture(GL_TEXTURE_2D, TO);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, canvasWidth, canvasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyTexture::Init(const std::string& path)
{
	//stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);

	if (!localBuffer) {
		std::cerr << "Failed to load texture: " << path << std::endl;
		return;
	}

	glGenTextures(1, &TO);
	glBindTexture(GL_TEXTURE_2D, TO);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (localBuffer) {
		stbi_image_free(localBuffer);
	}
}

void MyTexture::TransparencyCorrection() {
	glBindTexture(GL_TEXTURE_2D, TO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyTexture::Init(unsigned int widthIn, unsigned int heightIn, int transparent)
{
	width = widthIn;
	height = heightIn;
	glGenTextures(1, &TO);
	glBindTexture(GL_TEXTURE_2D, TO);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	unsigned char* data = new unsigned char[width * height * 4];
	for (int i = 0; i < width * height * 4; ++i) {
		if ((i+1) % 4 == 0) {
			if (transparent) {
				data[i] = 0;
			}
			else {
				data[i] = 255;
			}
		}
		else {
			data[i] = 255;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete[] data;

	glBindTexture(GL_TEXTURE_2D, 0);
}

MyTexture::~MyTexture()
{
	glDeleteTextures(1, &TO);
}

void MyTexture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, TO));
}

void MyTexture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int MyTexture::GetId() const
{
	return TO;
}

void MyTexture::BlendCorrection()
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	UnBind();
}