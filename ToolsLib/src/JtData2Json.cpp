#include "JtData2Json.h"

#include <assert.h>

std::ostream& operator<<(std::ostream& os, const TCollection_ExtendedString& extstr)
{
	static char str[2048];
	assert(extstr.LengthOfCString() < 2048);
	Standard_PCharacter mchastr = str;
	extstr.ToUTF8CString(mchastr);
	return (os << str);
}


int writeModel(Handle(JtData_Model) model, std::ostream& out, int indent /* = 0 */ , int config /* = 0 */)
{
	out << indentOp(indent) << "{\n";
	out << indentOp(indent) << "\t\"FileName\":\"" << model->FileName() << "\"\n";
	out << indentOp(indent) << "\t\"MajorVersion\":\"" << model->MajorVersion() << "\"\n";
	out << indentOp(indent) << "\t\"MinorVersion\":\"" << model->MinorVersion() << "\"\n";
	out << indentOp(indent) << "}\n";

	return 0;
}