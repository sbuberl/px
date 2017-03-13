
#ifndef SOURCEPOSITION_H_
#define SOURCEPOSITION_H_

namespace px {

	struct SourcePosition
	{
		size_t pos;
		size_t line;

		SourcePosition() : pos(0), line(1)
		{

		}
	};

}

#endif
