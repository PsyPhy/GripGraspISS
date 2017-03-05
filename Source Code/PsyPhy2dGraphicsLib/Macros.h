/****************************************************************************/
/*                                                                          */
/*                               Macros.h                                   */
/*                                                                          */
/****************************************************************************/

// Disable warnings about unsafe functions.
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6
#define _CRT_SECURE_NO_WARNINGS

/*    (c) Copyright 1994, 1995 - PsyPhy Consulting. All Rights Reserved.    */

#ifndef	_GraphicsMacros_

#include "..\Useful\Useful.h"

#ifdef __cplusplus 
extern "C" {
#endif

/*
    These macros allow us to write generic graphic programs, without
    regard for the actual device. This would be better accomplished
    in C++, but we don't have it for OS9.
*/

#define Point(obj, x, y)		      (*(obj->point))(obj, x, y)
#define	Moveto(obj, x, y)		      (*(obj->moveto))(obj, x, y)  	
#define	Lineto(obj, x, y)		      (*(obj->lineto))(obj, x, y)	      
#define Line(obj, x1, y1, x2, y2)	(*(obj->line))(obj, x1, y1, x2, y2)

#define Rectangle(obj, x1, y1, x2, y2)	      (*(obj->rectangle))(obj, x1,y1,x2,y2)
#define FilledRectangle(obj, x1, y1, x2, y2)	(*(obj->filled_rectangle))(obj, x1,y1,x2,y2)
#define Circle(obj, x, y, r)		              (*(obj->circle))(obj, x,y,r)
#define FilledCircle(obj, x, y, r)	          (*(obj->filled_circle))(obj, x,y,r)

#define	StartTrace(obj, x, y)		    (*(obj->start_trace))(obj, x, y)	      
#define	ContinueTrace(obj, x, y)		(*(obj->continue_trace))(obj, x, y)	      
#define	EndTrace(obj, x, y)		      (*(obj->end_trace))(obj, x, y)	      

#define StartPolygon(obj) 		  (*(obj->start_polygon))(obj)
#define AddVertex(obj, x, y) 		(*(obj->add_vertex))(obj, x, y)
#define OutlinePolygon(obj) 		(*(obj->outline_polygon))(obj)
#define FillPolygon(obj) 		    (*(obj->fill_polygon))(obj)

#define Erase(obj)		        (*(obj->erase))(obj)
#define EraseRectangle(obj,x1,y1,x2,y2)	(*(obj->erase_rectangle))(obj,x1,y1,x2,y2)
#define LineStyle(obj, sty)		(*(obj->set_style))(obj, sty)
#define	LinePattern(obj, pat)		(*(obj->set_pattern))(obj, pat)
#define Color(obj, col)			(*(obj->set_color))(obj, col)
#define ColorRGB(obj, r, g, b)			(*(obj->set_color_rgb))(obj, r, g, b)
#define Alu(obj, alu)			(*(obj->set_alu))(obj, alu)
#define Pen(obj, pen)			(*(obj->set_pen))(obj, pen)

#define Text(obj, str, x, y, dir)	(*(obj->text))(obj, str, x, y, dir)
#define	TextWidth(obj, str)		(*(obj->text_width))(obj, str)
#define	TextHeight(obj, str)		(*(obj->text_height))(obj, str)

#define Init(obj)			(*(obj->init))(obj)
#define InitDisplay(obj)		DisplayInit(obj)
#define Close(obj)			(*(obj->close))(obj)
#define Hardcopy(obj,filename)			(*(obj->hardcopy))(obj, filename)

#ifdef __cplusplus 
}
#endif

#define	_GraphicsMacros_

#endif

