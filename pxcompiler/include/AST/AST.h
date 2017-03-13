
#ifndef AST_H_
#define AST_H_

#include <string>

namespace px
{
	namespace ast
	{
		class Visitor;

		class AST
		{
		public:
			AST() = default;
			virtual ~AST() = default;
			virtual void *accept(Visitor &visitor) = 0;
		};
	}
}

#endif
