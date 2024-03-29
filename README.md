# Vector Field Simulaitor
This C program can visualize Vector fields using the SDL library.

## How to use
To add your vector field you need to add a function that accepts a vec2 and returns vec2.
Then you need to change the function in use to your function in one places:
* In 'setup' func. In the line:

``` C
my_field_function = V_source_p;
```

After that, you are basically good to go (given that you have SDL installed on your machine - https://wiki.libsdl.org/SDL2/Installation).

## Overview of the code
* Every time you change something, you update the 2 matrixes that describe the field 
    * size matrix
    * theta matrix

* The program works in a polar coordinate system. (your vector field function does not have to, but make sure to convert the output from your function)
Then the program renders the field into a texture that you can display at any time.

* the particles have *NO* connection to the matrixes of the vector field. So you need to make sure they work on the same vector field function.


* You can limit the FPS by uncommenting the line (like in the example):
``` C
    if (time_to_wait > 0 && time_to_wait < FRAME_TARGET_TIME) {
        // SDL_Delay(time_to_wait);
    }
```
in the 'fix framerate' function.


## Optional Input
There are several supported inputs to the program:
* ESC - free all the memory and close the window.
* r - reset all the values to default.
* p - print the current 'vector field size' matrix.
* h - hide the center of the field.
* d - delete all particles.
* arrow key up - zoom in.
* arrow key down - zoom out.
* left mouse button - move the field.
* right mouse button - spawn a new particle if possible.
* scroll wheel - increase or decrease the resolution of the field.


## License

[MIT](https://choosealicense.com/licenses/mit/)
