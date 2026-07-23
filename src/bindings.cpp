
#include "Renderer.h"
#include "canvas.h"
#include "engine.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm_caster.h"
#include "layer.h"
#include "main.h"
#include "primitive.h"
#include "primitives.h"
#include "pybind11/cast.h"
#include <array>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

static const std::array<float, 16> identity{1, 0, 0, 0, 0, 1, 0, 0,
                                            0, 0, 1, 0, 0, 0, 0, 1};

PYBIND11_MODULE(bpy, m) {

  Engine *engine = new Engine();
  engine->init();

  m.def(
      "engine", []() { return &Engine::instance(); },
      pybind11::return_value_policy::reference);

  m.doc() = "Python bindings for the Metal Procedural Art Engine";

  m.def("display", [](Canvas *canvas) {
    std::cout << "Opening BrushPY Window" << std::endl;
    openWindow(canvas);
  });

  m.def("close", []() { closeWindow(); });

  // Non-blocking pump of pending window events; call periodically after
  // display() so the window stays responsive while Python keeps running.
  m.def("poll_events", []() { pollEvents(); });

  // Take in function pointer and add it to the draw callbacks
  m.def("render", [](pybind11::function callback) {
    Renderer::addDrawCallback(
        [callback](Renderer *renderer) { callback(renderer); });
  });

  pybind11::class_<Canvas>(m, "Canvas")
      .def(pybind11::init<>(
               [](int width, int height) { return new Canvas(width, height); }),
           pybind11::arg("width"), pybind11::arg("height"))
      .def("render", &Canvas::render)
      //   .def("draw", &Canvas::draw)
      .def("addLayer", &Canvas::add)
      //   .def("layerCount", &Canvas::layerCount)
      .def("width", &Canvas::width)
      .def("height", &Canvas::height)
      .def("newLayer", &Canvas::newLayer);

  pybind11::class_<Layer>(m, "Layer")
      .def(pybind11::init<>())
      .def(
          "draw",
          [](Layer *self, Primitive *primitive, glm::mat4x4 matrix) {
            self->draw(primitive, matrix, glm::vec2(0, 0));
          },
          pybind11::arg("primitive"), pybind11::arg("matrix") = identity);

  pybind11::class_<Primitive>(m, "Primitive").def("render", &Primitive::render);

  pybind11::class_<Square, Primitive>(m, "Square")
      .def(pybind11::init([](float width, float height, float r, float g,
                             float b, float a) {
             return Square(glm::vec2(width, height), glm::vec4(r, g, b, a));
           }),
           pybind11::arg("width"), pybind11::arg("height"), pybind11::arg("r"),
           pybind11::arg("g"), pybind11::arg("b"), pybind11::arg("a"))
      .def_readwrite("size", &Square::size)
      .def_readwrite("color", &Square::color);

  pybind11::class_<Circle, Primitive>(m, "Circle")
      .def(pybind11::init([](float width, float height, float r, float g,
                             float b, float a) {
             return Circle(glm::vec2(width, height), glm::vec4(r, g, b, a));
           }),
           pybind11::arg("width"), pybind11::arg("height"), pybind11::arg("r"),
           pybind11::arg("g"), pybind11::arg("b"), pybind11::arg("a"))
      .def_readwrite("size", &Circle::size)
      .def_readwrite("color", &Circle::color);

  pybind11::class_<Image, Primitive>(m, "Image")
      .def(pybind11::init([](const char *imagePath) {
        return Image(imagePath, glm::vec2(0, 0));
      }))
      .def("setSize", [](Image *self, glm::vec2 size) { self->size = size; });
}
