- Jupyter commands can work directly on stored textures
- Viewer simply composites the layer pngs & `layer.json`

This allows for a live viewer, eg:

```python
import bpy
bpy.dispay("./artwork") # Live Renderer, can be left open to composite the layers
```

```Python
canvas = bpy.open("./artwork", 800, 600)

# Commands
layer = Layer() # modifying this layer will affect the canvas
layer.add(Text("Hello World"))

canvas.addLayer(layer)

```

