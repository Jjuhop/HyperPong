#include "game.h"
#include "Utils/logger.h"

#define WIDTH 1200
#define HEIGHT 800

Game::Game()
    : m_renderer(WIDTH, HEIGHT, "Jaa")
{
}

int Game::Run()
{
    if (!m_renderer.IsInitSuccess())
        return -1;

    GAME_INFO("Game started");

    int wBase = WIDTH, hBase = HEIGHT;
    int counter = 0;

    constexpr float dotRad = 10.f;
    constexpr float dotSpeed = 0.5f;

    std::mt19937_64 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    const float dir = static_cast<float>(dis(rng));
    Vec2 dotVel({ dotSpeed*std::cos(dir), dotSpeed*std::sin(dir) });
    Vec2 dotPos({ dotRad, static_cast<float>(hBase) * 0.5f });

    Vec2 gravity({ 0.0f, -9.5f });

    // Spring to cursor
    float strength = 0.2f;
    float springLength = 250.f;

    while (!m_renderer.WindowShouldClose()) {
        m_renderer.ClearBG(0.0f, 0.0f, 0.0f);
        m_renderer.BackGroundShader(Sh_Background, dotPos);

        float dt = m_renderer.GetFrameTime();
        Vec2 curs = m_renderer.GetMousePos();
        counter++;
        if (counter >= 700 || dt > 1.f/60.f) {
            counter = 0;
            GAME_INFO(std::format("Frame time {:.5f}s, ({:.0f} FPS), elapsed {:.5f}s", dt, 1.f / dt, m_renderer.GetElapsedSecs()));
            GAME_INFO(std::format("Mouse at ({}, {}), dot at ({}, {})", curs[0], curs[1], dotPos[0], dotPos[1]));
        }

        auto [w, h] = m_renderer.GetWindowSize();
        if (w != wBase || h != hBase) {
            GAME_INFO(std::format("Size changed, new size {}x{}", w, h));
            wBase = w;
            hBase = h;
        }

        dotPos = dotPos + (dotVel * dt * 1000);
        //dotVel = dotVel + (gravity * dt);

        // Attraction to cursor
        //Vec2 diff = curs - dotPos;
        ////float activeDist = std::max(diff.length() - springLength, 0.0f) * 0.001f;
        //float activeDist = (diff.length() - springLength) * 0.001f;
        //Vec2 acc = diff * activeDist * strength;
        //dotVel = (dotVel + (acc * std::min(dt, 1 / 60.f)));
        
        // Air res
        constexpr float airRes = 1.f;
        constexpr float mass = 5.f;
        constexpr float eps = 1e-9;
        float dotSp = dotVel.length();
        float newSp = dotSp < eps ? 0.f : mass / ((mass / dotSp + dt) * airRes);
        //dotVel = dotVel * (newSp / dotSp);

        // Collisions
        if (dotPos[0] < 0 + dotRad)
            dotVel[0] = std::abs(dotVel[0]);
        if (dotPos[0] > static_cast<float>(w) - dotRad)
            dotVel[0] = -std::abs(dotVel[0]);
        if (dotPos[1] < 0 + dotRad)
            dotVel[1] = std::abs(dotVel[1]);
        if (dotPos[1] > static_cast<float>(h) - dotRad)
            dotVel[1] = -std::abs(dotVel[1]);

        Vec2 rVec{ dotRad, dotRad };
        m_renderer.DrawRect( dotPos - rVec, dotPos + rVec, Vec4({dotVel[0] / dotSpeed, dotVel[1] / dotSpeed, dotPos[1] / h, 1.0f}));

        m_renderer.SwapAndPoll();
    }

    return 0;
}

bool Game::Init()
{
    return true;
}
