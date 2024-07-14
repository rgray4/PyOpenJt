#include "JtData2Json.h"

#include <assert.h>

const char* SegmentTypeString(int typeNum) {

	switch (typeNum) {
	case 1: return "Logical Scene Graph";
	case 2: return "JT B-Rep";
	case 3: return "PMI Data";
	case 4: return "Meta Data";
	case 6: return "Shape";
	case 7: return "Shape LOD0";
	case 8: return "Shape LOD1";
	case 9: return "Shape LOD2";
	case 10: return "Shape LOD3";
	case 11: return "Shape LOD4";
	case 12: return "Shape LOD5";
	case 13: return "Shape LOD6";
	case 14: return "Shape LOD7";
	case 15: return "Shape LOD8";
	case 16: return "Shape LOD9";
	case 17: return "XT B-Rep";
	case 18: return "Wireframe Representation";
	case 20: return "ULP";
	case 23: return "STT";
	case 24: return "LWPA";
	case 30: return "MultiXT B-Rep";
	case 31: return "InfoSegment";
	case 32: return "AEC Shape";
	case 33: return "STEP B-rep";
	default: return "Unknown";
	}
}


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
			out << indentOp(indent) << "\"TocTable\": [\n";
			indent++;
			bool first = true;
			for (auto it : toc){
				out << indentOp(indent);
				if (first)
					first = false;
				else
					out << ',';
				out << "[\"" << it.GUID << "\"," << it.Type << ",\"" << SegmentTypeString(it.Type) << "\"]\n";
					
			}
			indent--;
			out << indentOp(indent) << "\n],\n";
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
