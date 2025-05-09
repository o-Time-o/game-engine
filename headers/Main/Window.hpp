#ifndef WINDOW_CLASS
#define WINDOW_CLASS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	GLFWwindow* window;
	unsigned int width, height;

	Window(unsigned int width, unsigned int height) : width(width), height(height) {}

	bool Init(const char *title, GLFWmonitor *monitor, GLFWwindow *share);
	float GetDeltaTime();
	double GetFPS();
	bool ShouldClose() const { return glfwWindowShouldClose(window); }
	void BeginFrame();
	void EndFrame();
	void Clear();
};

#endif
