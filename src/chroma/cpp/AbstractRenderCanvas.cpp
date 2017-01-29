/**
 * Original Author: C) 2002 by Holger Dammertz
 **/

#include "AbstractRenderCanvas.h"
#include <stdlib.h>

AbstractRenderCanvas::AbstractRenderCanvas(int f_XRes, int f_YRes)
{
    m_XResolution = f_XRes;
    m_YResolution = f_YRes;
}

AbstractRenderCanvas::~AbstractRenderCanvas(void)
{
}
