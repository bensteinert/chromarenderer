/***************************************************************************
                         AbstractRenderCanvas.h
 ---------------------------------------------------------------------------
     begin                : Do Aug 14 2003
     copyright            : (C) 2002 by Holger Dammertz, Sabrina Skibak
     email                : holger.dammertz@informatik.uni-ulm.de
 ***************************************************************************/

#ifndef ABSTRACTRENDERCANVAS_H
#define ABSTRACTRENDERCANVAS_H

#include <math.h>
#include <stdio.h>

/**
 * This class is the base class for a canvas in which the RayTracer can
 * draw pixels. It allows to save the data as BMP or PNG images to disk.
 */
class AbstractRenderCanvas {
protected:
    int m_XResolution, m_YResolution;

    /** Each Render Canvas has to measure the time elapsed between
     * to rendering passes. The Point where this should happen is
     * eactly at the end of LeaveRenderMode! */
    float m_DeltaT;

public:
    AbstractRenderCanvas(int f_XRes, int f_YRes);

    virtual ~AbstractRenderCanvas(void);

    virtual bool Initialize() = 0;


    virtual void EnterRenderMode() { };


    virtual void LeaveRenderMode() { };

    /**
     * This method has to be implemented by the actual canvas implementation.
     */
    virtual void DrawPixel(int f_X, int f_Y, float f_R, float f_G, float f_B) = 0;

    virtual void DrawPixelUchar(int f_X, int f_Y, unsigned char f_R, unsigned char f_G, unsigned char f_B) = 0;


    int GetXResolution() { return m_XResolution; };


    int GetYResolution() { return m_YResolution; };


    float GetDeltaT() { return m_DeltaT; };
};


#endif
