
#include <Symbol.h>

namespace px
{
    Type * const Type::OBJECT{ new Type{"object", nullptr, 4, Type::BUILTIN} };
    Type * const Type::VOID{ new Type{"void", nullptr, 0, Type::BUILTIN_VOID | Type::SEALED} };
    Type * const Type::BOOL{ new Type{"bool", nullptr, 1, Type::BUILTIN_BOOL | Type::SEALED} };
    Type * const Type::INT8{ new Type{"int8", Type::OBJECT, 1, Type::BUILTIN_INT | Type::SEALED} };
    Type * const Type::INT16{ new Type{"int16", Type::OBJECT, 2, Type::BUILTIN_INT | Type::SEALED} };
    Type * const Type::INT32{ new Type{"int32", Type::OBJECT, 4, Type::BUILTIN_INT | Type::SEALED} };
    Type * const Type::INT64{ new Type{"int64", Type::OBJECT, 8, Type::BUILTIN_INT | Type::SEALED} };
    Type * const Type::FLOAT32{ new Type{"float32", Type::OBJECT, 4, Type::BUILTIN_FLOAT | Type::SEALED} };
    Type * const Type::FLOAT64{ new Type{"float64", Type::OBJECT, 8, Type::BUILTIN_FLOAT | Type::SEALED} };
    Type * const Type::STRING{ new Type{"string", Type::OBJECT, 4, Type::BUILTIN_STRING | Type::SEALED} };
}
