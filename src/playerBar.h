#pragma once

#include "OpenGL/renderer.h"

class Bar
{
public:
    Bar(Renderer& rend, float wndH, bool isLeft);

    void Up(float dt);
    void Down(float dt);
    void Brake(float dt);

    inline float GetY() const { return m_yPos; }
    inline float GetYVel() const { return m_yVel; }
    float GetCollisionX() const;

    void Draw() const;

private:
    Renderer& m_renderer;
	bool m_isLeft;
	float m_yPos;
	float m_yVel;

    static constexpr float s_size = 10.f;
    static constexpr float s_indent = 3 * s_size; // This is where the collisions happens
    static constexpr float s_acc = 6000.f;
    static constexpr float s_maxSpeed = 1200.f;
public:
    static constexpr float s_height = 7 * s_size;
    static constexpr float s_velTransferCoef = 0.04 * 0.001f;
};