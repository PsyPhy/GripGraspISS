
#define GUISIZE 600
#define PROMPTSIZE 420

A crew member initiates Grasp activities by selecting his or her designated subject ID in the upper-left panel of GRASP@ISS:

CENTER( IMAGE(GUIScreenshots\Login.png, GUISIZE) )

The sub-session to be performed at any given time is communicated to the crewmember via an Execution Note.
The desired sub-session ID sub-session is selected via a pull-down menu:

CENTER( IMAGE(GUIScreenshots\SelectSubsession.png, GUISIZE) )

Selecting the sub-session protocol brings up a list of tasks to be executed in a defined order. 
The first task is automatically selected:

CENTER( IMAGE(GUIScreenshots\StartTask.png, GUISIZE) )


The panel on the right then guides the crew member through the steps required by each task.
The panel may take one of four different forms.

First, the crew member is presented with one or more pages of instructions. 
Once the instructions have been read the subject proceeds to the next step by pressing the 'Next' button.
It is also possible to return to the previous step (exept when at the first step) by pressing the 'Back' button.

CENTER( IMAGE(GUIScreenshots\Instruction.png, PROMPTSIZE) )

The crew member may then be prompted to launch a particular action. 
In the example shown here, the system is ready to perform the tracker alignment. 
The crew member presses the 'Execute' button to initiate the action. 
He or she may also press 'Back' to return to the previous action or instruction (except when it is the first step in a task). 
In exceptional cases, the subject may be told by POCC to press 'Skip' to move on to the next step without performing the action.

CENTER( IMAGE(GUIScreenshots\Execute.png, PROMPTSIZE) )

<br>
<br>
<br>
<br>
<br>

Pressing 'Execute' typically causes a new program to execute, which will activate specific popup windows outside the GRASP@ISS main window.
When execution of the task terminates, focus will return to the GRASP@ISS main window and the termination status of the task is displayed.
If the task was successful, the crew member will typically see the following message allowing them to confirm and move on to the following step (by pressing 'Next')
or to return and repeat the task (by pressing 'Back').

CENTER( IMAGE(GUIScreenshots\Success.png, PROMPTSIZE) )

If the task did not complete successfully, an alert will be displayed that shows the error return code (in the 'Error Code' box) and 
that typically includes a graphical or text explanation of the anomaly. Shown here is the generic message that may be used for any 
anomalous condition, but more specific messages may also be displayed (see below). 

CENTER( IMAGE(GUIScreenshots\Alert.png, PROMPTSIZE) )

In these circumstances, the crew member is invited to repeat the task by pressing 'Retry'. 
If a second attempt also fails, the crew member has the option to move on to the next step anyway, by pressing 'Ignore' 
or to repeat the entire task from the begining by pressing 'Restart'.

The full set of possible instruction, status and alert screens are included in the following sections.

