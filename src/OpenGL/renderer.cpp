#include "renderer.h"
#include "../Utils/logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void GLAPIENTRY errorOccurredGL(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION: {
        return;
    }
    default:
        break;
    }
    printf("OpenGL error:\nSource: 0x%x\nType: 0x%x\n"
        "Id: 0x%x\nSeverity: 0x%x\n", source, type, id, severity);
    printf("%s\n", message);
    exit(-1);
}

Renderer::Renderer(int w, int h, const char* title)
    : m_initSuccess(false),
    m_window(nullptr),
    m_firstTimePoint(std::chrono::steady_clock::now()),
    m_lastTimePoint(std::chrono::steady_clock::now()),
    m_prevFrameTime(1.f / 60.f),
    m_vb(0), m_ib(0), m_va(0),
    m_shaderStorage()
{
    m_initSuccess = this->Init(w, h, title);
    m_shaderStorage.Init();

    m_firstTimePoint = std::chrono::steady_clock::now();
    m_lastTimePoint = std::chrono::steady_clock::now();
}

Renderer::~Renderer()
{
    if (m_initSuccess) {
        glDeleteBuffers(1, &m_vb);
        glDeleteBuffers(1, &m_ib);
        glDeleteVertexArrays(1, &m_va);
        glfwTerminate();
    }
}

bool Renderer::WindowShouldClose()
{
    // Calculate the frame time as well
    std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
    auto dur = curTime - m_lastTimePoint;
    m_lastTimePoint = curTime;
    m_prevFrameTime = static_cast<float>(dur.count()) / 1e9f;
    return glfwWindowShouldClose(m_window);
}

float Renderer::GetElapsedSecs() const
{
    auto dur = m_lastTimePoint - m_firstTimePoint;
    return static_cast<float>(dur.count()) / 1e9f;
}

std::pair<int, int> Renderer::GetWindowSize() const
{
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return std::make_pair(w, h);
}

Vec2 Renderer::GetMousePos() const
{
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return Vec2({static_cast<float>(x), static_cast<float>(this->GetWindowHeight() - y)});
}

bool Renderer::IsKeyDown(Key key)
{
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

void Renderer::ClearBG(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::BackGroundShader(BaseShader type, Vec2 highlightPos)
{
    auto [wi, hi] = this->GetWindowSize();
    Mat2 scale({
        2.0f / wi, 0.0f,
        0.0f, 2.0f / hi
        });
    Vec2 trans({ -1.0f, -1.0f });

    Vec2 normalizedPos = (scale * highlightPos) + trans;

    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vb);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glBindVertexArray(m_va);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    m_shaderStorage.Bind(type);
    if (type == Sh_Background)
        m_shaderStorage.GetShader(Sh_Background).SetUniform1f("u_Time", this->GetElapsedSecs());
        m_shaderStorage.GetShader(Sh_Background).SetUniform2f("u_HighlightPos", highlightPos[0], highlightPos[1]);
        m_shaderStorage.GetShader(Sh_Background).SetUniform2f("u_WindDim", static_cast<float>(wi), static_cast<float>(hi));
    glBindVertexArray(m_va);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::SwapAndPoll()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(m_window);
    /* Poll for and process events */
    glfwPollEvents();
}

void Renderer::DrawRect(Vec2 llPix, Vec2 urPix, Vec4 c)
{
    auto [wi, hi] = this->GetWindowSize();
    Mat2 scale({
        2.0f / wi, 0.0f,
        0.0f, 2.0f / hi
    });
    Vec2 trans({ -1.0f, -1.0f });

    Vec2 ll = (scale * llPix) + trans;
    Vec2 ur = (scale * urPix) + trans;

    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    float vertices[] = {
        ll[0], ll[1], 0.0f, 0.0f,
        ur[0], ll[1], 1.0f, 0.0f,
        ur[0], ur[1], 1.0f, 1.0f,
        ll[0], ur[1], 0.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vb);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glBindVertexArray(m_va);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    m_shaderStorage.Bind(Sh_ColorFill);
    m_shaderStorage.GetShader(Sh_ColorFill).SetUniform4f("u_Color", c[0], c[1], c[2], c[3]);
    glBindVertexArray(m_va);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawRectSh(Vec2 llPix, Vec2 urPix, BaseShader sh)
{
    auto [wi, hi] = this->GetWindowSize();
    Mat2 scale({
        2.0f / wi, 0.0f,
        0.0f, 2.0f / hi
        });
    Vec2 trans({ -1.0f, -1.0f });

    Vec2 ll = (scale * llPix) + trans;
    Vec2 ur = (scale * urPix) + trans;

    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    float vertices[] = {
        ll[0], ll[1], 0.0f, 0.0f,
        ur[0], ll[1], 1.0f, 0.0f,
        ur[0], ur[1], 1.0f, 1.0f,
        ll[0], ur[1], 0.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vb);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glBindVertexArray(m_va);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);

    m_shaderStorage.Bind(sh);
    if (sh == Sh_BlackHole)
        m_shaderStorage.GetShader(Sh_BlackHole).SetUniform1f("u_Time", this->GetElapsedSecs());
    glBindVertexArray(m_va);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void WindResizeCallback(GLFWwindow* wnd, int32_t w, int32_t h) {
    glViewport(0, 0, w, h);
}

bool Renderer::Init(int w, int h, const char* title)
{
    /* Initialize the library */
    if (glfwInit() == GLFW_FALSE)
        return false;

    // Debug enabling
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    // Core profile 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_POSITION_X, 80);
    glfwWindowHint(GLFW_POSITION_Y, 80);
   
    /* Create a windowed mode window and its OpenGL context */
    m_window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        return false;
    }
    //glfwSetWindowPos(m_window, 80, 80);

    /* Make the window's context current */
    glfwMakeContextCurrent(m_window);

    //Framerate
    glfwSwapInterval(1);

    // Load modern OpenGL using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        RENDERER_ERROR("Failed to initialize OpenGL context");
        glfwTerminate();
        return false;
    }

    // Print version
    const char* vers = (const char*)glGetString(GL_VERSION);
    RENDERER_INFO(std::format("OpenGL version {}", vers));

    // Debug stuff
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(errorOccurredGL, NULL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Buffers
    glGenBuffers(1, &m_vb);
    glGenBuffers(1, &m_ib);
    glCreateVertexArrays(1, &m_va);

    glfwSetWindowSizeCallback(m_window, WindResizeCallback);

    return true;
}