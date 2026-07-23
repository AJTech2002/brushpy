```Python
import time
import random
import bpy
from pyglm import glm

canvas = bpy.Canvas(800, 600)
print("Rendering artwork...")
layer = bpy.Layer()

canvas.add_layer(layer)


while True:
    for i in range(100):
        # random color
        col = (random.random(), random.random(), random.random())
        square = bpy.Square(glm.vec2(10*(100-i), 10*(100-i)), glm.vec4(col[0], col[1], col[2], 1.0))
        image = bpy.Image("/Users/ajayvenkat/Development/metal-procedural-art/test.png")
        size = glm.vec2(10*(100-i), 10*(100-i))
        image.set_size(size)

    
        layer.draw(square)
    canvas.render_out("/Users/ajayvenkat/Development/metal-procedural-art/artworks/test.png")
    time.sleep(0.01)


```