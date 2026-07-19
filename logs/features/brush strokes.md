- how can I do a brush stroke in realtime for the latest one

```python

lightBrush = Brush(shader, options); # brush is compiled 

brushes.add(lightBrush) # this is exposed in iPad app 

layer.add (stroke(path, lightBrush)); # to do a programmatic stroke


```

Tiling will be internally done in the `StrokePrimitive` - and actually the tiling will only be needed for the realtime stroke.