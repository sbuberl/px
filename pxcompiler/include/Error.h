#ifndef _PX_ERROR_H_
#define _PX_ERROR_H_

#include <SourcePosition.h>
#include <Utf8String.h>

#include <unicode/ustdio.h>
#include <unicode/ustring.h>
#include <memory>
#include <vector>

namespace px {

    class Error
    {
    public:
        const SourcePosition position;
        const Utf8String errorMsg;

        Error(const SourcePosition &pos, const Utf8String &error) : position{ pos }, errorMsg{ error }
        {
        }
    };

    class ErrorLog
    {
    public:
        void addError(const Error& error)
        {
            errors.push_back(error);
        }

        size_t count() const
        {
            return errors.size();
        }

        void output() const
        {
            UFILE *out = u_get_stdout();
            for (auto error : errors)
            {
                auto position = error.position;
                Utf8String message = position.fileName + "(" + std::to_string(position.line) + ", " + std::to_string(position.lineColumn) + "): " + error.errorMsg;
                int32_t length;
                UErrorCode errorCode = U_ZERO_ERROR;
                u_strFromUTF8(NULL, 0, &length, message.c_str(), message.byteLength(), &errorCode);
                errorCode = U_ZERO_ERROR;
                std::unique_ptr<UChar[]> utf16Message(new UChar[length + 2]);
                u_strFromUTF8(utf16Message.get(), length, &length, message.c_str(), message.byteLength(), &errorCode);
                utf16Message[length] = '\r';
                utf16Message[length + 1] = '\n';
                u_file_write(utf16Message.get(), length + 2, out);
            }
        }

    private:
        std::vector<Error> errors;
    };
}

#endif
