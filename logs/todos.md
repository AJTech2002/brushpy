# Running Todos

~~Ensure Layer has an initialized MTL Texture Output~~

~~Add ability to create a layer, add to canvas 
Add ability to create a primitive, add to layer 
Change region to the whole for now~~

Create Square Primitive and render to layer with Uniform Support
Abstract Square Primitive to support easier Primitive Creation or Refactor Primitive Metal code into another Metal File 

Current pain points:
- Art work needs a lifecycle hooks to place render commands in the right place 
	- `draw()`
- Should be called once on Renderer:submitDraw


==

- Dont want to call Primitive's init
- Don't want to allocate an instance with static props
- Dont want Primitive to store its own Compute 

User's wont be creating primitives.
