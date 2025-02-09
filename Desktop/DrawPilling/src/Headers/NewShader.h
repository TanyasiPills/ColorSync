#pragma once

#include <string>
#include <unordered_map>

struct ShaderSource2 {
	std::string Vertex;
	std::string Fragment;
};

class NewShader 
{
private:

	std::unordered_map<std::string, int> UniformLocs;
public:
	unsigned int shaderId;
	NewShader();
	~NewShader();

	void BindShaderFile(const std::string& filepath);

	void Bind() const;
	void UnBind() const;

	void SetUniform1i(const std::string& name, int i1);
	void SetUniform1f(const std::string& name, float f1);
	void SetUniform3f(const std::string& name, float f1, float f2, float f3);
	void SetUniform4f(const std::string& name, float f1, float f2, float f3, float f4);
private:
	ShaderSource2 ParseShader(const std::string& filepath);
	unsigned int CompileShader(const std::string& source, unsigned int type);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int GetUniformLocation(const std::string& name);
};
