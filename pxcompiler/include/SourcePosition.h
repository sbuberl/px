
#ifndef _PX_SOURCEPOSITION_H_
#define _PX_SOURCEPOSITION_H_

#include <Utf8String.h>

namespace px {

    struct SourcePosition
    {
        Utf8String fileName;
        size_t fileOffset;
        size_t line;
        size_t lineColumn;

        SourcePosition(const Utf8String &name) : fileName{ name }, fileOffset{ 0 }, line{ 1 }, lineColumn{ 1 }
        {

        }

        void setLocation(const SourcePosition &other)
        {
            fileOffset = other.fileOffset;
            line = other.line;
            lineColumn = other.lineColumn;
        }

        void advance(size_t length = 1)
        {
            lineColumn += length;
            fileOffset += length;
        }

        void nextLine()
        {
            ++line;
            lineColumn = 1;
        }
    };

}

#endif
