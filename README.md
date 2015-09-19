from_scratch
============

This is a wannabe tile based dungeon crawler rpg game. For now it is a bare minimum 3d engine which creates a bsp tree for a given Geometry.
It uses the binary space partitioning information to create a Possible Visibility Set (PVS) from each tree leaf, in order to eliminate
rendering of non visible geometry at run time. Uses OpenGL for rendering.

level_editor project is a very simple level editor that can be used to create 2d maps. 3D geometry is generated from the 2D map which then is fed into the engine to generate the bsp tree.

Most recent branch is mapReader (need level_editor to compile in folder ../level_editor).

**Requirements**: g++ compiler, libglew-dev, freeglut3-dev

**Video**: https://www.dropbox.com/s/5smhk7dos6kqwnn/from_scratch.mp4?dl=0
