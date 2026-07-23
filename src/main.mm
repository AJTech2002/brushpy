#include "main.h"
#include "artwork.h"
#include "canvas.h"
#include "engine.h"

int main() {
  Engine *engine = new Engine();
  engine->init();

  Canvas *canvas = artwork1();
  return display(canvas);
}
