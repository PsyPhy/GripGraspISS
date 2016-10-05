#define BIG(txt) <p style="font-size:110%" > txt </p>
#define HEADER(hdr) <p style="text-align:center; font-size:150%"  > hdr </p>
#define SUBHEADER(hdr) <p style="text-align:center; font-size:120%"  > hdr </p>
#define IMAGE(file,size) <img src=#file style="vertical-align:middle" width=size/>
#define CENTER(x) <p style="text-align:center" > x </p>
#define LEFT(x) <p style="text-align:left" > x </p>
#define RIGHT(x) <p style="text-align:right" > x </p>
#define COMMA ,

#define OPNOM(x) *__##x##__*
#define FFRESTRAINT		OPNOM(Floating Backrest)
#define CHAIR			OPNOM(GRIP Chair)
#define TRACKER			OPNOM(Tracking Cameras)
#define CHESTMARKERS	OPNOM(Chest Marker Array)
#define HMDMARKERS		OPNOM(Head Marker Array)
#define HANDMARKERS		OPNOM(Hand Marker Array)
#define MARKERCABLES	OPNOM(Marker Array Cables)
#define REMOTE			OPNOM(VR Headset Remote)
#define HMDCABLE		OPNOM(VR Headset Cable)
#define HMD				OPNOM(Virtual Reality Headset)
#define BIB				OPNOM(Marker Support Bib)
#define HALO			halo
#define CBUTTON			__*Select Button*__