#pragma once

#include "OpenGL/renderer.h"

class Game
{
public:
	Game();
	~Game() = default;

	int Run();

private:
	Renderer m_renderer;
private:
	bool Init();
};