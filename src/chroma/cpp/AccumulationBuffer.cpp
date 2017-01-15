#include "AccumulationBuffer.h"


AccumulationBuffer::AccumulationBuffer() : Image(), acc(0) {
}


AccumulationBuffer::AccumulationBuffer(int sizex, int sizey) : Image(sizex, sizey), acc(0) {
}


AccumulationBuffer::~AccumulationBuffer() {
}
