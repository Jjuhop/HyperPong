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
    constexpr float dotSpeed = 400.f;

    std::mt19937_64 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<> dis(-1.4, 1.4);

    const float dir = static_cast<float>(dis(rng));
    Vec2 dotVel({ dotSpeed*std::cos(dir), dotSpeed*std::sin(dir) });
    Vec2 dotPos({ dotRad, static_cast<float>(hBase) * 0.5f });

    while (!m_renderer.WindowShouldClose()) {
        m_renderer.ClearBG(0.8f, 0.2f, 0.7f);
        m_renderer.BackGroundShader(Sh_Background, dotPos);

        float dt = m_renderer.GetFrameTime();
        counter++;
        if (counter >= 1000) {
            counter = 0;
            GAME_INFO(std::format("Frame time {:.5f}s, ({:.0f} FPS), elapsed {:.5f}s", dt, 1.f / dt, m_renderer.GetElapsedSecs()));
        }

        auto [w, h] = m_renderer.GetWindowSize();
        if (w != wBase || h != hBase) {
            GAME_INFO(std::format("Size changed, new size {}x{}", w, h));
            wBase = w;
            hBase = h;
        }

        
        dotPos = dotPos + (dotVel * dt);

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
