```python

# Inputs / Outputs are defined in a controlled way + binding is done automatically
const blendShader = """1
COLOR = mix(LAST, CUR, 0.5);
"""

let layer = Layer()
layer.add(img, blendShader)

canvas.add(layer); # Only here does the render logic get done

layer.add(stroke, blendShader); # marks parent tree dirty which re-renders whatever changed

# Layer contains an output raster the size of the canvas, but also knows it's dirty region

for i in range(0,10):
	let clone = layer.clone()
	clone.translate(-5, 10);


```

### render chain