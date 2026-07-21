
#include "Renderer.h"
#include "canvas.h"
#include "main.h"
#include <iostream>
#include <pybind11/pybind11.h>

PYBIND11_MODULE(bpy, m) {
  m.doc() = "Python bindings for the Metal Procedural Art Engine";

  m.def("open", [](int width, int height) {
    std::cout << "Opening BrushPY Window" << std::endl;
    start_brushpy();
  });

  pybind11::class_<Canvas>(m, "Canvas");
}