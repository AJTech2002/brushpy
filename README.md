# Description
This project is a library to create raster art through code, the first iteration will be a simple editor where you can photobash easily using python & various shader techniques. Then I would like to make it an iPad app where you can code brushes and use the Pencil to interact with raster graphics in a way very unique only to the digital medium.

# Setup 

You will need a Mac.

Then in Root:

```
cd build && cmake .. && cd .. && cmake --build ./build && open ./bin/BrushPY.app
```

This project can compile & run without XCode however for debugging & writing shaders to generate an XCode project you can:

```
sh ./generate-xcode.sh
```

Then open with the generated XCode Project File.