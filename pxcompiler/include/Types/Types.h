
/*
 * Expression.h
 *
 *  Created on: Mar 5, 2010
 *      Author: steve
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <map>

namespace px
{
	class Type
	{
	public:
		enum TypeFlags
		{
			NONE = 0,
			BUILTIN = 0x08,
			BUILTIN_INT = 0x09,
			BUILTIN_FLOAT = 0x0a,
			ABSTRACT = 0x10,
			SEALED = 0x20
		};

		std::string name;
		Type *parent;
		unsigned int size;
		unsigned int flags;

		Type(std::string name, Type *parent, unsigned int size, unsigned int flags)
		{
			this->name = name;
			this->parent = parent;
			this->size = size;
			this->flags = flags;
		}

		bool inherits(Type *t)
		{
			return t != nullptr && parent != nullptr && (t == parent || parent->inherits(t));
		}

		bool is_a(Type *t)
		{
			return this == t || inherits(t);
		}

		bool isBuiltinFloat()
		{
			return (flags & BUILTIN_FLOAT) == BUILTIN_FLOAT;
		}

		bool isBuiltinInt()
		{
			return (flags & BUILTIN_INT) == BUILTIN_INT;
		}
	};

	class TypeTable
	{
	protected:
		std::map<std::string, Type*> types;
	public:
		TypeTable()
		{
			Type *object = new Type("object", nullptr, 4, Type::BUILTIN);
			addType(object);

			addType("int8", object, 1, Type::BUILTIN_INT | Type::SEALED);
			addType("int16", object, 2, Type::BUILTIN_INT | Type::SEALED);
			addType("int32", object, 4, Type::BUILTIN_INT | Type::SEALED);
			addType("int64", object, 8, Type::BUILTIN_INT | Type::SEALED);
			addType("float32", object, 4, Type::BUILTIN_FLOAT | Type::SEALED);
			addType("float64", object, 8, Type::BUILTIN_FLOAT | Type::SEALED);
		}

		void addType(Type *type)
		{
			types[type->name] = new Type(type->name, type->parent, type->size, type->flags);
		}

		void addType(std::string name, Type *parent, unsigned int size, unsigned int flags)
		{
			types[name] = new Type(name, parent, size, flags);
		}

		Type* getType(std::string name)
		{
			if(types.find(name) != types.end())
				return types[name];
			else
				return nullptr;
		}
	};
}

#endif
