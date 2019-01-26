SINDY WANG (500766809)

CPS511 Assignment 2

For this assignment I used Microsoft Visual Studio
To run the code:

Create a new project in Visual Studio

Theres a HTML file that shows you how to set up a new project linked

After doing that, put all the C and header files into the source files 
section in the project, also put buildings.txt in souce files too.

Press run, and if there an error about a freeglut.dll file, go to your 
freeglut folder and go into bin and copy freeglut.dll

Now to go the project folder and into the Debug folder, paste the freeglut.dll 
file there.

After that, the program should run.


What works:
Creating a solid cube to move, and scale x,y,z.
 - use F1 to create new cube
 - use T to translate
 - use H to scale height
 - use S to scale X and Z

Camera
 - use left click on mouse to drag screen around
 - use U to zoom in
 - use J to zoom out 

Extrusion
 - use E to make the extruded mesh
 - to make another building, press P to reset the solid cube then use F1 again to spawn the cube

Writing and reading to file
 - when you use E, the values of the mesh are saved into the file buildings.txt

What doesn't work:
 - Polygon changing and math functoin for it (where you drag to change the base of building)
 - Values of the buildings are saved in file and can read the values but can't draw them
 - When pressing E, only the first building you make shows and the others don't
 