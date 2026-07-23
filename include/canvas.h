#pragma once
#include "layer.h"
#include "vector"

class Layer;

namespace MTL {
class ComputePipelineState;
class Texture;
};

/*
  Canvas owns its layers and its own composited output texture, so it can be
  rasterized headlessly (e.g. from Python) with no Renderer/window present.
  Renderer only ever reads Canvas::outputTexture() to present it on screen.
*/
class Canvas {

public:
  Canvas(int width, int height);
  ~Canvas();
  std::vector<Layer *> layers() const { return _layers; }
  Layer *layer(const int index) const { return _layers[index]; }
  void init();
  void draw();
  void render();
  void dispose();
  void add(Layer *layer);
  Layer *newLayer() {
    Layer *layer = new Layer();
    add(layer);
    return layer;
  }
  int layerCount() const { return (int)_layers.size(); }
  int width() const { return _width; }
  int height() const { return _height; }
  MTL::Texture *outputTexture() const { return _outputTexture; }

private:
  std::vector<Layer *> _layers;

  int _width;
  int _height;
  MTL::Texture *_outputTexture = nullptr;
};