#ifndef _PX_IO_H_
#define _PX_IO_H_

#include <SourcePosition.h>
#include <Utf8String.h>

#include <unicode/ustdio.h>
#include <unicode/ustring.h>
#include <iostream>
#include <sstream>
#include <memory>


namespace px {

    static Utf8String readFile(std::istream &input)
    {
        std::stringstream buffer;
        buffer << input.rdbuf();
        return Utf8String{buffer.str()};
    }

    static void writeString(UFILE* output, const Utf8String &content)
    {
        int32_t length;
        UErrorCode errorCode = U_ZERO_ERROR;
        u_strFromUTF8(NULL, 0, &length, content.c_str(), content.byteLength(), &errorCode);
        errorCode = U_ZERO_ERROR;
        std::unique_ptr<UChar[]> utf16Message(new UChar[length + 2]);
        u_strFromUTF8(utf16Message.get(), length, &length, content.c_str(), content.byteLength(), &errorCode);
        utf16Message[length] = '\r';
        utf16Message[length + 1] = '\n';
        u_file_write(utf16Message.get(), length + 2, output);
    }

}

#endif //_PX_IO_H_
