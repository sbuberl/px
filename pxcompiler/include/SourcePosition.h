
#ifndef SOURCEPOSITION_H_
#define SOURCEPOSITION_H_

namespace px {

    struct SourcePosition
    {
        size_t fileOffset;
        size_t line;
        size_t lineColumn;

        SourcePosition() : fileOffset{ 0 }, line{ 1 }, lineColumn{ 1 }
        {

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
