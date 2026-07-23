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

### Design

- **Python/C++ Engine**
	- This can independently be run with Python code alone
	- Can work attached to a specific folder which contains the metadata about the artwork 
	- This way artworks can be limited to code without needing the visual renderer 
- **Display**
- **Interaction**

