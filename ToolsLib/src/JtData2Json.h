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



int writeModel(Handle(JtData_Model) model, std::ostream& out, int config = 0);