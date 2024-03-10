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

std::ostream& operator<<(std::ostream& os, const Jt_GUID& extstr)
{
	char str[128];
	extstr.ToString(str);
	return (os << str);
}


int writeModel(Handle(JtData_Model) model, std::ostream& out, int indent /* = 0 */ , int config /* = 0 */)
{
	out << indentOp(indent) << "\"FileName\":\"" << model->FileName() << "\",\n";
	out << indentOp(indent) << "\"MajorVersion\":\"" << model->MajorVersion() << "\",\n";
	out << indentOp(indent) << "\"MinorVersion\":\"" << model->MinorVersion() << "\",\n";
	if (config & dump_TOC) {
		auto toc = model->getTOC();

		if (toc.size() > 0) {
			out << indentOp(indent) << "\"TocTable\": [";
			indent++;
			bool first = true;
			for (auto it : toc){
				if (first)
					first = false;
				else
					out << ',';
				out << "[\"" << it.GUID << "\"," << it.Type << ']';
					
			}
			indent--;
			out << "],\n";
		}
	}
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
				out << "\\\\n";
			else if (*it == '\"')
				out << "\\\\\"";
			else if (*it == '\r')
				out << "\\\\r";
			else if (*it == '\b')
				out << "\\\\b";
			else if (*it == '\f')
				out << "\\\\f";
			else if (*it == '\t')
				out << "\\\\t";
			else if (*it == '\\')
				out << "\\\\";
			else
				out << *it;
		}

	}

	out << indentOp(indent) << "}\n";

	return 0;
}
