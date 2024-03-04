#include "shader.h"

#include "../Utils/logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Shader::Shader()
	: m_id(0)
{ }

Shader::~Shader()
{
	glDeleteProgram(m_id);
}

void Shader::Init(const std::string& fragFile)
{
	std::string fragSrc;
	if (!this->LoadFileToString(fragSrc, fragFile))
		return;
	std::string vertSrc = R"(
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec3 v_Position;
out vec2 v_TexCoord;

void main() {
	v_Position = a_Position;
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 1.0);
}
)";

	m_id = this->CreateShader(fragSrc, vertSrc);
}

void Shader::Init(const std::string& fragFile, const std::string& vertFile)
{
	std::string fragSrc;
	bool fRead = this->LoadFileToString(fragSrc, fragFile);
	std::string vertSrc;
	bool vRead = this->LoadFileToString(vertSrc, vertFile);
	if (fRead && vRead)
		m_id = this->CreateShader(fragSrc, vertSrc);
}


void Shader::Bind() const
{
	glUseProgram(m_id);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int32_t v)
{
	glUniform1i(GetUniformLocation(name), v);
}

void Shader::SetUniform1f(const std::string& name, float v)
{
	glUniform1f(GetUniformLocation(name), v);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const float* mat)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, mat);
}

bool Shader::LoadFileToString(std::string& outBuf, const std::string& fName)
{
	outBuf.clear();
	std::ifstream file(fName);
	if (!file) {
		RENDERER_ERROR(std::format("Could not open file {}", fName));
		return false;
	}
	std::string line;
	while (file.good()) {
		std::getline(file, line);
		outBuf += line += "\n";
	}
	if (file.fail())
		RENDERER_ERROR(std::format("File {} was left in bad state after reading", fName));
	
	return !file.fail();
}

RID Shader::CreateShader(const std::string& fragSrc, const std::string& vertSrc)
{
	RID vs = CompileShader(GL_VERTEX_SHADER, vertSrc);
	RID fs = CompileShader(GL_FRAGMENT_SHADER, fragSrc);
	if (vs == 0 || fs == 0)
		return 0;

	RID program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	int isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		glDeleteShader(vs);
		glDeleteShader(fs);

		RENDERER_ERROR(infoLog.data());

		return 0;
	}

	glValidateProgram(program);

	glDetachShader(program, vs);
	glDetachShader(program, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

RID Shader::CompileShader(uint32_t type, const std::string& src)
{
	RID id = glCreateShader(type);
	const char* str = src.data();
	glShaderSource(id, 1, &str, nullptr);
	glCompileShader(id);

	//Error handling
	int32_t result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int32_t length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> infoLog(length);
		glGetShaderInfoLog(id, length, &length, &infoLog[0]);
		glDeleteShader(id);

		const char* typeStr = (type == GL_VERTEX_SHADER ? "vertex" : "fragment");
		RENDERER_ERROR(std::format("Failed to compile {} shader!", typeStr));
		RENDERER_ERROR(infoLog.data());

		return 0;
	}

	return id;
}

uint32_t Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.contains(name)) {
		return m_UniformLocationCache[name];
	}
	int32_t location = glGetUniformLocation(m_id, name.c_str());
	if (location == -1) {
		RENDERER_ERROR(std::format("Uniform {} doesn't exist!", name));
	}
	m_UniformLocationCache[name] = location;
	return location;
}

// *** ShaderStorage *** //

ShaderStorage::ShaderStorage()
	: m_shaders(BaseShader_COUNT)
{
}

void ShaderStorage::Init()
{
	for (size_t i = 0; i < BaseShader_COUNT; i++) {
		auto [fragFile, vertFile] = s_baseShaderPaths[i];
		// Check if there even is a vertex file defined
		if (*vertFile != 0) {
			m_shaders[i].Init(fragFile, vertFile);
		} else {
			m_shaders[i].Init(fragFile);
		}
	}
}

void ShaderStorage::Bind(BaseShader type) const
{
	RENDERER_ASSERT(type < BaseShader_COUNT && type >= 0, "Invalid shader supplied");
	RENDERER_ASSERT(m_shaders[type].IsOk(), "This shader wasn't created successfully!");
	m_shaders[type].Bind();
}

void ShaderStorage::Unbind() const
{
	glUseProgram(0);
}
