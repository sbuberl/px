
#ifndef _PX_UTF8STRING_H_
#define _PX_UTF8STRING_H_

#include <unicode/utf8.h>
#include <unicode/uiter.h>

#include <cstring>
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

        Utf8String(const Utf8String &other) : bytes{ other.bytes }, pointsStart{ other.pointsStart }, count{other.count}
        {
        }

        Utf8String(Utf8String &&other) : bytes{ std::move(other.bytes) }, pointsStart{ std::move(other.pointsStart) }, count{ other.count }
        {
            other.count = 0;
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

        size_t size() const
        {
            return count;
        }

        size_t length() const
        {
            return count;
        }

        bool startsWith(const Utf8String &other) const
        {
            if( count >= other.count) {
                return std::equal(bytes.begin(), bytes.end(), bytes.begin());
            }
            return false;
        }

        bool endsWith(const Utf8String &other) const
        {
            if( count >= other.count) {
                return std::equal(bytes.rbegin(), bytes.rend(), bytes.rbegin());
            }
            return false;
        }

        Utf8String &operator=(const Utf8String &other)
        {
            count = other.count;
            bytes = other.bytes;
            pointsStart = other.pointsStart;
            return *this;
        }

        Utf8String operator+(const Utf8String &other) const
        {
            Utf8String copy{ *this };
            size_t length = bytes.size();
            copy.count += other.count;
            copy.bytes.insert(std::end(copy.bytes), std::begin(other.bytes), std::end(other.bytes));
            for (const uint32_t &start : other.pointsStart)
                copy.pointsStart.push_back(length + start);
            return copy;
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

        Utf8String operator+=(const Utf8String &other)
        {
            size_t length = bytes.size();
            count += other.count;
            bytes.insert(std::end(bytes), std::begin(other.bytes), std::end(other.bytes));
            for (const uint32_t &start : other.pointsStart)
                pointsStart.push_back(length + start);
            return *this;
        }

        int32_t operator[](uint32_t index) const
        {
            int32_t codePoint = 0;
            uint32_t offset = pointsStart[index];
            U8_GET(bytes.data(), 0, offset, bytes.size(), codePoint);
            return codePoint;
        }

        bool operator==(const Utf8String& other) const
        {
            return count == other.count && std::memcmp(bytes.data(), other.bytes.data(), count) == 0;
        }

        bool operator!=(const Utf8String& other) const
        {
            return count != other.count || std::memcmp(bytes.data(), other.bytes.data(), count) != 0;
        }

        bool operator<(const Utf8String& other) const
        {
            return std::memcmp(bytes.data(), other.bytes.data(), count) < 0;
        }

        bool operator>(const Utf8String& other) const
        {
            return std::memcmp(bytes.data(), other.bytes.data(), count) > 0;
        }

        void clear()
        {
            bytes.clear();
            count = 0;
            pointsStart.clear();
        }

        friend void swap(Utf8String& a, Utf8String& b)
        {
            std::swap(a.count, b.count);
            std::swap(a.bytes, b.bytes);
            std::swap(a.pointsStart, b.pointsStart);
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

                U8_NEXT(data, pos, bytesSize, codePoint);
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
            length = str.length();
            offset = 0;
            const char *stringBytes = reinterpret_cast<const char*>(str.data());
            uiter_setUTF8(&charIterator, stringBytes, str.byteLength());
            currentCodePoint = uiter_current32(&charIterator);
        }

        Utf8Iterator& operator=(const Utf8Iterator& rhs)
        {
            charIterator = rhs.charIterator;
            currentCodePoint = rhs.currentCodePoint;
            length = rhs.length;
            offset = rhs.offset;
            return *this;
        }

        bool hasPrevious()
        {
            return offset != 0;
        }

        bool hasNext()
        {
            return offset < (length - 1);
        }

        int32_t operator *() const
        {
            return currentCodePoint;
        }

        Utf8Iterator& operator ++()
        {
            uiter_next32(&charIterator);

            currentCodePoint = uiter_current32(&charIterator);

            ++offset;

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

            --offset;

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
        size_t length;
        size_t offset;
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
