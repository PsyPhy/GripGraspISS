#pragma once

#define MAX_SUBJECTS	32
#define MAX_SESSIONS	32
#define MAX_PROTOCOLS	32
#define MAX_TASKS		64
#define MAX_STEPS		128

#define	GRASP_START_GUI_CODE -99
#define	GRASP_START_TASK_CODE -88
#define	GRASP_START_STATUS_CODE -77
#define	GRASP_STOP_STATUS_CODE -66

namespace GraspGUI {

	typedef struct {
		long time;
		short subject;
		short protocol;
		short task;
		short step;
		short code;
	} GraspActionRecord;

#define GUI_ITEMS_IN_SLICE 7

	typedef struct {
		char ID[8];
		GraspActionRecord record[GUI_ITEMS_IN_SLICE];
	} GraspActionSlice;

}