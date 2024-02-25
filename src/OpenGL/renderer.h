#pragma once

#include "../Utils/matrix.h"
#include "shader.h"

struct GLFWwindow;
typedef uint32_t RID;

class Renderer
{
public:
	Renderer(int w, int h, const char* title);
	~Renderer();

	inline bool IsInitSuccess() const { return m_initSuccess; }

	// *** Rendering related *** //

	// * Queries *

	bool WindowShouldClose();
	inline float GetFrameTime() const { return m_prevFrameTime; }
	float GetElapsedSecs() const;
	std::pair<int, int> GetWindowSize() const;
	inline int GetWindowWidth() const { auto [w, h] = GetWindowSize(); return w; }
	inline int GetWindowHeight() const { auto [w, h] = GetWindowSize(); return h; }

	// * Setters *

	void ClearBG(float r, float g, float b, float a = 1.0f);
	void BackGroundShader(BaseShader type, Vec2 highlightPos);

	void SwapAndPoll();

	void DrawRect(Vec2 ul, Vec2 lr, Vec4 c = Vec4({1.f, 1.f, 1.f, 1.f}));

private:
	bool m_initSuccess;
	GLFWwindow* m_window;
	std::chrono::steady_clock::time_point m_firstTimePoint, m_lastTimePoint;
	float m_prevFrameTime;
	RID m_vb, m_ib, m_va;
	ShaderStorage m_shaderStorage;
private:
	bool Init(int w, int h, const char* title);
};