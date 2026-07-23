# Running Todos

### Python Integration

1. Add remaining Primitive's to Python bindings
2. Handling `glm` types, maybe need to bridge
3. Test out `pyglm` - keep as technical without wrappers initially
4. Allow bridging types to Python for
	1. Texture 
		1. Able to get any Layer's texture as a texture reference for uniforms or as well as as a buffer / float32[]
	2. Math

5. `img = tint(Image("test.png"), glm::vec4(1.0, 0.0, 0.0, 0.0))`
6. Layer 
	1. Undo Support
	2. Opacity Support
	3. Blending Function Support
7. Only render dirty region's of layers out when re-doing Canvas rendering 


### Output & Session Storage

4. Live folder watching & polling - this prevents re-opening
5. Layer output to texture implementation + `layers.json` - each folder as an artwork
