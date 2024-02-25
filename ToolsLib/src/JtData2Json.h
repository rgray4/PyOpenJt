#pragma once
/**
 * \file JtData2Json.h
 * \date 20.2.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds the interface to tools createing JSON from JtData_... classes
 */

#include <JtData_Model.hxx>

#include <ostream>

std::ostream& operator<<(std::ostream& os, const TCollection_ExtendedString& extstr);

struct indentOp {
	indentOp(int level) : level(level) {}
private:
	friend std::ostream& operator<<(std::ostream& stream, const indentOp& val);
	int level;
};

inline std::ostream& operator<<(std::ostream& stream, const indentOp& val) {
	for (int i = 0; i < val.level; i++)
		stream << '\t';
	return stream;
}


int writeModel(Handle(JtData_Model) model, std::ostream& out, int indent = 0, int config = 0);