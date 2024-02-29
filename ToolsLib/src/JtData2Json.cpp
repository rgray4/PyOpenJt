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

int writeKeyValueStream(const std::vector<char>& stream, std::ostream& out, int indent /* = 0 */, int config /* = 0 */)
{
	out << indentOp(indent) << "{\n";

	bool tick = true;

	out << indentOp(indent) << "\t\"";
	for (auto it = stream.begin(); it != stream.end(); ++it) {
		if (*it == '\0') {
			if (tick) {
				out << "\":\"";
				tick = false;
			}
			else {
				out << "\"";
				if ((it + 1) == stream.end())
					out << "\n";
				else
					out << ",\n" << indentOp(indent) << "\t\"";
				tick = true;
			}
		}
		else {
			if (*it == '\n')
				out << "\\n";
			else if(*it == '\"')
				out << "\\\"";
			else
				out << *it;
		}

	}

	out << indentOp(indent) << "}\n";

	return 0;
}
