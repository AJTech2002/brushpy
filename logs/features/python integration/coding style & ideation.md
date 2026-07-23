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

### rasterization 

```Python

img = Image("test.png")

# I want to be able to chain commands, perhaps Image is called a 'ComputePrimitive' and you can stack primitives and use one as input to another - or they run one after the other 

img = tint(Image("test.png"), glm::vec4(1.0, 0.0, 0.0, 0.0))
img = drop_shadow(img)
```