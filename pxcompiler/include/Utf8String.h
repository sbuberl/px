
#ifndef UTF8STRING_H_
#define UTF8STRING_H_

#include <unicode/utf8.h>
#include <unicode/uiter.h>

#include <iterator>
#include <string>
#include <vector>

namespace px {

    class Utf8String
    {
    public:
        typedef std::vector<uint8_t>::const_iterator bytes_iterator;

        Utf8String() : bytes{}, count{ 0 }
        {
            bytes.reserve(32);
        }

        Utf8String(char c) : Utf8String{}
        {
            bytes.push_back(c);
            pointsStart = { 0 };
            count = 1;
        }

        Utf8String(const char *text) : Utf8String{ std::string{text} }
        {
        }

        Utf8String(const std::string &text)
        {
            std::copy(text.begin(), text.end(), std::back_inserter(bytes));
                    count = countChars();
        }

        const uint8_t *data() const
        {
            return bytes.data();
        }

        const char *c_str() const
        {
            return reinterpret_cast<const char*>(bytes.data());
        }

        std::string toString() const
        {
            return std::string{ c_str(), bytes.size() };
        }

        bytes_iterator bytesBegin() const
        {
            return bytes.cbegin();
        }

        bytes_iterator bytesEnd() const
        {
            return bytes.cend();
        }

        size_t byteLength() const
        {
            return bytes.size();
        }

        size_t length() const
        {
            return count;
        }

        Utf8String& operator+=(int32_t codePoint)
        {
            uint8_t buffer[4] = { 0 };
            int offset = 0;
            UBool isError = 0;
            U8_APPEND(buffer, offset, sizeof(buffer), codePoint, isError);

            pointsStart.push_back(bytes.size());
            std::copy(buffer, buffer + offset, std::back_inserter(bytes));

            ++count;
            return *this;
        }

        int32_t operator[](int32_t index) const
        {
            int32_t codePoint = 0;
            uint32_t offset = pointsStart[index];
            U8_GET(bytes.data(), 0, offset, bytes.size(), codePoint);
            return codePoint;
        }

        bool operator==(const Utf8String& other) const
        {
            return count == other.count && memcmp(bytes.data(), other.bytes.data(), count) == 0;
        }

        bool operator!=(const Utf8String& other) const
        {
            return count != other.count || memcmp(bytes.data(), other.bytes.data(), count) != 0;
        }

        bool operator<(const Utf8String& other) const
        {
            return memcmp(bytes.data(), other.bytes.data(), count) < 0;
        }

        bool operator>(const Utf8String& other) const
        {
            return memcmp(bytes.data(), other.bytes.data(), count) > 0;
        }

        void clear()
        {
            bytes.clear();
            count = 0;
            pointsStart.clear();
        }

        std::size_t hash() const
        {
            std::size_t seed = bytes.size();
            for (auto& i : bytes)
            {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }

    private:

        size_t countChars()
        {
            int32_t pos = 0;
            int32_t codePoint = 0;
            size_t count = 0;
            int32_t bytesSize = bytes.size();
            const uint8_t *data = bytes.data();

            while (pos < bytesSize)
            {
                pointsStart.push_back(pos);

                U8_NEXT(bytes.data(), pos, bytesSize, codePoint);
                count++;

                if (codePoint < 0 && pos != bytesSize)
                    return 0;
            }

            return count;
        }

        std::vector<uint8_t> bytes;
        std::vector<uint32_t> pointsStart;
        size_t count;
    };

    class Utf8Iterator
    {
    public:
        Utf8Iterator(const Utf8String &str)
        {
            const char *stringBytes = reinterpret_cast<const char*>(str.data());
            uiter_setUTF8(&charIterator, stringBytes, str.byteLength());
            currentCodePoint = uiter_current32(&charIterator);
        }

        Utf8Iterator& operator=(const Utf8Iterator& rhs)
        {
            charIterator = rhs.charIterator;
            currentCodePoint = rhs.currentCodePoint;
            return *this;
        }

        bool operator !()
        {
            return !charIterator.hasNext(&charIterator);
        }

        int32_t operator *() const
        {
            return currentCodePoint;
        }

        Utf8Iterator& operator ++()
        {
            uiter_next32(&charIterator);

            currentCodePoint = uiter_current32(&charIterator);

            return *this;
        }

        Utf8Iterator operator ++(int)
        {
            Utf8Iterator temp = *this;
            ++(*this);
            return temp;
        }

        Utf8Iterator& operator --()
        {
            uiter_previous32(&charIterator);

            currentCodePoint = uiter_current32(&charIterator);

            return *this;
        }

        Utf8Iterator operator --(int)
        {
            Utf8Iterator temp = *this;
            --(*this);
            return temp;
        }

    private:
        int32_t currentCodePoint;
        UCharIterator charIterator;
    };
}

namespace std
{
    template<> struct hash<px::Utf8String>
    {
        typedef px::Utf8String argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const
        {
            return s.hash();
        }
    };
}

#endif