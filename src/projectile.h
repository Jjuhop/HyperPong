#pragma once

#include "OpenGL/renderer.h"
#include "Utils/matrix.h"

class Projectile
{
public:
	Projectile(Renderer& rend, float wndH);

	inline const Vec2& GetPos() const { return m_pos; }
	inline const Vec2& GetVel() const { return m_vel; }

	void GravityToPoint(const Vec2& point, float mass, float dt);
	void ApplyAirRes(float airResCoef, float dt);

	void CheckYCollision(float wndH);
	void CheckLeftCollision(float barY, float barH, float barRight, float barYSpeed, float ySpeedTransferCoef);
	void CheckRightCollision(float barY, float barH, float barRight, float barYSpeed, float ySpeedTransferCoef, bool is2Player, float wndW);

	void Advance(float dt);
	void Draw() const;

private:
	Renderer& m_renderer;
	Vec2 m_pos;
	Vec2 m_vel;


	static constexpr float s_speed = 1.f;	// Speed in m/s (with conversion 1 pix = 1 mm)
	static constexpr float s_mass = 3.f;
public:
	static constexpr float s_rad = 10.f;	// Radius in pixels
};