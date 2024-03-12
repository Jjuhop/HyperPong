#include "projectile.h"

Projectile::Projectile(Renderer& rend, float wndH)
	: m_renderer(rend),
    m_pos(), m_vel()
{
    m_pos = Vec2({ s_rad, wndH * 0.5f });
    
    std::mt19937_64 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    const float dir = static_cast<float>(dis(rng));
    m_vel = Vec2({ s_speed * std::cos(dir), s_speed * std::sin(dir) });
}

void Projectile::GravityToPoint(const Vec2& point, float mass, float dt)
{
    Vec2 diff = (point - m_pos) * 0.001f;                // Normalized to meters
    float distSq = std::max(diff.lengthSqr(), 1e-4f);   // Add small min value to prevent huge accelerations
    Vec2 acc = diff.normalized() * (mass / distSq);
    m_vel = m_vel + acc * dt;
}

void Projectile::ApplyAirRes(float airResCoef, float dt)
{
    float speed = m_vel.length();
    if (speed > 3.f) {
        float newSp = s_mass / ((s_mass / (speed * airResCoef) + dt) * airResCoef);
        m_vel = m_vel * (newSp / speed);
    }
}

void Projectile::CheckYCollision(float wndH)
{
    if (m_pos[1] < 0 + s_rad)
        m_vel[1] = std::abs(m_vel[1]);
    if (m_pos[1] > wndH - s_rad)
        m_vel[1] = -std::abs(m_vel[1]);
}

void Projectile::CheckLeftCollision(float barY, float barH, float barRight, float barYSpeed, float ySpeedTransferCoef)
{
    if (m_pos[0] < barRight + s_rad) {
        // Check the corners
        if (m_pos[1] > barY + barH) {
            // Upper right corner of bar should be checked more carefully
            Vec2 barCorn({ barRight, barY + barH });
            Vec2 dotCorn = m_pos + (-s_rad);
            Vec2 diffNorm = (dotCorn - barCorn).normalized();
            Vec2 velNorm = m_vel.normalized();
            if (velNorm[1] >= diffNorm[1]) {
                m_vel[0] = std::abs(m_vel[0]);
                m_vel[1] += barYSpeed * ySpeedTransferCoef;
            } else if (m_pos[1] - s_rad <= barY + barH) {
                m_vel[1] = std::abs(m_vel[1]);    // Top hit
            }
        } else if (m_pos[1] < barY - barH) {
            // Lower right corner of bar should be checked more carefully
            Vec2 barCorn({ barRight, barY - barH });
            Vec2 dotCorn = m_pos + Vec2({ -s_rad, s_rad });
            Vec2 diffNorm = (dotCorn - barCorn).normalized();
            Vec2 velNorm = m_vel.normalized();
            if (velNorm[1] <= diffNorm[1]) {
                m_vel[0] = std::abs(m_vel[0]);
                m_vel[1] += barYSpeed * ySpeedTransferCoef;
            } else if (m_pos[1] + s_rad >= barY - barH) {
                m_vel[1] = -std::abs(m_vel[1]);    // Bottom hit
            }
        } else {
            // Clear hit
            m_vel[0] = std::abs(m_vel[0]);
            m_vel[1] += barYSpeed * ySpeedTransferCoef;
        }
    }
}

void Projectile::CheckRightCollision(float barY, float barH, float barLeft, float barYSpeed, float ySpeedTransferCoef, bool is2Player, float wndW)
{
    if (is2Player && m_pos[0] > barLeft - s_rad) {
        // Collisions on the right
        // Check the corners
        if (m_pos[1] > barY + barH) {
            // Upper left corner of bar should be checked more carefully
            Vec2 barCorn({ barLeft, barY + barH });
            Vec2 dotCorn = m_pos + Vec2({ s_rad, -s_rad });
            Vec2 diffNorm = (dotCorn - barCorn).normalized();
            Vec2 velNorm = m_vel.normalized();
            if (velNorm[1] >= diffNorm[1]) {
                m_vel[0] = -std::abs(m_vel[0]);
                m_vel[1] += barYSpeed * ySpeedTransferCoef;
            } else if (m_pos[1] - s_rad <= barY + barH) {
                m_vel[1] = std::abs(m_vel[1]);    // Top hit
            }
        } else if (m_pos[1] < barY - barH) {
            // Lower left corner of bar should be checked more carefully
            Vec2 barCorn({ barLeft, barY - barH });
            Vec2 dotCorn = m_pos + s_rad;
            Vec2 diffNorm = (dotCorn - barCorn).normalized();
            Vec2 velNorm = m_vel.normalized();
            if (velNorm[1] <= diffNorm[1]) {
                m_vel[0] = -std::abs(m_vel[0]);
                m_vel[1] += barYSpeed * ySpeedTransferCoef;
            } else if (m_pos[1] + s_rad >= barY - barH) {
                m_vel[1] = -std::abs(m_vel[1]);    // Bottom hit
            }
        } else {
            // Clear hit
            m_vel[0] = -std::abs(m_vel[0]);
            m_vel[1] += barYSpeed * ySpeedTransferCoef;
        }
    } else if (m_pos[0] >= wndW - s_rad) {
        // Single player game
        m_vel[0] = -std::abs(m_vel[0]);
    }
}

void Projectile::Advance(float dt)
{
    // Adjust dot position according to velocity (turn the unit from meters to pix)
    m_pos = m_pos + (m_vel * dt * 1000);
}

void Projectile::Draw() const
{
    m_renderer.DrawRect(m_pos + (-s_rad), m_pos + s_rad, Vec4({m_vel[0] / s_speed, m_vel[1] / s_speed, m_pos[1] / 1000.f, 1.0f}));
}
