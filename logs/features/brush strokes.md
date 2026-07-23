- how can I do a brush stroke in realtime for the latest one

```python

lightBrush = Brush(shader, options); # brush is compiled 

brushes.add(lightBrush) # this is exposed in iPad app 

layer.add (stroke(path, lightBrush)); # to do a programmatic stroke


```

Tiling will be internally done in the `StrokePrimitive` - and actually the tiling will only be needed for the realtime stroke.

Given the style of programming is now immediate mode, you don't need the iPad app to actually write any code. 

The ideal workflow is that you plug into the mac via iPad and keep the iPad as simply a mirror / viewer of the current session and so the processing of the stroke etc. can all be done on the Mac - this is important, eg. checkout this example [[../private/what sets this apart from others|what sets this apart from others]] this would indicate that you can pass tensorflow outputs into the brush and if the iPad was separate either you run tensorflow on the ipad or you transmit the results each of which have a litany of problems.