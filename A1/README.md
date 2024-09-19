# Compilation
To compile and run, use the default premake/make commands in the A1 directory:

``` 
$ premake4 gmake
$ make
$ ./A1
```

# Manual
## Assumptions
### Persistence
The A1 specification mentioned:

> If, while rotating, the mouse is moving at the time that the button is released, the rotation should continue on its own. This decision should be made at the time of release; after that, it should persist independently of mouse movement, until the next button press.

I assumed the "button" to be referring to the left mouse button. The rotation will begin with left mouse-button click-and-drag, and when in persistence, rotation will stop only when the left mouse button is pressed again.

## Extra Commands/Features

## Additional data files
