#pragma once

namespace halodi_controller
{
    class NativeUpdateableInterface
    {
    public:
        virtual ~NativeUpdateableInterface() {}

        virtual void readStateIntoBuffer(int& index, double* buffer) = 0;
        virtual void writeCommandIntoBuffer(int& index, double* buffer) = 0;

        virtual int stateSize() = 0;
        virtual int commandSize() = 0;
    };
}
