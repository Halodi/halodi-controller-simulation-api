#include "sharedbufferimplementation.h"
namespace halodi_controller
{


SharedBufferImplementation::SharedBufferImplementation(int32_t size, char * const data_) :
        bufferSize(size),
        dataBuffer(data_)
{

}

int32_t SharedBufferImplementation::size()
{
    return bufferSize;
}

char *SharedBufferImplementation::data()
{
    return dataBuffer;
}

SharedBufferImplementation::~SharedBufferImplementation()
{

}


}
