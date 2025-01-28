#include "old/GLManager.h"
#include "GLEW/glew.h"
#include "VertexBuffer.h"
#include <GLFW/glfw3.h>

unsigned int* shaderProgram;
GLint colorLoc;

void GLManager::GetShaderProgram(unsigned int& shader) {
	shaderProgram = &shader;
	colorLoc = glGetUniformLocation(*shaderProgram, "uColor");
}

void GLManager::initBuffers(unsigned int& VBO, unsigned int& VAO) {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
}

void GLManager::updateVBO(unsigned int& VBO, const std::vector<float>& vertices) {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
}

void GLManager::drawStuff(unsigned int& VAO,GLenum mode, const std::vector<float>& vertices, float color[3]) {
	glUniform3f(colorLoc, color[0], color[1], color[2]);
	glBindVertexArray(VAO);
	glDrawArrays(mode, 0, static_cast<GLsizei>(vertices.size() / 2));
	glBindVertexArray(0);
}
