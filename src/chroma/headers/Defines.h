#ifndef DEFINES_H_
#define DEFINES_H_

#include <stdint.h>
#include <cassert>

//#include "Structs.h"

//CONSTANTS AND SHORT INLINES
#define NaNLIMIT 1000000
#define APID 5838929


#ifdef _DEBUG
#define DBG_ASSERT(expr)  assert(expr);
#define DEBUG_CODE(expr) expr;
#define PRINTERR(expr) cerr << expr << endl;
#define SVGOUT(v1,v2,l) sb.svg.writeLine(v1,v2,l);

#else
#define DBG_ASSERT(expr)
#define DEBUG_CODE(expr)
#define PRINTERR(expr)
#define SVGOUT(v1,v2,l)
#endif

//#if !defined NOSDL && defined MEASURETIME
//#define MEASUREBRACKET(expr, timefield) {unsigned int timedelta = SDLTicks(); expr; timefield += SDLTicks() - timedelta;}
//#define STARTMEASUREMENT unsigned int timedelta = SDLTicks();
//#define STOPMEASUREMENT(timefield) timefield += SDLTicks() - timedelta;
//#else
//#define MEASUREBRACKET(expr,timefield) expr;
//#endif



#define _MM_ALIGN16 __attribute__ ((aligned (16)))
#define VALID_RETURN_VEC(expr) 	Vector3 eval_expr = expr;\
							assert(eval_expr<<NaNLIMIT);\
							return eval_expr;

#define SQRT2 (1.4142135f)
#define PLANK (6.62607095e10-34) //Js
#define LIGHTSPEED (299792458.0f) // m per s
#define EPS    (0.00001f)
#define ONEMINUSEPS (0.99999f)
#define PI     (3.141592653)
#define HALF_PI   (1.570796328)
#define ONE_DIV_PI_f (0.3183099f)
#define PI_f   (3.1415927f)
#define HALF_PI_f  (1.5707964f)
#define TWO_PI 6.283185308
#define TWO_PI_f (6.2831854f)

#define _28DIV23PI (0.387507687526834)
#define RAD(x) (x*(PI/180.0))
#define DEG(x) (x*(180.0/PI))
#define RR_LIMIT 0.9f
#define MOVEINTERVAL 1
#define MOVEINTERVAL2 0.1
#define MOVEINTERVAL3 0.01
#define BOUNDRGI 1.0f
#define GEOM_BOUND (0.01f)
#define PERLIN_PARAM 8
#define NAMELENGTH 32
#define TEXTOUTPUTLEN 64

#define SVG_ORANGE 4
#define SVG_RED 1
#define SVG_GREEN 2
#define SVG_BLUE 3
#define SVG_BLACK 0
#define SVG_GREY 5
#define SVG_VIOLET 8
#define SVG_AQUA 10
#define SVG_YELLOW 12

// RENDEROPTIONS
#define CLAMP				// clamp rgb values on preview window
#define DRAWASTRUCT			// draw bounding boxes of BVH
//#define BADOUEL 			// if not defined, it is Moeller Test
#define BVH 				// if not defined, its BSP

// NEEDED BY RAY/LIGHTTRACER TO USE THE RIGHT FUNCTIONS
#ifdef BVH
#define MAX_DEPTH 30
#define MIN_TRIANGLES_PER_LEAF 4
#else
#define ASTRUCT bsp
#define MAX_DEPTH 24
#define MIN_TRIANGLES_PER_LEAF 4
#endif


//FILMSIZE OF THE CAMERA
#define FILMSIZEX 36.0f
#define ASPECTRATIO 1.3333334f

typedef union {
	float f;
	uint32_t i;
}floatint;

inline int signOfFloat(float f){
	return f>=0.0f?1:-1;
}

inline int ieeeSignWOif(float f){
	floatint fi;
	fi.f = f;
	uint32_t tmp = (fi.i >> 31);
	DBG_ASSERT(tmp==(f>=0?0:1));
	return 1 - 2*tmp;
}

inline bool equals(float expr, float equal){

	if(expr < equal + EPS && expr > equal - EPS)
		return true;
	else
		return false;
}


enum Surfaces {
	Lambert, Mirror, Plastic, Glas, Metal
};

#endif /*DEFINES_H_*/
