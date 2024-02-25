#pragma once

#include "../Utils/logger.h"

typedef uint32_t RID;

class Shader
{
public:
	Shader();
	~Shader();

	inline bool IsOk() const { return m_id != 0; }

	void Init(const std::string& fragFile);
	void Init(const std::string& fragFile, const std::string& vertFile);

	void Bind() const;
	void Unbind() const;

	// Setting uniforms
	void SetUniform1i(const std::string& name, int32_t v);
	void SetUniform1f(const std::string& name, float v);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const float* mat);

private:
	RID m_id;
	std::unordered_map<std::string, int32_t> m_UniformLocationCache;
private:
	bool LoadFileToString(std::string& outBuf, const std::string& fName);
	RID CreateShader(const std::string& fragSrc, const std::string& vertSrc);
	RID CompileShader(uint32_t type, const std::string& src);

	uint32_t GetUniformLocation(const std::string& name);
};

enum BaseShader
{
	Sh_WhiteFill = 0,
	Sh_ColorFill,
	Sh_Background,
	BaseShader_COUNT
};

class ShaderStorage
{
public:
	ShaderStorage();
	~ShaderStorage() = default;

	void Init();

	void Bind(BaseShader type) const;
	void Unbind() const;

	inline Shader& GetShader(BaseShader type) { RENDERER_ASSERT(type < BaseShader_COUNT && type >= 0, "Invalid shader supplied"); return m_shaders[type]; }

private:
	std::vector<Shader> m_shaders;

	static constexpr std::array<std::pair<const char*, const char*>, BaseShader_COUNT> s_baseShaderPaths = {
		std::make_pair("C:/Omat/Koodailu/Pong/src/res/shaders/whiteFill.frag", ""),
		{"C:/Omat/Koodailu/Pong/src/res/shaders/colorFill.frag", ""},
		{"C:/Omat/Koodailu/Pong/src/res/shaders/background.frag", ""}
	};
};