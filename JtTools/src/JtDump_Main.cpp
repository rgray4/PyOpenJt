/**
 * \file JtDump_Main.cpp
 * \date 20.2.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds the executable definition to dump Jt information as JSON
 */

#include <CLI/CLI.hpp>

#include <JtData_Model.hxx>
#include <JtNode_Partition.hxx>
#include <JtNode_Base.hxx>

#include <iostream>
#include <iomanip>

using namespace std;


int main(int argc, char* argv[])
{
	CLI::App app("JtDump - dumps information of a Jt file as JSON");

	// add version output
	app.set_version_flag("--version", std::string("0.8"));

	std::vector<std::string> files;
	app.add_option("jt-files", files, "Jt file(s) to dump")->check(CLI::ExistingFile);

	CLI11_PARSE(app, argc, argv);

	if (files.size() == 0) {

		cerr << "No input Jt files given!" << endl;
		return 1;
	}

	// go through Jt files
	for (const auto& aarg : files) {
		cout << aarg << " ";

		//ifstream aFile(aarg, std::ios::binary | std::ios::in);

#ifdef _WIN32
		TCollection_ExtendedString aFileName(aarg.c_str(), Standard_True);
#else
		TCollection_ExtendedString aFileName(theFileName.toUtf8().data());
#endif

		Handle(JtData_Model) aModel = new JtData_Model(aFileName);

		if (aModel.IsNull())
			return 2;

		Handle(JtNode_Partition) PartitionNode = aModel->Init();

		if(!PartitionNode->Load())
			return 3;

		auto childrean = PartitionNode->Children();
		for (Standard_Integer aChildIdx = 0; aChildIdx < childrean.Count(); ++aChildIdx)
		{
			Handle(JtNode_Base) aChildRecord = Handle(JtNode_Base)::DownCast(childrean[aChildIdx]);

			if (aChildRecord.IsNull())
			{
				continue;
			}

		}
	}

	return 0;
}