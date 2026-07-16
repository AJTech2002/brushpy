#pragma once
#include "vector"

class Layer;
class Renderer;

class Canvas {

public:
  Canvas(int width, int height);
  ~Canvas();
  std::vector<Layer *> layers() const { return _layers; }
  Layer *getLayer(const int index) const { return _layers[index]; }
  void init(const Renderer *renderer);
  void draw(const Renderer *renderer);
  void dispose();
  void addLayer(Layer *layer) { _layers.push_back(layer); }
  int getLayerCount() const { return (int)_layers.size(); }
  int width() const { return _width; }
  int height() const { return _height; }

private:
  std::vector<Layer *> _layers;
  int _width;
  int _height;
};