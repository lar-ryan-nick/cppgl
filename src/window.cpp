#include "window.h"

Window::Window(const std::string& windowName, int w, int h, float r, float g, float b, float a) : backgroundColor(r, g, b, a) {
	window = glfwCreateWindow(w, h, windowName.c_str(), NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	view = new View(0, 0, width, height);
	glfwSetWindowUserPointer(window, view);
	glfwSetScrollCallback(window, scrollCallback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPosCallback(window, &mouseCallback);
}

Window::~Window() {
	delete view;
}

View& Window::getView() {
	return *view;
}

void Window::processInput() {
}

void Window::render() {
	glfwMakeContextCurrent(window);
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(backgroundColor.getRed(), backgroundColor.getGreen(), backgroundColor.getBlue(), backgroundColor.getAlpha());
	glClear(GL_COLOR_BUFFER_BIT);
	Polygon maxPolygon;
	maxPolygon.addVertex(glm::vec2(-1.0f, 1.0f));
	maxPolygon.addVertex(glm::vec2(1.0f, 1.0f));
	maxPolygon.addVertex(glm::vec2(1.0f, -1.0f));
	maxPolygon.addVertex(glm::vec2(-1.0f, -1.0f));
	view->draw(0.0f, 0.0f, width, height, glm::mat4(), Polygon());
	glfwSwapBuffers(window);
}

Size Window::getSize() const {
	int width = 0, height = 0;
	glfwGetWindowSize(window, &width, &height);
	return Size(width, height);
}

Color Window::getBackgroundColor() {
	return backgroundColor;
}

void Window::setBackgroundColor(const Color& bc) {
	backgroundColor = bc;
}

void Window::close() {
	glfwDestroyWindow(window);
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
}

void Window::scrollCallback(GLFWwindow* w, double xOffset, double yOffset) {
	View* view = static_cast<View*>(glfwGetWindowUserPointer(w));
	double xpos, ypos;
	glfwGetCursorPos(w, &xpos, &ypos);
	int width = 0, height = 0;
	glfwGetWindowSize(w, &width, &height);
	int frameWidth = 0, frameHeight = 0;
	glfwGetFramebufferSize(w, &frameWidth, &frameHeight);
	xpos *= frameWidth / width;
	ypos *= frameHeight / height;
	view->scroll(2 * xOffset, 2 * yOffset, xpos, ypos);
}

void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	static bool firstCall = true;
	static float lastX = xpos;
	static float lastY = ypos;
	if (!firstCall) {
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
		lastX = xpos;
		lastY = ypos;
	} else {
		firstCall = false;
	}
}
