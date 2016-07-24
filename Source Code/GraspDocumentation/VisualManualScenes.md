
#define SINGLE 300
#define SERIES 150

The Visual-Manual task starts with a welcome screen that prompts the subject 
to press a button in order to start a block of trials:

CENTER( IMAGE(ready.bmp, SINGLE) )

He or she is then guided to look straight ahead and bring the head to an upright position with respect to the shoulders.
A 'laser pointer' (red dot) indicates the direction of gaze, 
which must be made to fall on the orange central target through pitch and yaw movements of the head.
Simultaneously, the halo around the subject's viewpoint into the virtual world changes color, from red when the head 
is tilted far from the upright position through yellow and cyan until it turns suddenly bright green to
indicate that the head is at the proper orientation.  
A circular arrow may appear when the head
is far from the desired posture, giving the subject an indication of which way to tilt the head:

CENTER( IMAGE(leftFarRedress.bmp, SERIES) IMAGE(leftCloserRedress.bmp, SERIES) IMAGE(leftNearRedress.bmp, SERIES) IMAGE(goodRedress.bmp, SERIES) IMAGE(rightNearRedress.bmp, SERIES) IMAGE(rightCloserRedress.bmp, SERIES) IMAGE(rightFarRedress.bmp, SERIES))

CENTER(Tilted to the Left ---------------------- Correct Orientation ---------------------- Tilted to the Right)

A target orientation is then presented to the subject in the form of a line of balls presented at the end of the tunnel:
CENTER( IMAGE(vTarget.bmp, SINGLE) )

The target disappears and the subject is prompted to tilt the head to a new orientation through the same system 
of halo colors and direction arrows as described previously:

CENTER( IMAGE(leftFarAlign.bmp, SERIES) IMAGE(leftCloserAlign.bmp, SERIES) IMAGE(leftNearAlign.bmp, SERIES) IMAGE(goodAlign.bmp, SERIES) IMAGE(rightNearAlign.bmp, SERIES) IMAGE(rightCloserAlign.bmp, SERIES) IMAGE(rightFarAlign.bmp, SERIES))

CENTER(Tilted to the Left ---------------------- Correct Orientation ---------------------- Tilted to the Right)

The subject then reproduces the remembered target orientation with the hand. 
The subject sees a) an orange spherical target at the end of the tunnel, b) a virtual representation of a tool in the visual scene (a cylinder) 
that moves with the hand in 3D visual space and c) a 'laser pointer' that projects from the tool along the axis of the fingers. 
The subject must raise the arm in front of the body, aim the tool by moving the arm in azimuth and elevation until the 
laser pointer falls on the spherical target and then rotate the palm of the hand to align with the remembered orientation of the visual target.

CENTER( IMAGE(kTool.bmp, SINGLE) )

The subject presses the Center Button on the Remote Control to validate the trial and move on to the next.
If the block of trials is completed, a message is displayed to let the subject know:

CENTER( IMAGE(blockCompleted.bmp, SINGLE))

Note that other circular prompt with text instructions may appear at different times in response to different conditions
or actions by the subject. For instance, a message will be generated if the subject does not maintain the head in the specified tilted position.
All possible messages are shown in a separate section of this document.