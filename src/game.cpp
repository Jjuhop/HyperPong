#include "game.h"
#include "Utils/logger.h"

#define WIDTH 1920
#define HEIGHT 1080

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
    constexpr float dotSpeed = 1.f;
    const Vec2 rVec{ dotRad, dotRad };

    std::mt19937_64 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    const float dir = static_cast<float>(dis(rng));
    Vec2 dotVel({ dotSpeed*std::cos(dir), dotSpeed*std::sin(dir) });
    Vec2 dotPos({ dotRad, static_cast<float>(hBase) * 0.5f });

    Vec2 gravity({ 0.0f, -9.5f });

    // Spring to cursor
    float strength = 0.2f;
    float springLength = 250.f;

    // Bars
    constexpr float barH = 7 * dotRad;
    constexpr float barIndent = 3 * dotRad; // This is where the collisions happens
    constexpr float barAcc = 4000.f;
    constexpr float barMaxSpeed = 690.f;

    float bar1y = 0.5f * hBase, bar1yv = 0.0f;
    float bar2y = 0.5f * hBase, bar2yv = 0.0f;

    // Black hole in the middle
    Vec2 bhPos({0.5f * wBase, 0.5f * hBase});
    constexpr float bhMass = 0.03f;

    bool r_down = false;

    while (!m_renderer.WindowShouldClose()) {
        m_renderer.ClearBG(0.0f, 0.0f, 0.0f);
        m_renderer.BackGroundShader(Sh_Background, dotPos);

        float dt = m_renderer.GetFrameTime();
        Vec2 curs = m_renderer.GetMousePos();
        counter++;
        if (counter >= 5000 || dt > 1.f/60.f) {
            counter = 0;
            GAME_INFO(std::format("Frame time {:.5f}s, ({:.0f} FPS), elapsed {:.5f}s", dt, 1.f / dt, m_renderer.GetElapsedSecs()));
            GAME_INFO(std::format("Mouse at ({}, {}), dot at ({}, {}), dot speed {}", curs[0], curs[1], dotPos[0], dotPos[1], dotVel.length()));
        }

        auto [w, h] = m_renderer.GetWindowSize();
        if (w != wBase || h != hBase) {
            GAME_INFO(std::format("Size changed, new size {}x{}", w, h));
            wBase = w;
            hBase = h;
        }

        if (m_renderer.IsKeyDown(KEY_R) && !r_down) {
            m_renderer.ResetShaders();
            GAME_INFO("Resetting shaders");
        }
        r_down = m_renderer.IsKeyDown(KEY_R);

        //dotPos = dotPos + (dotVel * dt * 1000);
        
        
        if (m_renderer.IsKeyDown(KEY_UP)) {
            bar1yv = std::min(bar1yv + barAcc * dt, barMaxSpeed);
        } else if (m_renderer.IsKeyDown(KEY_DOWN)) {
            bar1yv = std::max(bar1yv - barAcc * dt, -barMaxSpeed);
        } else {
            // Braking
            bar1yv *= 0.93f;
        }
        bar1y += bar1yv * dt;

        // Gravity
        //dotVel = dotVel + (gravity * dt);

        // Attraction to black hole
        Vec2 bhDiff = (bhPos - dotPos) * 0.001f;    // Normalized to meters
        float bhDistSq = std::max(bhDiff.lengthSqr(), 1e-4f);
        Vec2 bhAcc = bhDiff.normalized() * (bhMass / bhDistSq);
        //dotVel = dotVel + bhAcc * std::min(dt, 1.f/60.f);

        // Attraction to cursor
        //Vec2 diff = curs - dotPos;
        ////float activeDist = std::max(diff.length() - springLength, 0.0f) * 0.001f;
        //float activeDist = (diff.length() - springLength) * 0.001f;
        //Vec2 acc = diff * activeDist * strength;
        //dotVel = (dotVel + (acc * std::min(dt, 1 / 60.f)));
        
        // Air res
        constexpr float airRes = 1.f;
        constexpr float mass = 5.f;
        constexpr float eps = 1e-9f;
        float dotSp = dotVel.length();
        if (dotSp > 3.f) {
            float newSp = dotSp < eps ? 0.f : mass / ((mass / dotSp + dt) * airRes);
            dotVel = dotVel * (newSp / dotSp);
        }

        // Collisions y
        if (dotPos[1] < 0 + dotRad)
            dotVel[1] = std::abs(dotVel[1]);
        if (dotPos[1] > static_cast<float>(h) - dotRad)
            dotVel[1] = -std::abs(dotVel[1]);

        // Collisions with bars
        if (dotPos[0] < barIndent + dotRad) {
            // Check the corners
            if (dotPos[1] > bar1y + barH) {
                // Upper right corner of bar should be checked more carefully
                Vec2 barCorn({ barIndent, bar1y + barH });
                Vec2 dotCorn = dotPos - rVec;
                Vec2 diffNorm = (dotCorn - barCorn).normalized();
                Vec2 velNorm = dotVel.normalized();
                if (velNorm[1] >= diffNorm[1])
                    dotVel[0] = std::abs(dotVel[0]);
                else if (dotPos[1] - dotRad <= bar1y + barH)
                    dotVel[1] = std::abs(dotVel[1]);    // Top hit
            } else if (dotPos[1] < bar1y - barH) {
                // Lower right corner of bar should be checked more carefully
                Vec2 barCorn({ barIndent, bar1y - barH });
                Vec2 dotCorn = dotPos + Vec2({ -dotRad, dotRad });
                Vec2 diffNorm = (dotCorn - barCorn).normalized();
                Vec2 velNorm = dotVel.normalized();
                if (velNorm[1] <= diffNorm[1])
                    dotVel[0] = std::abs(dotVel[0]);
                else if (dotPos[1] + dotRad >= bar1y - barH)
                    dotVel[1] = -std::abs(dotVel[1]);    // Bottom hit
            } else {
                // Clear hit
                dotVel[0] = std::abs(dotVel[0]);
            }
        }

        if (dotPos[0] >= static_cast<float>(wBase) - dotRad)
            dotVel[0] = -std::abs(dotVel[0]);

        
        //m_renderer.DrawRect( dotPos - rVec, dotPos + rVec, Vec4({dotVel[0] / dotSpeed, dotVel[1] / dotSpeed, dotPos[1] / h, 1.0f}));
        Vec2 bar1mid({ 2 * dotRad, bar1y });
        Vec2 barCornOffset({ dotRad, barH });
        //m_renderer.DrawRect(bar1mid - barCornOffset, bar1mid + barCornOffset);

        Vec2 bhSize({ 88.f, 88.f });
        m_renderer.DrawRectSh(bhPos - bhSize, bhPos + bhSize, Sh_BlackHole);

        m_renderer.SwapAndPoll();
    }

    return 0;
}

bool Game::Init()
{
    return true;
}
