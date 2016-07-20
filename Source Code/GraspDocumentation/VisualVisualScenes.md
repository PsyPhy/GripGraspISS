
#define SINGLE 300
#define SERIES 150

The Visual-Visual task starts with a welcome screen that prompts the subject 
to press a button in order to start a block of trials:

CENTER( IMAGE(ready.bmp, SINGLE) )

He or she is then guided to bring the head to an upright position with respect to the shoulders.
The halo around the subject's view into the virtual world changes color, from red when the head 
is tilted far from the upright position through yellow and green until it turns brighy cyan to
indicate that the head is at the proper orientation. A circular arrow may appear when the head
is far from the desired posture, giving the subject an indication of which way to tilt the head:

CENTER( IMAGE(leftFar.bmp, SERIES) IMAGE(leftCloser.bmp, SERIES) IMAGE(leftNear.bmp, SERIES) IMAGE(good.bmp, SERIES) IMAGE(rightNear.bmp, SERIES) IMAGE(rightCloser.bmp, SERIES) IMAGE(rightFar.bmp, SERIES))

CENTER(Tilted to the Left ---------------------- Correct Orientation ---------------------- Tilted to the Right)

A target orientation is then presented to the subject in the form of a line of balls presented at the end of the tunnel:

CENTER( IMAGE(vTarget.bmp, SINGLE) )

The target disappears and the subject is prompted to tilt the head to a new orientation through the same system 
of halo colors and direction arrows as described previously:

CENTER( IMAGE(leftFar.bmp, SERIES) IMAGE(leftCloser.bmp, SERIES) IMAGE(leftNear.bmp, SERIES) IMAGE(good.bmp, SERIES) IMAGE(rightNear.bmp, SERIES) IMAGE(rightCloser.bmp, SERIES) IMAGE(rightFar.bmp, SERIES))

CENTER(Tilted to the Left ---------------------- Correct Orientation ---------------------- Tilted to the Right)

The subject then reproduces the remembered target orientation by turning the virtual tool using the Remote Control. 
The subject presses the right and left arrows of the Navigation Ring on the Remote Control 
to turn the virtual tool clockwise or counter-clockwise, respectively.

CENTER( IMAGE(vTool.bmp, SINGLE) )

The subject presses the Center Button on the Remote Control to validate the trial and move on to the next.
If the block of trials is completed, a message is displayed to let the subject know:

CENTER( IMAGE(blockCompleted.bmp, SINGLE))

Note that other circular prompt with text instructions may appear at different times in response to different conditions
or actions by the subject. For instance, a message will be generated if the subject does not maintain the head in the specified tilted position.
All possible messages are shown in a separate section to this document.