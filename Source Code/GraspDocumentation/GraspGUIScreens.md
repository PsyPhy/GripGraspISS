
#define GUISIZE 500
#define PROMPTSIZE 420

HEADER( __GRASP Software User Interface__ )

This document provides an overview of the steps followed by a subject or operator in order 
to run the GRASP installation and scientific protocols on the Perspectives laptop.
It covers only the software aspects, assuming that theGRASP hardware (including shared GRIP hardware) is already
installed per the PODF specific to those activities.

SUBHEADER( __*Starting GRASP on the Perspectives Laptop*__ )

A crew member initiates Grasp activities by double clicking on the GRASP icon situated on the desktop. 
This will start up first a utility program to verify the installation of the virtual reality engine and VR Headset.
The initial screen will be as follows while the VR system initializes:

CENTER( IMAGE(GUIScreenshots\OculusStartupCheck1.png, GUISIZE) )

after 10-15 seconds a VR screen should appear as follows:

CENTER( IMAGE(GUIScreenshots\OculusStartupCheck2.png, GUISIZE) )

If the VR scene does not appear after 30 seconds, the operator should press and hold for 3 seconds 
the Select button on the VR Remote and then release, as indicatded by the instructions on the screen.
This should cause the VR scene to appear in the window and in the VR Headset.
If not, the operator should contact Col-CC for further instructions.

After the VR engine has been started successfully, a window showing the status of the 3D tracker:

CENTER( IMAGE(GUIScreenshots\GraspTrackerDaemon.png, PROMPTSIZE) )

should appear for ~10 sec and then dissapear (collapse to task bar).

Finally, the GRASP main menu should appear:

CENTER( IMAGE(GUIScreenshots\Login.png, GUISIZE) )

The subject or operator selects his or her designated subject ID in the upper-left panel of GRASP@ISS.
Then the desired sub-session ID sub-session is selected via a pull-down menu:

CENTER( IMAGE(GUIScreenshots\SelectSubsession.png, GUISIZE) )

The subject ID and the sub-session to be performed at any given time is communicated to the crewmember via an Execution Note.

Selecting the sub-session protocol brings up a list of tasks to be executed in a defined order. 
The first task is automatically selected. An example is shown here:

CENTER( IMAGE(GUIScreenshots\StartTask.png, GUISIZE) )

Note that at any time the subject can press the 'Status' button to bring up a screen showing the 
visibility of the markers on various marker structures and a test window showing the status of the VR
hardware.

CENTER( IMAGE(GUIScreenshots\StatusDisplay.png, GUISIZE) )

The subject can also press the 'History' button to display a list of the tasks that have been
carried out on that day, with start and end time and an indication of the completion code for 
each task.

CENTER( IMAGE(GUIScreenshots\HistoryDisplay.png, GUISIZE) )

SUBHEADER( __*Executing Tasks*__ )

The panel on the right then guides the crew member through the steps required by each task.
The panel may take one of four different forms.

1) First, the crew member is presented with one or more pages of instructions. 
Once the instructions have been read the subject proceeds to the subsequent page by pressing the 'Next' button.
It is also possible to return to the previous step (exept when at the first step) by pressing the 'Back' button.

CENTER( IMAGE(GUIScreenshots\Instruction.png, PROMPTSIZE) )

2) The crew member may then be prompted to launch a particular action. 
In the example shown here, the system is ready to perform the tracker alignment. 
The crew member presses the 'Execute' button to initiate the action. 
He or she may also press 'Back' to return to the previous action or instruction (except when it is the first step in a task). 
In exceptional cases, the subject may be told by COL-CC to press 'Skip' to move on to the next step without performing the action.

CENTER( IMAGE(GUIScreenshots\Execute.png, PROMPTSIZE) )

<br>
<br>

3) Pressing 'Execute' typically causes a new program to execute, which will activate specific popup windows outside the GRASP@ISS main window.
When execution of the task terminates, focus will return to the GRASP@ISS main window and the termination status of the task is displayed.
If the task was successful, the crew member will typically see the following message allowing them to confirm and move on to the following step (by pressing 'Next')
or to return and repeat the task (by pressing 'Back').

CENTER( IMAGE(GUIScreenshots\Success.png, PROMPTSIZE) )

4) If the task did not complete successfully, an alert will be displayed that shows the error return code (in the 'Error Code' box) and 
that typically includes a graphical or text explanation of the anomaly. Shown here is the generic message that may be used for any 
anomalous condition, but more specific messages may also be displayed (see below). 

CENTER( IMAGE(GUIScreenshots\Alert.png, PROMPTSIZE) )

In these circumstances, the crew member is invited to repeat the task by pressing 'Retry'. 
If a second attempt also fails, the crew member has the option to move on to the next step anyway, by pressing 'Ignore' 
or to repeat the entire task from the begining by pressing 'Restart'.

The full set of possible instruction, status and alert screens are included in the following sections.

