#ifndef _PX_ERROR_H_
#define _PX_ERROR_H_

#include <IO.h>
#include <SourcePosition.h>
#include <Utf8String.h>

#include <unicode/ustdio.h>
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
                writeString(out, message);
            }
        }

    private:
        std::vector<Error> errors;
    };
}

#endif
