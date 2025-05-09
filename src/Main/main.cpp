#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Main/Window.hpp"
#include "Main/Game.hpp"
#include <stb/stb_image.h>
#include "Graphics/ResourceManager.hpp"

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

Window window(SCREEN_WIDTH, SCREEN_HEIGHT);
Game game(SCREEN_WIDTH, SCREEN_HEIGHT);

void Framebuffer_size_callback(GLFWwindow *window, int width, int height);
void Key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void Mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Cursor_pos_callback(GLFWwindow* win, double xpos, double ypos);
void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
	if(!window.Init("Game", nullptr, nullptr))
		return -1;

	glViewport(0, 0, window.width, window.height);
    glfwSetFramebufferSizeCallback(window.window, Framebuffer_size_callback);

	glfwSetKeyCallback(window.window, Key_callback);

	glfwSetMouseButtonCallback(window.window, Mouse_button_callback);
	glfwSetCursorPosCallback(window.window, Cursor_pos_callback);
	glfwSetScrollCallback(window.window, Scroll_callback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init();

	game.Init();

    while(!window.ShouldClose())
    {
		float deltaTime = window.GetDeltaTime();

		game.ProcessInput(deltaTime);
		game.Update(deltaTime);

		window.BeginFrame();
		game.Render();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		game.RenderUI(deltaTime, window.GetFPS());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.EndFrame();
    }

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	ResourceManager::Clear();
	window.Clear();
    
    return 0;
}

void Framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	width = width;
	height = height;
    glViewport(0, 0, width, height);
}

void Key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if(ImGui::GetIO().WantCaptureKeyboard) return;

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if(key >=0 && key <= 1024)
	{
		if(action == GLFW_PRESS)
			game.keys[key] = true;
		else if(action == GLFW_RELEASE)
			game.keys[key] = false;
	}
}

void Mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(ImGui::GetIO().WantCaptureMouse) return;

	if(button >= 0)
	{
		if(action == GLFW_PRESS)
			game.mouse[button] = true;
		else if(action == GLFW_RELEASE)
			game.mouse[button] = false;
	}
}

void Cursor_pos_callback(GLFWwindow* win, double xpos, double ypos)
{
	if(ImGui::GetIO().WantCaptureMouse) return;

	float x = static_cast<float>(xpos) - window.width * 0.5f;
    float y = window.height * 0.5f - static_cast<float>(ypos);
    game.cursor = glm::vec2(x, y);
}

void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(ImGui::GetIO().WantCaptureMouse) return;

    game.scroll = glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset));
}
