#include "playerBar.h"

Bar::Bar(Renderer& rend, float wndH, bool isLeft)
	: m_renderer(rend),
	m_isLeft(isLeft),
	m_yPos(wndH * 0.5f),
	m_yVel(0.0f)
{

}

void Bar::Up(float dt)
{
	m_yVel = std::min(m_yVel + s_acc * dt, s_maxSpeed);
	m_yPos += m_yVel * dt;
}

void Bar::Down(float dt)
{
	m_yVel = std::max(m_yVel - s_acc * dt, -s_maxSpeed);
	m_yPos += m_yVel * dt;
}

void Bar::Brake(float dt)
{
	m_yVel *= std::pow(0.93f, dt * 144.f);
	m_yPos += m_yVel * dt;
}

float Bar::GetCollisionX() const
{
	if (m_isLeft)
		return s_indent;
	else
		return static_cast<float>(m_renderer.GetWindowWidth()) - s_indent;
}

void Bar::Draw() const
{
	float x = m_isLeft ? 2 * s_size : static_cast<float>(m_renderer.GetWindowWidth()) - 2 * s_size;
	const Vec2 bar1mid({x, m_yPos });
	const Vec2 barCornOffset({ s_size, s_height });
	m_renderer.DrawRect(bar1mid - barCornOffset, bar1mid + barCornOffset);
}
