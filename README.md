# Quick3D
Qt based 3D engine with geo-referenced entities

This used to be a very simple 3D engine that could show a 3D terrain and objects.
But it has been pushed much further and is now a general purpose engine.

## Building

Open .pro file in Qt Creator and build **without** shadow build option.

## Features

* Global terrain with automatic level of detail. Can be generated from real earth elevation data or procedurally and can be populated with vegetation.
* Geo-referenced objects, with parent-child relationship
* Q3D file format, with a python script to export from Blender
* Scenes are setup in a XML file or at runtime
* Basic animation classes

## Things to code or to fix

### Optimize
* Frame rate drops too fast when scene goes up in complexity. Probably too much overhead on object/material setup.

### Terrain
* Tile seams : for now, seams between terrain tiles are visible. Should create small patches on edges to match tiles.
* Z-fighting : should use an adaptive z-buffer depth to avoid this.
* Loading : sometimes tiles simply don't show up.
* Vegetation rendering : vegetation objects (and all alpha containing objects) should be rendered last to avoid alpha problems.
* Vegetation spawning : still some bug in this algorithm.

### Animation
* Skinning : nothing done yet.

### Camera
* Frustum : sometimes objects disappear because they are not considered inside the viewing frustum (although we know they are).

### Materials
* PBR rendering : allow scripting of PBR methods for realistic shading.
* Shaders: there are some artefacts due to bad looping when using perlin noise.
