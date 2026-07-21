
#include "Renderer.h"
#include "canvas.h"
#include "main.h"
#include <iostream>
#include <pybind11/pybind11.h>

PYBIND11_MODULE(bpy, m) {
  m.doc() = "Python bindings for the Metal Procedural Art Engine";

  m.def("open", [](int width, int height) {
    std::cout << "Opening BrushPY Window" << std::endl;
    start_brushpy(width, height);
  });

  pybind11::class_<Canvas>(m, "Canvas")
      .def(pybind11::init<>())
      .def("render", &Canvas::render);
  //   .def("draw", &Canvas::draw)
  //   .def("addLayer", &Canvas::addLayer)
  //   .def("layerCount", &Canvas::layerCount)
  //   .def("width", &Canvas::width)
  //   .def("height", &Canvas::height);
}