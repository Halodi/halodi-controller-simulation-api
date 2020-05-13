#pragma once

#include "halodi-controller/halodicontroller.h"


namespace halodi_controller
{
    class SharedBufferImplementation : public SharedBuffer
    {
    public:
        SharedBufferImplementation(int32_t size, char *const data_);

        int32_t size();
        char *const data();

        virtual ~SharedBufferImplementation();

    private:
        int32_t  bufferSize;
        char *const dataBuffer;


    };
}

