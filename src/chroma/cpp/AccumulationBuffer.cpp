#include "AccumulationBuffer.h"


AccumulationBuffer::AccumulationBuffer() : Image(), acc(0)//, accArray(0)
{ }


AccumulationBuffer::AccumulationBuffer(int sizex, int sizey) : Image(sizex, sizey),
                                                               acc(0)//, accArray(new int[sizex*sizey])
{
    //memset(accArray,0,width*height*4);
}


AccumulationBuffer::~AccumulationBuffer() {
    //delete[] accArray;
}
