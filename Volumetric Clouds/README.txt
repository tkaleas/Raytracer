Volumetric Cloud Renderer
--------------------------

Everything works like its supposed to except there is an extra mode in this application.

Commands
--------

anim -> produces a preset animation (hardcoded, may not work properly, if the right files aren't included)
	
	Note: these should match up 

gen -> goes to cloud generating mode

exit -> exits out of the application or switches back to rendering mode if currently in gen mode.
	

	
Cloud Generating Mode
---------------------

type in a new configuration file for voxel grid generating.
Grids are created using a Perlin Noise function that I adjusted for what I thought looked like clouds (adjustment of turbulence).

Voxel Grid Configuration File
------------------------------
 
FILE cg_gen_testgrid2.txt 			-> voxel grid file to output
PRST 1.3					-> persistence value in perlin noise function (how much to scale the noise function on each iteration)
OCTSIZE 10					-> number of octaves or additions to perform during the perlin noise function
VSIZE 0.05					-> voxel size
GSIZE 72					-> grid size (# of voxels)
TIME 1.2					-> current "time" for getting static images
PAD 2						-> # of voxels to pad the voxel grid with 0.0's



So the extra stuff I have implemented is:

	-Tri-Linear Interpolation
	-Voxel Cloud Generation (Using Perlin Noise)
	-Animation (I've included the file in the same directory)