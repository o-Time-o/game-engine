#include "Main/Window.hpp"
#include <iostream>

bool Window::Init(const char *title, GLFWmonitor *monitor, GLFWwindow *share)
{
	if(!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(width, height, title, monitor, share);

    if(!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

	return true;
}

float Window::GetDeltaTime()
{
	static float lastFrame = glfwGetTime();
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    return deltaTime;
}

double Window::GetFPS()
{
	static double lastTime = glfwGetTime();
    static int frameCount = 0;
    static double fps = 0.0;

    double currentTime = glfwGetTime();
    frameCount++;

    if (currentTime - lastTime >= 1.0) {
        fps = double(frameCount) / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }

    return fps;
}

void Window::BeginFrame()
{
	glfwPollEvents();
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::EndFrame()
{
	glfwSwapBuffers(window);
}

void Window::Clear()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
