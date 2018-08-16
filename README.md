# CurveSkel-Tabb-Medeiros-2018

## Alpha-level release with some new features, Aug. 2018

If you want the more stable release, see the [older version](https://github.com/amy-tabb/CurveSkel-Tabb-Medeiros).

Changelog:
- Option to apply algorithm to all of the connected components, or only the largest one.  The previous version only applied the algorithm to the largest connected component.
- Output the individual segments of the curve skeleton visually in a .ply file, as well as the voxel coordinates in a .txt file.
- Another input and output option of xyz coordinates. 

# Underlying ideas -- the paper

This README file is to accompany code for curve skeletonization of elongated objects that may have noisy surfaces, produced by Amy Tabb and Henry Medeiros as a companion to their paper:
	*Fast and robust curve skeletonization for real-world elongated objects*

````latex
@INPROCEEDINGS{Tabb18Fast,
author={Amy Tabb and Henry Medeiros},
booktitle={2018 IEEE Winter Conference on Applications of Computer Vision (WACV)},
title={Fast and robust curve skeletonization for real-world elongated objects},
year={2018},
pages={1935-1943},
doi={10.1109/WACV.2018.00214},
month={March},}
````

This paper is also available from arXiv:1702.07619 [cs.CV] [here](https://arxiv.org/pdf/1702.07619.pdf) -- including the supplementary material.  The arxiv version is identical in content to the IEEE version.

# Citing the code

The code may be used without restriction. If the results of the code are used as a part of a system described in a publication, we request that the authors cite the published paper at a minimum.  If you use the implementation contained in this github release as a part of a publication, we'd be thrilled if you cited the code release as well.  The citation for the code itself is: 

````latex
@electronic{tabb2018skel_code,
author = {Tabb, Amy},
year = {2018},
title = {Code from: Fast and robust curve skeletonization for real-world elongated objects},
doi = {10.15482/USDA.ADC/1399689},
owner = {Ag Data Commons},
howpublished= {\url{http://dx.doi.org/10.15482/USDA.ADC/1399689}}
}
```` 


(Other citation styles are fine -- this is one that worked for us, but we're not totally happy with it.)

Finally, all organizations have ways of assessing impact, and as someone whose work will likely never see the light of patenting since I develop algorithms, if this curve skeleton algo is useful to your work in some way and you are able to let me know in a one line email, that would be fabulous.  Examples: "We downloaded the code.  Thanks!";  "We downloaded the code and use it in our systems, in ways X, Y, Z."    

# Minimal working examples
Minimal working examples are available as a part of this Github repository, in the `demo_files` directory.  Also, this release is part of a data release hosted at Ag Data Commons, and more information may be available [there](http://dx.doi.org/10.15482/USDA.ADC/1399689).

The model may be only used for evaluation and debugging purposes of the code, and not used in any other published, or 3D printed, work without getting my permission. However, no gaurantees are expressed or implied of the code or the model.

Last update 16 August 2018.
Comments/Bugs/Problems: amy.tabb@ars.usda.gov

# Docker version

C++ compiling, linking, running not your cup of tea, or are you on another OS?  I just completed a Docker release -- [here](https://hub.docker.com/r/amytabb/curveskel-tabb-medeiros-docker/).  However, this README.md is going to be your friend, so come back.

# Compiling, Linking, Running
Basic instructions for compilation and linking:

1. This code has been written and tested on Ubuntu 14.04 and Ubuntu 16.04, using Eclipse CDT as the IDE, and is written in C/C++.  


2. This code is dependent on the >= OpenCV-3.* libraries and OpenMP library (libgomp).  These libraries should be in the include path, or specified in your IDE.


3. Compiler flags: we use OpenMP for parallelization and the C++11 standard.  Note that Eclipse's indexer marks some items from C++11 as errors (but still compiles).  

The flags needed using the gnu compiler, openmp, and the C++11 standard are: `-fopenmp  -std=gnu++11`

though depending on the compiler used, you may need different [flags](https://www.dartmouth.edu/~rc/classes/intro_openmp/compile_run.html)
	
4. 	libraries needed for linking are:
```
	gomp   [OpenMP]
	opencv_core [OpenCV]
	opencv_legacy [deprecated, some versions of OpenCV and not needed] 
	opencv_highgui
	opencv_imgproc
	opencv_imgcodecs
```
5. **Note before going further -- if in Ubuntu, you need to `apt-get install build-essential` to get C++ compiler(s) that are OpenMP enabled.  If you are on a Mac, you will need something comparable to build-essential.**
	
6. Easy way to build in Ubuntu with [Eclipse CDT](https://www.eclipse.org/cdt/) with git support (Egit): 
- `git clone` into the directory of choice.  
- Create new Managed C++ project, for the directory, select the directory where the `CurveSkel-Tabb-Medeiros-2018` was cloned into.
- The compiler and linker flags for OpenCV 4.0 will be imported.  Build and you're ready to run!

7. Easy way to build without Eclipse CDT:

```
g++ src/*.cpp -o curve_skel -fopenmp -std=gnu++11 -Wall -I/usr/local/include -lgomp -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
```

The executable `curve_skel` is created.  If you used method 6., the location will be in `Debug`.

## Running

### File formats: our file format, image stacks, xyz or occupied voxels 
 
Instructions for use:
We use a file format for representing models that is composed of a text file containing the model points `0.txt` and bounding box in file `BB.txt`.  

We include samples of these two types of files in the `demo_files/OurFileFormat` folder.  We also have some conversion functions (see 3 in this section).

#### Explanation of our file format

Our file format uses unsigned integers to represent occupied voxels in a text file, `0.txt`, which, when combined with the bounding box specifications in the bounding box file `BB.txt`, one can determine the coordinates of all of the voxels in the `0.txt` file.  The structure of `BB.txt` is:

```
1         
0 0 0
300 580 260
```

And the lines are as follows:

1. The distance between voxels.  In this example, the voxels are 1 unit apart, which would be typical for MRI or CT data.
2. Line two has the smallest coordinates of the bounding box.
3. Line three has the largest coordinates of the bounding box, plus 1. So if the largest x coordinate is 259, we list 300 here. 

Then, to decode the coordinates in  `0.txt`, inspect the conversion functions in `ReconstructionStructure.cpp`.  For instance:

````c++
void ReconstructionStructure::ReturnXYZIndicesFromIndex(int_type_t voxel_index, int_type_t& x, int_type_t& y, int_type_t& z){

	int_type_t temp_index = voxel_index;
	x = temp_index/(number_voxels_per_dim[1]*number_voxels_per_dim[2]);

	temp_index -= x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]);
	y = temp_index/(number_voxels_per_dim[2]);

	temp_index -= y*(number_voxels_per_dim[2]);

	z = temp_index;

	if (x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]) + y*(number_voxels_per_dim[2]) + z != voxel_index){
		cout << "ERROR on vox computation! " << endl << endl;
		exit(1);
	}
}
````
The user sends `voxel_index` to this function, and the `x`, `y`, `z` coordinates -- from `(0,0,0)` are initialized.  `int_type_t` is a number type that I have defined that is an unsigned integer.  I'm using properties of integer division and modulo to map single numbers from `0.txt` to `(x, y, z)` and back again.  This is similar to what is done for image coordinates, by the way. `number_voxels_per_dim` is shorthand for "number of voxels per dimension". 

Here's the opposite function:

````c++
int_type_t ReconstructionStructure::ReturnIndexFromXYZIndices(int_type_t x, int_type_t y, int_type_t z){
	if (x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]) + y*(number_voxels_per_dim[2]) + z >= number_voxels_grid){
		cout << "ERROR ON size " <<x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]) + y*(number_voxels_per_dim[2]) + z << ", n voxesl " << number_voxels_grid << endl;
		cout << "x , y, z " << x << ", " << y << ",  " << z << endl;
		exit(1);
	}

	return x*(number_voxels_per_dim[1])*(number_voxels_per_dim[2]) + y*(number_voxels_per_dim[2]) + z;

}
````

You may wonder why the heck I do this, and it is largely to save on memory and time for accessing elements -- I store the occupied voxels in a vector, versus large 3D grids with lots of empty, wasted space.  There are prices to pay for determining *where* things are, but these are relatively small prices to pay and are taken care of in the beginning, once.  Also, the size of these text files are quite small because and can be read and decoded quickly. 

### Apply algorithm to all connected components, or only the largest

A new feature in this release is the ability to apply the curve skeletonization algorithm to all of the connected components, or only the largest one.  For all of the formats except the image stack, the results are written individually by component.  This argument is a Boolean flag: 0 is only apply the algorithm to the largest component, 1 is apply to all components.  I'll abbreviate this variable as `Boolean-cc-flag`.

### Run with default threshold, our file format
1. To run the code using the default threshold (1e-12) for rejecting spurious segments, you call with two arguments, the name of the directory containing `0.txt` and `BB.txt`, and the Boolean connectioned component flag:
```
./program_name directory Boolean-cc-flag
```

### Run with selected threshold, our file format
2. To run the code with different values of the threshold, you call with three argmuents, the directory name, the Boolean connectioned component flag, followed by the threshold value [0, 1]:
```
./program_name directory Boolean-cc-flag threshold
```
### Run with xyz representation, write to xyz representation
3. We created a new sparse representation using x, y, z coordinates.  The coordinates should be integers, but may be negative.  To use the format, indicate the number of occupied voxels in the object in the file, then list the x, y, z coordinates of those voxels.  An example is given in `demo_files/XYZFormatA` and `demo_files/XYZFormatB`. The file should be named `xyz.txt.`  The beginning of one of the files is:

```
88798      [Number of points]
0 150 86   [x, y, z coordinates of an occupied voxel]
0 150 87
0 151 82
0 151 83
0 151 84
0 151 85
0 151 86
0 151 89
0 151 90
0 151 91
0 151 92
0 151 93
0 152 77
0 152 78
0 152 79
0 152 80
```

During the course of the algorithm, our file format's `0.txt` and `BB.txt` files will be created as well.  While the threshold was an option for our file format, it is mandatory for the conversion file formats, and then there is another mandatory argument for the xyz format:

```
./program_name directory Boolean-cc-flag threshold 1
```

The total number of arguments is four; the last argument is `1`.  The default threshold is `1e-12`.

### Run from an image stack representation, write to an image stack representation.
4. We have chosen the image sequence representation for conversion.  To convert models from any other format, one can use ImageJ/Fiji to load and then save as an image
sequence.  Once a sequence is created, save in a folder titled `rawimages` within the directory.  Then call with four arguments:
```
./program_name directory Boolean-cc-flag threshold 0
```

We have included an example in `demo_files/ConversionFromImageSequence`.  The result is saved in a folder called `processedimages`, which one can then load with ImageJ/Fiji to visualize in
one's preferred environment.  The result is also saved in the format described below.

### Connection Component Example
5. There's an example with two connected components in `demo_files/ConnectedComponentExample`, using our file format.  You can try to run it with:

```
./program_name directory 1 threshold 
```

(Here, we want to apply the algorithm to both connected components).  You'll then be able to look at the results.

## Results

Results format:
The results are written as .ply meshes.  One can view them with free viewer [Meshlab](http://www.meshlab.net/).  A sample of results is shown in the `demo_files/OurFileFormatResult` directory.

1. The `segments_by_cc` is a folder with details by connected component.
- `cc_1_segments_TabbMedeiros.ply` shows the segments of the curve skeleton, the number (starting from 1), indicates for which connected component.
- `results_segments1.txt` lists the voxel coordinates of individual curve skeleton segments per connected component number (the number indicates for which connected component.)
This file is in our format, and generally lists:

```
number of segments
number per segment 0
voxel_index voxel_index ......
number per segment 1
voxel_index voxel_index ......
```

For those using xyz coordinates, the format is identical except using xyz coordinates, as shown in this code snippet.  The image stack representation also writes results using the xyz representation for segments.

````c++
out << number_segments_graph << endl;
for (int_type_t i = 0; i < number_segments_graph; i++){
	number_elements = segment_paths_by_cc[cc][i].size();
	out << number_elements << endl;
	for (int_type_t j = 0; j < number_elements; j++){
		CC->ReturnXYZIndicesFromIndex(SkeletonGraphCC[cc][segment_paths_by_cc[cc][i][j]].grid_id, xd, yd, zd);

		out << xd << " " << yd << " " << zd << endl;
	}
	out << endl;
}
````
2. The `skeletons_by_cc` is a folder with full skeletons and the initial object.
- `initial1.ply` is a file shows a mesh of the original object.  This may be important for verifying that your model files are set up correctly. 
- `result_1.txt` is the sparse representation of the curve skeleton result (this will xyz for the xyz format).
- `skel_TabbMedeiros_1.ply` is the curve skeleton of the connected component indicated by the number.

3. `details.txt` shows some record of the algorithm's progress and the size of the input.  For instance, 
```
time 1.079 seconds 
Number of occupied voxels per connected component: 88798
Number possible voxels 45240000
Number of proposed tips by connected component 145
Threshold for spurious segment classification 1e-12
```
time is the run time for the algorithm to compute the curve skeleton, not including load and write time.  The initial model contained 88,798 voxels.  The user enter threshold 1e-12 or used the default, etc.

4. `trace.txt` has some small details of the run, such as what options were selected:

```
Read from /home/atabb/DemoData/CS_ADC/demo_files_with_results/OurFileFormat/0.txt
File has 1 connected components and user has selected to curve skeletonize  all of them
cc 0 size 88798
```

