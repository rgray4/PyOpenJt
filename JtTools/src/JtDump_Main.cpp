/**
 * \file JtDump_Main.cpp
 * \date 20.2.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds the executable definition to dump Jt information as JSON
 */

#include <CLI/CLI.hpp>

#include <JtData_Model.hxx>
#include <JtNode_Base.hxx>
#include <JtNode_Partition.hxx>
#include <JtNode_Part.hxx>
#include <JtNode_RangeLOD.hxx>
#include <JtNode_LOD.hxx>
#include <JtNode_Instance.hxx>
#include <JtNode_Shape_Vertex.hxx>
#include <JtNode_Shape_TriStripSet.hxx>
#include <JtNode_MetaData.hxx>
#include <JtNode_Shape_Base.hxx>
#include <JtNode_Shape_TriStripSet.hxx>

#include <JtProperty_LateLoaded.hxx>

#include <JtAttribute_Material.hxx>
#include <JtAttribute_GeometricTransform.hxx>

#include <JtElement_ProxyMetaData.hxx>
#include <JtElement_ShapeLOD_Vertex.hxx>

#include <JtData2Json.h>
#include <JtLayer.h>
#include <JtUsdMesh.h>

#include <iostream>
#include <iomanip>
#include <assert.h>
#include <regex>

//#include <Matrix.h>

using namespace std;

std::ostream* outStream = &(std::cout);
int indention = 0;
int outFilter = dumpConfig::dump_all;
int meshData = 1;

void WriteMeshesWithoutLSG(Handle(JtData_Model) rootModel, const std::string& fileName);
void HandleAllChildren(const Handle(JtNode_Group)& theGroupRecord, const std::string& thePrefix);
void RecurseDownTheTree(const Handle(JtNode_Base)& theNodeRecord, const std::string& thePrefix);
void HandleAttributes(const Handle(JtNode_Base)& theNodeRecord);
const std::vector<char>& HandleLateLoadsMeta(const JtData_Object::VectorOfLateLoads& aLateLoaded);

int main(int argc, char* argv[])
{
	CLI::App app("JtDump - dumps information of a Jt file as JSON");

	// add version output
	app.set_version_flag("--version", std::string("0.8"));

	std::vector<std::string> files;
	app.add_option("jt-files", files, "Jt file(s) to dump")->check(CLI::ExistingFile);
    std::string outFileName;
    app.add_option("-o", outFileName, "Out File Name");
    app.add_option("-m", meshData, "Mesh Data mode (0=none)");

	CLI11_PARSE(app, argc, argv);

	if (files.size() == 0) {

		cerr << "No input Jt files given!" << endl;
		return 1;
	}

    std::ofstream outFile(outFileName.c_str());
    if (outFile.is_open()) {
        outStream = &outFile;
    }

    if (files.size() > 1) {
        *outStream << '[' << endl;
        indention++;
    }

    bool first = true;

	// go through Jt files
	for (const auto& aarg : files) {

        string fileName = regex_replace(aarg, std::regex("\\\\"), "/"); // replace 'def' -> 'klm'

        if (files.size() > 1) {
            if (first)
                first = false;
            else
                *outStream << ',';
        }

        *outStream << '{' << endl;
        indention++;


		TCollection_ExtendedString aFileName(fileName.c_str(), Standard_True);


		Handle(JtData_Model) rootModel =   new JtData_Model(aFileName);


		if (rootModel.IsNull())
			return 2;

		Handle(JtNode_Partition) PartitionNode = rootModel->Init();

    
        writeModel(rootModel, *outStream, indention, outFilter);

        if (PartitionNode.IsNull()) {
            WriteMeshesWithoutLSG(rootModel, fileName);
        }
        else {
            RecurseDownTheTree(PartitionNode, fileName);
        }

        indention--;
        *outStream << '}' << endl;
		
	}

    if (files.size() > 1) {
        indention--;
        *outStream << ']' << endl;
    }

    assert(indention == 0);

	return 0;
}


static Handle(Standard_Type) TypeOf_JtNode_Partition				= STANDARD_TYPE(JtNode_Partition);
static Handle(Standard_Type) TypeOf_JtNode_Part						= STANDARD_TYPE(JtNode_Part);
static Handle(Standard_Type) TypeOf_JtNode_RangeLOD					= STANDARD_TYPE(JtNode_RangeLOD);
static Handle(Standard_Type) TypeOf_JtNode_LOD						= STANDARD_TYPE(JtNode_LOD);
static Handle(Standard_Type) TypeOf_JtNode_Group					= STANDARD_TYPE(JtNode_Group);
static Handle(Standard_Type) TypeOf_JtNode_Instance					= STANDARD_TYPE(JtNode_Instance);
static Handle(Standard_Type) TypeOf_JtNode_Shape_Vertex				= STANDARD_TYPE(JtNode_Shape_Vertex);
static Handle(Standard_Type) TypeOf_JtNode_MetaData                 = STANDARD_TYPE(JtNode_MetaData);
static Handle(Standard_Type) TypeOf_JtNode_Shape_TriStripSet        = STANDARD_TYPE(JtNode_Shape_TriStripSet);
static Handle(Standard_Type) TypeOf_JtAttribute_Material			= STANDARD_TYPE(JtAttribute_Material);
static Handle(Standard_Type) TypeOf_JtAttribute_GeometricTransform	= STANDARD_TYPE(JtAttribute_GeometricTransform);
static Handle(Standard_Type) TypeOf_JtElement_ProxyMetaData         = STANDARD_TYPE(JtElement_ProxyMetaData);
static Handle(Standard_Type) TypeOf_JtElement_ShapeLOD_Vertex       = STANDARD_TYPE(JtElement_ShapeLOD_Vertex);

void WriteMeshesWithoutLSG(Handle(JtData_Model) rootModel, const std::string& fileName)
{
    auto toc = rootModel->getTOC();

    int indent = 1;

    if (toc.size() > 0) {
        *outStream << indentOp(indent) << "\"MeshTable\": [\n";
        indent++;
        bool first = true;
        for (auto it : toc) {
            if (it.Type == 7) {
                *outStream << indentOp(indent);
                if (first)
                    first = false;
                else
                    *outStream << ',';

                char guidBuf[100]; it.GUID.ToString(guidBuf);
                *outStream << "[\"" << guidBuf << "\"," << it.Type << "]\n";
            }
        }
        indent--;
        *outStream << indentOp(indent) << "\n]\n";
    }

}

void RecurseDownTheTree(const Handle(JtNode_Base)& theNodeRecord, const std::string& thePrefix)
{

    // Handle all different types of LSG Nodes
    if (theNodeRecord->IsKind(TypeOf_JtNode_Partition))
    {
        Handle(JtNode_Partition) aPartitionRecord = Handle(JtNode_Partition)::DownCast(theNodeRecord);

        *outStream << indentOp(indention) << "\"LSG\":\n";
        *outStream << indentOp(indention) << "{\n";
        indention++; 
        *outStream << indentOp(indention) << "\"jt_type\":\"Partition\",\n";
        *outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        HandleAttributes(theNodeRecord);
        HandleAllChildren( aPartitionRecord, thePrefix);

        indention--;
        *outStream << indentOp(indention) << "}\n";
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Part))
    {
        // cast to actuall type
        Handle(JtNode_Part) aPartRecord = Handle(JtNode_Part)::DownCast(theNodeRecord);
        const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();

        *outStream << indentOp(indention) << "{\n";
        indention++;
        *outStream << indentOp(indention) << "\"jt_type\":\"Part\",\n";
        *outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        // handle late loaded properties
        auto metaData = HandleLateLoadsMeta(aLateLoaded);
        if (metaData.size() > 0) {
            *outStream << indentOp(indention) << "\"properties\":\n";
            writeKeyValueStream(metaData, *outStream, indention);

            // handling layer in Part
            std::vector<uint32_t> layer = ScanForLayer(metaData);
            *outStream << indentOp(indention) << ",\"layer\":[";
            bool first = true;
            for (const auto& layerNum : layer) {
                if (first)
                    first = false;
                else
                    *outStream << ',';
                *outStream << layerNum;
            }
            *outStream << "],\n";
        }

        HandleAttributes(theNodeRecord);
        HandleAllChildren(Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);

        indention--;
        *outStream << indentOp(indention) << "}\n";

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_MetaData))
    {
        Handle(JtNode_MetaData) aPartRecord = Handle(JtNode_MetaData)::DownCast(theNodeRecord);
        const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();

        *outStream << indentOp(indention) << "{\n";
        indention++;
        *outStream << indentOp(indention) << "\"jt_type\":\"MetaData\",\n";
        *outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        // handle late loaded properties
        auto metaData = HandleLateLoadsMeta(aLateLoaded);
        if (metaData.size() > 0) {
            *outStream << indentOp(indention) << "\"properties\":\n";
            writeKeyValueStream(metaData, *outStream, indention);

            // handling layer in top meta data
            LayerInfo layerInfo = ScanForLayerFilter(metaData);
            *outStream << indentOp(indention) << ",\"layer-filter\":[";
            bool first = true;
            for (const auto& layerFilter : layerInfo.LayerMap) {
                if (first)
                    first = false;
                else
                    *outStream << ',';
                *outStream << "{ \"name\":\"" << layerFilter.first << "\", \"layer\":[";
                bool first2 = true;
                for (const auto layerNum : layerFilter.second) {
                    if (first2)
                        first2 = false;
                    else
                        *outStream << ',';
                    *outStream << layerNum;
                }
                *outStream << "]}";
            }
            *outStream << "],\n";
            *outStream << indentOp(indention) << "\"active-layer-filter\":\"" << layerInfo.ActiveLayerFilter << "\",\n";
        }

        HandleAttributes(theNodeRecord);
        HandleAllChildren(Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);

        indention--;
        *outStream << indentOp(indention) << "}\n";

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_RangeLOD))
    {
        Handle(JtNode_RangeLOD) aRangeLODRecord =
            Handle(JtNode_RangeLOD)::DownCast(theNodeRecord);

        *outStream << indentOp(indention) << "{\n";
        indention++;
        *outStream << indentOp(indention) << "\"jt_type\":\"RangeLOD\",\n";
        *outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        *outStream << indentOp(indention) << "\"center\":["
            << (aRangeLODRecord->Center().X) << ','
            << (aRangeLODRecord->Center().Y) << ','
            << (aRangeLODRecord->Center().Z) << "],\n";

        *outStream << indentOp(indention) << "\"limits\":[";
        bool first = true;

        for (Standard_Integer anIdx = 1; anIdx <= aRangeLODRecord->RangeLimits().Count(); ++anIdx){
            if (first)
                first = false;
            else
                *outStream << ',';
            *outStream << aRangeLODRecord->RangeLimits()[anIdx];
        }
        *outStream << "],\n";

        HandleAttributes(theNodeRecord);
        HandleAllChildren(aRangeLODRecord, thePrefix);

        indention--;
        *outStream << indentOp(indention) << "}\n";

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_LOD))
    {
        *outStream << indentOp(indention) << "{\n";
        indention++;
        *outStream << indentOp(indention) << "\"jt_type\":\"LOD\",\n";
        *outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        HandleAttributes(theNodeRecord);
        HandleAllChildren(Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);

        indention--;
        *outStream << indentOp(indention) << "}\n";

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Group))
    {
        *outStream << indentOp(indention) << "{\n";
        indention++;
        *outStream << indentOp(indention) << "\"jt_type\":\"Group\",\n";
        *outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        HandleAttributes(theNodeRecord);
        HandleAllChildren(Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);

        indention--;
        *outStream << indentOp(indention) << "}\n";
        }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Instance))
    {
        Handle(JtNode_Instance) anInstance = Handle(JtNode_Instance)::DownCast(theNodeRecord);

        *outStream << indentOp(indention) << "{\n";
        indention++;
        *outStream << indentOp(indention) << "\"jt_type\":\"Instance\",\n";
        *outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

    
        HandleAttributes(theNodeRecord);

        Handle(JtNode_Base) aNode = Handle(JtNode_Base)::DownCast(anInstance->Object());
        *outStream << indentOp(indention) << "\"InstancedObject\":\n";
        RecurseDownTheTree(aNode, thePrefix);

        indention--;
        *outStream << indentOp(indention) << "}\n";
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Shape_Vertex))
    {
        Handle(JtNode_Shape_Vertex) aShapeRecord = Handle(JtNode_Shape_Vertex)::DownCast(theNodeRecord);

        if (aShapeRecord->IsKind(TypeOf_JtNode_Shape_TriStripSet))
        {
            Handle(JtNode_Shape_TriStripSet) aMeshRecord = Handle(JtNode_Shape_TriStripSet)::DownCast(aShapeRecord);

            *outStream << indentOp(indention) << "{\n";
            indention++;
            *outStream << indentOp(indention) << "\"jt_type\":\"TriStripSet\",\n";
            *outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";


            aMeshRecord->Bounds();
            const JtData_Object::VectorOfLateLoads& aLateLoaded = aMeshRecord->LateLoads();


            for (int i = 0; i < aLateLoaded.Count(); i++) {
                //cerr << "TriStripSet Late load SegType: " << aLateLoaded[i]->getSegmentType() << '\n';
                if (aLateLoaded[i]->DefferedObject().IsNull())
                    aLateLoaded[i]->Load();

                Handle(JtData_Object) prop = aLateLoaded[i]->DefferedObject();
                if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_ShapeLOD_Vertex)) {
                    Handle(JtElement_ShapeLOD_Vertex) aProxyMetaDataElement = Handle(JtElement_ShapeLOD_Vertex)::DownCast(prop);

                    //writeUsdMesh(aProxyMetaDataElement, "TestMesh");

                    auto indices = aProxyMetaDataElement->Indices();
                    auto vertices = aProxyMetaDataElement->Vertices();
                    auto normals = aProxyMetaDataElement->Normals();

                    *outStream << indentOp(indention) << "\"NumIndices\":" << indices.Count() << ",\n";
                    *outStream << indentOp(indention) << "\"NumVertices\":" << vertices.Count() << ",\n";
                    *outStream << indentOp(indention) << "\"NumNormals\":" << normals.Count() << ",\n";

                    bool first = true;
                    *outStream << indentOp(indention) << "\"Indices\":[";
                    if (meshData != 0) {
                        first = true;
                        for (int l = 0; l < indices.Count(); l++) {
                            if (first)
                                first = false;
                            else
                                *outStream << ',';
                            *outStream << indices.Data()[l];

                        }
                    }
                    *outStream << "],\n";

                    *outStream << indentOp(indention) << "\"Vertices\":[";
                    if (meshData != 0) {
                        first = true;
                        for (int l = 0; l < vertices.Count() * 3; l++) {
                            if (first)
                                first = false;
                            else
                                *outStream << ',';
                            *outStream << vertices.Data()[l];

                        }
                    }
                    *outStream << "],\n";

                    


                    *outStream << indentOp(indention) << "\"Normals\":[";
                    if (meshData != 0) {
                        first = true;
                        for (int l = 0; l < normals.Count() * 3; l++) {
                            if (first)
                                first = false;
                            else
                                *outStream << ',';
                            *outStream << normals.Data()[l];

                        }
                    }
                    *outStream << "]\n";
                }

            }

            indention--;
            *outStream << indentOp(indention) << "}\n";

            /*
            JTCommon_AABB aBox(
                Eigen::Vector4f(aMeshRecord->Bounds().MinCorner.X,
                    aMeshRecord->Bounds().MinCorner.Y,
                    aMeshRecord->Bounds().MinCorner.Z,
                    1.f),
                Eigen::Vector4f(aMeshRecord->Bounds().MaxCorner.X,
                    aMeshRecord->Bounds().MaxCorner.Y,
                    aMeshRecord->Bounds().MaxCorner.Z,
                    1.f));

            if (!mySources.Contains(aMeshRecord))
            {
                JTData_MeshNodeSourcePtr aMeshSource(
                    new JTData_MeshNodeSource(myLoadingQueue, aMeshRecord));

                // Add to new mesh source to the map (can be reused)
                mySources.Add(aMeshRecord, aMeshSource);
            }
            */

        }
        else
            cerr <<  "!!! '" << theNodeRecord->Name() << "' " << "TypeOf_JtNode_Shape_Vertex->Unknown \n";
    }
    else
    {
        cerr  << "!!! '" << theNodeRecord->Name() << "' " << "Unknown node type \n";
    }

}

const std::vector<char>& HandleLateLoadsMeta(const JtData_Object::VectorOfLateLoads& aLateLoaded)
{
    static std::vector<char> EmptyVector;

    for (int i = 0; i < aLateLoaded.Count(); i++) {
        if (aLateLoaded[i]->getSegmentType() == SegmentType::Meta_Data) {
            if (aLateLoaded[i]->DefferedObject().IsNull())
                aLateLoaded[i]->Load();

            Handle(JtData_Object)  prop = aLateLoaded[i]->DefferedObject();
            if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_ProxyMetaData)) {
                Handle(JtElement_ProxyMetaData) aProxyMetaDataElement =
                    Handle(JtElement_ProxyMetaData)::DownCast(prop);

                return aProxyMetaDataElement->getKeyValueStream();

                //writeKeyValueStream(stream, outStream, indention);

               // aLateLoaded[i]->Unload();

            }
        }
        else {
            int type = aLateLoaded[i]->getSegmentType();
            if(type!=17 && type != 18)
                cerr << "Unknown Late load: " << aLateLoaded[i]->getSegmentType() << '\n';
        }
    }

    return EmptyVector;
}

void HandleAttributes(const Handle(JtNode_Base)& theNodeRecord)
{
    // Extract attributes

    if (theNodeRecord->Attributes().IsEmpty())
        return;

    for (Standard_Integer anIdx = 0; anIdx < (Standard_Integer)theNodeRecord->Attributes().Count(); ++anIdx)
    {
        const Handle(JtData_Object)& anObject = theNodeRecord->Attributes()[anIdx];

        if (anObject.IsNull())
            continue;

        Handle(JtAttribute_Base) anAttrib = Handle(JtAttribute_Base)::DownCast(anObject);

        if (anAttrib.IsNull())
            cerr << "Error! Invalid node attribute\n";


        if (anAttrib->IsKind(TypeOf_JtAttribute_GeometricTransform))
        {
            Handle(JtAttribute_GeometricTransform) aTransform =
                Handle(JtAttribute_GeometricTransform)::DownCast(anAttrib);

            *outStream << indentOp(indention) << "\"Attribute_GeometricTransform\": [";
            bool first = true;
            for (int i = 0; i < 16; i++) {
                if (first)
                    first = false;
                else
                    *outStream << ',';
                *outStream << aTransform->GetTrsf()[i];
            }
            *outStream << "],\n";

        }
        else if (anAttrib->IsKind(TypeOf_JtAttribute_Material))
        {
            Handle(JtAttribute_Material) aMaterial =
                Handle(JtAttribute_Material)::DownCast(anAttrib);

            *outStream << indentOp(indention) << "\"Attribute_MaterialDiffuseColor\": [";
            bool first = true;
            for (int i = 0; i < 4; i++) {
                if (first)
                    first = false;
                else
                    *outStream << ',';
                *outStream << aMaterial->DiffuseColor()[i];
            }
            *outStream << "],\n";

            aMaterial->AmbientColor();
            aMaterial->SpecularColor();
            aMaterial->EmissionColor();

        }
        else {
            *outStream << indentOp(indention) << "Unknown Attribute type \n";
        }
       
    }


}

void HandleAllChildren(const Handle(JtNode_Group)& theGroupRecord, const std::string& thePrefix)
{
    if (theGroupRecord->Children().IsEmpty())
        return;

    *outStream << indentOp(indention) << "\"children\":\n";
    *outStream << indentOp(indention) << "[\n";
    indention++;

    bool first = true;

    for (Standard_Integer aChildIdx = 0; aChildIdx < (Standard_Integer)theGroupRecord->Children().Count(); ++aChildIdx)
    {
        if (first)
            first = false;
        else
            *outStream << indentOp(indention) << ",\n";

        Handle(JtNode_Base) aChildRecord = Handle(JtNode_Base)::DownCast(theGroupRecord->Children()[aChildIdx]);

        if (aChildRecord.IsNull())
        {
            *outStream << indentOp(indention) << "{}";
            continue;
        }

        RecurseDownTheTree(aChildRecord, thePrefix);
    }

    indention--;
    *outStream << indentOp(indention) << "]\n";
}