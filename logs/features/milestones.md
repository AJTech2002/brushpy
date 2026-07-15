1. C++ Base Architecture for `Layer` and `Canvas` and adding `Primitives` and the whole dirty region tracking/hierarchy management pipeline and ensure rendering can be done quickly even with lots of layer cloning etc. 
	1. Compositing and layer updates can be decoupled etc. 
	2. Make 1 complete art work with ideation + coding a specific file for it `Artwork`
	3. Maybe QuadTree to most efficiently isolate the changes from different layers and update everything in the least amount of passes as possible

2. Base Features
	1. Scene Graph & Persistence
	2. Base Interactions 
		1. Masking with Mouse 

3. Python Integration + Quality of Life Features (Caching by line etc.) + VScode Integration + Optimizations of Engine

4. Support for iPad & Apple Pencil Companion App 
5. 