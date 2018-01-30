# pikassho
wxWidgets OpenGL canvas "image generator"

This is a small expriment made by me by a lot of... help from various sources.  
Shows how to use OpenGL glsl shaders with wxWidgets, something I have yet to see an example for.  
There are some color pickers (non-working) besides the open gl canvas. The idea was that you could pick colors to use 
for drawing OpenGL textures. While the colors in the texture is indeed the ones show initally in the color pickers,
it does not change when the user selects another color.  
There is also a save button, that actually is working. Saves the output of the OpenGL. 
Sadly it gives the resulting image an transparent half as it sets the width to whole screen.

The function of the program is that it random generates x amount of colors (4 + up to additional 4 I belive).
Then it pass the colors to the OpenGL function that generates a texture based on the colors.
The texture is supposed to be "random". It puts colors at random points on the texture and then use the first best
color closest.

How to compile:  
Beats me, really.  
Change the makefile according to your system. Change "wx-config-gtk3" to whatever wx-config config is called.  
Run make and then execute pikassho. You should see a GUI window.  
Requires wxWidgets and libPNG and probablly som other stuff. OpenGL headers, GLU, GLEW. Modern C++ compiler.
