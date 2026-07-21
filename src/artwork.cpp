#include "artwork.h"
#include "canvas.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "layer.h"
#include "primitives.h"
#include "renderer.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void artwork1() {
  auto callbck = [](Renderer *renderer) {
    std::cout << "~~ Running Artwork 1 ~~" << std::endl;
    Canvas *canvas = new Canvas();

    Layer *layer = new Layer();

    canvas->addLayer(layer);

    Square *square =
        new Square(glm::vec2(100, 100), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    Circle *circle =
        new Circle(glm::vec2(30, 30), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    Image *image =
        new Image("/Users/ajayvenkat/Development/metal-procedural-art/test.png",
                  glm::vec2(100, 100));

    // TODO: This has to be done within a draw call

    glm::mat4x4 transform = glm::mat4x4(1.0f);
    transform = glm::translate(transform, glm::vec3(100, 100, 0));
    transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(0, 0, 1));
    // transform = glm::scale(transform, glm::vec3(2.0f, 1.0f, 1.0f));

    layer->add(image, transform);

    square->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    transform = glm::mat4x4(1.0f);

    transform = glm::translate(transform, glm::vec3(300, 300, 0));
    // transform = glm::rotate(transform, glm::radians(30.0f), glm::vec3(0, 0,
    // 1));
    layer->add(square, transform);
    layer->add(circle, transform);
  };
  Renderer::addDrawCallback(callbck);
}