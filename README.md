## Outline

This repo combines 2 assignments for an advanced cpp programming course.

1- Assignment 4

in this assignment you will get familiar with C++’s IO tools and concurrency. Your goal is to create a Nifti
image loader which is capable of asynchronously reading .nii files from disk and then displaying the image on an
OpenGL texture.

Part 1: Nifti image loader (75%) Nifti files are image files ending in a .nii extension, commonly used to store 3d or
4d medical imaging data. I have provided you with a single 3d nifti image (example below) called t1.nii.gz (the .gz
indicates that it is compressed) on moodle.

![Image](https://github.com/user-attachments/assets/d890d823-b725-48ca-9ee1-62023ca04275)

As you can see it is a 3d MRI image of a brain. Each panel shows an orthogonal ‘slice’ through the image (axial,
sagittal, and coronal slices respectively from left to right).

Your task is to create a C++ program that can read this nifti image and then display it as a texture using opengl. You
will need to read up on the nifti image specification (resources below) which gives the header format. Once you are
able to read the header, you can use the information provided in the header to read the full image data and place it
in a 3d array and then show a single axial slice (leftmost image above) on a texture using OpenGL. Your program
should also check to see if the image is zipped (in .gz format) and if so, it should unzip the image before loading it.

https://brainder.org/2012/09/23/the-nifti-file-format/

https://brainder.org/2012/09/23/the-nifti-file-format/

Use C++’s std IO stream libraries to handle the image loading. You can also check github for examples of how to
read the image data, for example here is a class representing a Nifti image header. You can do something similar
(represent the header as its own C++ class, and the image itself as a separate class).

Part 2: Concurrency (25%): once you have succeeded in loading the image and displaying an axial slice on an
opengl texture, refactor your code such that the image loading is done asynchronously. This means that your
program will have two threads:

1. A main thread that is running the OpenGL window, handling user input (if any), etc.
2. A second thread that handles the unzipping and loading of the nifti image. This thread will terminate once the image has completed loading.

The purpose of using two threads is to allow the user to continue interacting with the program while the image is
being loaded. For the purposes of this assignment, you can display a slowly rotating triangle in the opengl window
that will be replaced by the axial slice once the image finishes loading (the rotating triangle is simply to show that
your main thread is still active while the image loads in the background).

Feel free to add some extras such as showing all 3 views (axial, sagittal, and coronal), allowing the user to scroll
through the slices using the mouse, or a button to load other images from the file system.

2- Assignment 5

in this assignment you will extend your work from assignment 4 to handle 4D images (functional
MRI) as well as the 3D structural images you worked with in assignment 4.

Detailed specs:

1. Load 4d images (separate thread) - [x] Your program should load 4d nifti images (you can use the same code you used in
   assignment 4, it should work if you implemented the nifti format loader correctly). The
   difference between a 4d and a 3d image is 4d images are comprised of multiple 3d
   volumes (a 4d image is a time series of 3d volumes, see the explanation video for more
   details). Because the 4d image takes some time to load, it should be done on a separate
   thread (as with assignment 4).

2. Three anatomical views with slice scrolling capability (main thread) - [x] Once the 4d image has loaded on the separate thread, display the 0th volume (which is a 3d
   image) from the time series in three separate windows corresponding to coronal, sagittal,
   and axial anatomical views. Each view should have some type of slider or arrow icons to
   manipulate the current slice, so the user can scroll through the slices. This can take place
   on the main thread. Optionally, you can also give the user the ability to click somewhere in
   one of the 3 windows which will change the slices index in the other two windows (for
   example if they click in the middle of the brain in the sagittal slice window, this will change
   the index of both the axial and coronal slice windows, see video).

3. Time series scrolling capability (main thread) - [x] If the currently loaded image is 4d, give the user the option to select a button called ‘graph’
   which will display the time series for the currently selected voxel in a separate window. The
   currently selected voxel is determined by the slice index that the user has selected by
   navigating through the coronal, sagittal, and axial anatomical views (see video for more
   details). The time series window must also support scrolling functionality. Scrolling through
   the time series will change the currently selected volume from the 0th volume (displayed by
   default when the image loads) to whichever volume has been scrolled to (see video for
   example of how this works).

## Expected results for Assignment 5

![Image](https://github.com/user-attachments/assets/ec50fff7-e8ac-449d-bcc3-14455f0ba0c4)
