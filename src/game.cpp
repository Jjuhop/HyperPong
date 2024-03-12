#include "game.h"
#include "projectile.h"
#include "playerBar.h"
#include "Utils/logger.h"

#define BASEWIDTH 1600
#define BASEHEIGHT 900

Game::Game()
    : m_renderer(BASEWIDTH, BASEHEIGHT, "Hyper Pong")
{
}

int Game::Run()
{
    if (!m_renderer.IsInitSuccess())
        return -1;

    GAME_INFO("Game started");

    // Game projectile (dot)
    Projectile dot(m_renderer, static_cast<float>(BASEHEIGHT));

    //Vec2 gravity({ 0.0f, -9.5f });

    // Spring to cursor
    //float strength = 0.2f;
    //float springLength = 250.f;

    // Bars
    Bar leftBar(m_renderer, static_cast<float>(BASEHEIGHT), true);
    Bar rightBar(m_renderer, static_cast<float>(BASEHEIGHT), false);

    // Black hole in the middle
    Vec2 bhPos({0.5f * BASEWIDTH, 0.5f * BASEHEIGHT});
    constexpr float bhMass = 0.03f;
    constexpr float bhSize = 88.f;

    // A bit hacky way to get key presses
    bool r_down = false, i_down = false;

    bool isTwoPlayer = true;

    while (!m_renderer.WindowShouldClose()) {
        m_renderer.ClearBG(0.0f, 0.0f, 0.0f);
        m_renderer.BackGroundShader(Sh_Background, dot.GetPos());

        float dt = m_renderer.GetFrameTime();
        Vec2 curs = m_renderer.GetMousePos();

        // Info stuff in I key press or laggy frames
        if (m_renderer.IsKeyDown(KEY_I) && !i_down || dt > 1.f / 20.f) {
            GAME_INFO(std::format("Frame time {:.5f}s, ({:.0f} FPS), elapsed {:.5f}s", dt, 1.f / dt, m_renderer.GetElapsedSecs()));
            Vec2 dotPos = dot.GetPos();
            GAME_INFO(std::format("Mouse at ({}, {}), dot at ({}, {}), dot speed {}", curs[0], curs[1], dotPos[0], dotPos[1], dot.GetVel().length()));
        }
        i_down = m_renderer.IsKeyDown(KEY_I);

        auto [w, h] = m_renderer.GetWindowSize();

        // Shader reset in R press
        if (m_renderer.IsKeyDown(KEY_R) && !r_down) {
            m_renderer.ResetShaders();
            GAME_INFO("Resetting shaders");
        }
        r_down = m_renderer.IsKeyDown(KEY_R);

        // Select 1/2 player mode
        if (m_renderer.IsKeyDown(KEY_1))
            isTwoPlayer = false;
        else if (m_renderer.IsKeyDown(KEY_2))
            isTwoPlayer = true;

        // Player 1 (left) controls
        if (m_renderer.IsKeyDown(KEY_W) || (!isTwoPlayer && m_renderer.IsKeyDown(KEY_UP))) {
            leftBar.Up(dt);
        } else if (m_renderer.IsKeyDown(KEY_S) || (!isTwoPlayer && m_renderer.IsKeyDown(KEY_DOWN))) {
            leftBar.Down(dt);
        } else {
            leftBar.Brake(dt);
        }

        // Player 2 (right) controls
        if (isTwoPlayer && m_renderer.IsKeyDown(KEY_UP)) {
            rightBar.Up(dt);
        } else if (isTwoPlayer && m_renderer.IsKeyDown(KEY_DOWN)) {
            rightBar.Down(dt);
        } else {
            rightBar.Brake(dt);
        }

        // Gravity (Downwards)
        //dotVel = dotVel + (gravity * dt);

        // Gravity towards black hole
        dot.GravityToPoint(bhPos, bhMass, dt);

        // Attraction to cursor
        //Vec2 diff = curs - dotPos;
        ////float activeDist = std::max(diff.length() - springLength, 0.0f) * 0.001f;
        //float activeDist = (diff.length() - springLength) * 0.001f;
        //Vec2 acc = diff * activeDist * strength;
        //dotVel = (dotVel + (acc * std::min(dt, 1 / 60.f)));
        
        // Air res at larger speeds to slow the dot down
        dot.ApplyAirRes(1.f, dt);

        dot.CheckYCollision(static_cast<float>(h));

        dot.CheckLeftCollision(leftBar.GetY(), Bar::s_height, leftBar.GetCollisionX(), leftBar.GetYVel(), Bar::s_velTransferCoef);
        dot.CheckRightCollision(rightBar.GetY(), Bar::s_height, rightBar.GetCollisionX(), rightBar.GetYVel(), Bar::s_velTransferCoef, isTwoPlayer, static_cast<float>(w));

        dot.Advance(dt);
        
        // Draw the dot and bars
        dot.Draw();
        leftBar.Draw();
        if (isTwoPlayer)
            rightBar.Draw();

        m_renderer.DrawRectSh(bhPos - bhSize, bhPos + bhSize, Sh_BlackHole);

        m_renderer.SwapAndPoll();
    }

    return 0;
}

bool Game::Init()
{
    return true;
}
