# Compilation
To compile and run, use the default premake/make commands in the A1 directory:

``` 
$ premake4 gmake
$ make
$ ./A1
```

The program was tested on machine `gl38`.

# Manual
## Comments / Known Issues
### Persistence at Higher FPS
The persistence feature seems to not function as smoothly on systems allowing for higher FPS, you have to *really* move the mouse to get it to spin. My guess as to why this is occuring is because my mouse `delta x` calculation is done on each frame, and not based on the mouse movement handler. On my home system, which runs at 75 fps, I believe that the system is able to pick up a difference in mouse movement, but since the higher fps system is polling the mouse much quicker rate, it is not catching the difference in mouse x.

I did not have time to fix this bug, but I would add a "buffer" of `n` frames, based on the FPS rate of the system, after which I would poll the position of the mouse to get a `delta x`.

## Assumptions
### Persistence
The A1 specification mentioned:

> If, while rotating, the mouse is moving at the time that the button is released, the rotation should continue on its own. This decision should be made at the time of release; after that, it should persist independently of mouse movement, until the next button press.

I assumed the "button" to be referring to the left mouse button. The rotation will begin with left mouse-button click-and-drag, and when in persistence, rotation will stop only when the left mouse button is pressed again.

## Extra Commands/Features

## Additional data files
