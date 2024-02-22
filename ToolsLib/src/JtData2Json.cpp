#include "JtData2Json.h"


std::ostream& operator<<(std::ostream& os, const TCollection_ExtendedString& extstr)
{
	char* str = new char[extstr.LengthOfCString() + 1];
	extstr.ToUTF8CString(str);
	return (os << str);
}



int writeModel(Handle(JtData_Model) model, std::ostream& out, int config /* = 0 */)
{
	out << '{';
	out << "\"FileName\":\"" << model->FileName() << '\"';
	out << '}';


	return 0;
}