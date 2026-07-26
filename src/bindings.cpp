#include "canvas.h"
#include "engine.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm_caster.h"
#include "layer.h"
#include "main.h"
#include "primitive.h"
#include "primitives.h"
#include "renderer.h"
#include <iostream>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(bpy, m) {
  m.doc() = "Python bindings for the Metal Procedural Art Engine";

  // The engine is a process-lifetime singleton; intentionally never freed,
  // same as any other "lives until exit" global.
  Engine *engine = new Engine();
  engine->init();

  // ==== Engine ====

  m.def(
      "engine", []() { return &Engine::instance(); },
      py::return_value_policy::reference);

  // ==== Window / app lifecycle ====

  m.def(
      "display",
      [](Canvas *canvas) {
        std::cout << "Opening BrushPY Window" << std::endl;
        openWindow(canvas);
      },
      py::arg("canvas"));

  m.def("close", []() { closeWindow(); });

  // Non-blocking pump of pending window events; call periodically after
  // display() so the window stays responsive while Python keeps running.
  m.def("poll_events", []() { pollEvents(); });

  // Registers a callback to run on the Renderer's next draw pass.
  m.def(
      "render",
      [](py::function callback) {
        Renderer::addDrawCallback(
            [callback](Renderer *renderer) { callback(renderer); });
      },
      py::arg("callback"));

  m.def("startCommandBuffer", []() { Engine::beginCommandBuffer(); });

  m.def("endCommandBuffer", []() { Engine::endCommandBuffer(); });

  // === Image Proceesing ===
  m.def("tint", [](Primitive *input, glm::vec4 color) {
    TintFunction *tintFunction = new TintFunction(input, color);
    // tintFunction->setColor(color);
    return tintFunction;
  });

  // ==== Canvas ====

  py::class_<Canvas>(m, "Canvas")
      .def(py::init<int, int>(), py::arg("width"), py::arg("height"))
      .def("render", &Canvas::render)
      .def("render_out", &Canvas::renderOut, py::arg("path"))
      .def("add_layer", &Canvas::add, py::arg("layer"))
      .def("new_layer", &Canvas::newLayer)
      .def("width", &Canvas::width)
      .def("height", &Canvas::height);

  // ==== Layer ====

  // Canvas owns every Layer passed to add_layer() / returned by new_layer()
  // (see the ownership note in canvas.h) and deletes it when the Canvas is
  // destroyed, so the Python wrapper must not *also* free the underlying
  // object on GC - hence the py::nodelete holder.
  py::class_<Layer, std::unique_ptr<Layer, py::nodelete>>(m, "Layer")
      .def(py::init<>())
      // Overloaded rather than given pybind default values: a glm-typed
      // py::arg(...) = value default is cast to a Python object once, at
      // module-import time, which would make importing bpy at all depend on
      // the optional `glm` package even for callers who never touch it.
      .def(
          "draw",
          [](Layer *self, Primitive *primitive) { self->draw(primitive); },
          py::arg("primitive"))
      .def(
          "draw",
          [](Layer *self, Primitive *primitive, glm::mat4x4 transform_px) {
            self->draw(primitive, transform_px);
          },
          py::arg("primitive"), py::arg("transform_px"))
      .def(
          "draw",
          [](Layer *self, Primitive *primitive, glm::mat4x4 transform_px,
             glm::vec2 size_px) {
            self->draw(primitive, transform_px, size_px);
          },
          py::arg("primitive"), py::arg("transform_px"), py::arg("size_px"));

  // ==== Primitives ====

  py::class_<Primitive>(m, "Primitive").def(py::init<>());
  py::class_<TintFunction, Primitive>(m, "TintFunction").def(py::init<>());

  py::class_<Square, Primitive>(m, "Square")
      .def(py::init<glm::vec2, glm::vec4>(), py::arg("size"), py::arg("color"))
      .def_readwrite("size", &Square::size)
      .def_readwrite("color", &Square::color);

  py::class_<Circle, Primitive>(m, "Circle")
      .def(py::init<glm::vec2, glm::vec4>(), py::arg("size"), py::arg("color"))
      .def_readwrite("size", &Circle::size)
      .def_readwrite("color", &Circle::color);

  py::class_<Image, Primitive>(m, "Image")
      // Same reasoning as Layer::draw above: overloaded instead of given a
      // glm-typed pybind default, so importing bpy never requires `glm`.
      .def(py::init([](const char *imagePath) { return new Image(imagePath); }),
           py::arg("image_path"))
      .def(py::init([](const char *imagePath, glm::vec2 size) {
             return new Image(imagePath, size);
           }),
           py::arg("image_path"), py::arg("size"))
      .def(
          "set_size", [](Image *self, glm::vec2 size) { self->size = size; },
          py::arg("size"))
      .def_readwrite("size", &Image::size);
}
