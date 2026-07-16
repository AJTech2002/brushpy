#pragma once
#include "vector"

class Layer;
class Renderer;

class Canvas {

public:
  Canvas();
  ~Canvas();
  std::vector<Layer *> layers() const;
  Layer *getLayer(const int index) const;
  void init(const Renderer *renderer);
  void draw(const Renderer *renderer);
  void dispose();
  void addLayer(Layer *layer);
  int getLayerCount() const;
  int width() const;
  int height() const;

private:
  std::vector<Layer *> _layers;
};