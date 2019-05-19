
#include <Symbol.h>

namespace px
{
    Type * const Type::UNKNOWN{ new Type{ std::string{"<<unknown>>"}, nullptr, 0, Type::BUILTIN } };
    Type * const Type::OBJECT{ new Type{ std::string{"object"}, nullptr, 4, Type::BUILTIN} };
    Type * const Type::VOID{ new Type{ std::string{"void"}, nullptr, 0, Type::BUILTIN_VOID | Type::SEALED} };
    Type * const Type::BOOL{ new Type{ std::string{"bool"}, Type::OBJECT, 1, Type::BUILTIN_BOOL | Type::SEALED} };
    Type * const Type::INT8{ new Type{ std::string{"int8"}, Type::OBJECT, 1, Type::BUILTIN_INT | Type::SEALED} };
    Type * const Type::INT16{ new Type{ std::string{"int16"}, Type::OBJECT, 2, Type::BUILTIN_INT | Type::SEALED} };
    Type * const Type::INT32{ new Type{ std::string{"int32"}, Type::OBJECT, 4, Type::BUILTIN_INT | Type::SEALED} };
    Type * const Type::INT64{ new Type { std::string{"int64"}, Type::OBJECT, 8, Type::BUILTIN_INT | Type::SEALED} };
    Type * const Type::UINT8{ new Type{ std::string{"uint8"}, Type::OBJECT, 1, Type::BUILTIN_UINT | Type::SEALED } };
    Type * const Type::UINT16{ new Type{ std::string{"uint16"}, Type::OBJECT, 2, Type::BUILTIN_UINT | Type::SEALED } };
    Type * const Type::UINT32{ new Type{ std::string{"uint32"}, Type::OBJECT, 4, Type::BUILTIN_UINT | Type::SEALED } };
    Type * const Type::UINT64{ new Type{ std::string{"uint64"}, Type::OBJECT, 8, Type::BUILTIN_UINT | Type::SEALED } };
    Type * const Type::FLOAT32{ new Type{ std::string{"float32"}, Type::OBJECT, 4, Type::BUILTIN_FLOAT | Type::SEALED} };
    Type * const Type::FLOAT64{ new Type{ std::string{"float64"}, Type::OBJECT, 8, Type::BUILTIN_FLOAT | Type::SEALED} };
    Type * const Type::CHAR{ new Type{ std::string{"char"}, Type::OBJECT, 4, Type::BUILTIN_CHAR | Type::SEALED } };
    Type * const Type::STRING{ new Type{ std::string{"string"}, Type::OBJECT, 4, Type::BUILTIN_STRING | Type::SEALED} };
}
