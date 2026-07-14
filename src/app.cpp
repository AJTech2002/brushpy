#include "app.h"
#include <GLFW/glfw3.h>
#include <iostream>

int start() {
  std::cout << "hello world" << std::endl;

  GLFWwindow *window;

  if (!glfwInit()) {
    return 1;
  }

  window = glfwCreateWindow(640, 480, "BrushPy", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}