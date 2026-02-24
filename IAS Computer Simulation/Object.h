#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <string>

namespace osl
{
	class Object 
	{
		public:
		virtual ~Object() {}
		virtual std::string toString() const = 0;
		
		friend std::ostream& operator<<(std::ostream& out,const Object& obj)
		{
			out << obj.toString();
			return out;
		}
	};
}
#endif
