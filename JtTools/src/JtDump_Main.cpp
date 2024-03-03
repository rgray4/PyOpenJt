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

#include <iostream>
#include <iomanip>
#include <assert.h>

//#include <Matrix.h>

using namespace std;

std::ostream &outStream = std::cout;
int indention = 0;

void HandleAllChildren(const Handle(JtNode_Group)& theGroupRecord, const std::string& thePrefix);
void RecurseDownTheTree(const Handle(JtNode_Base)& theNodeRecord, const std::string& thePrefix);

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

    if (files.size() > 1) {
        outStream << '[' << endl;
        indention++;
    }


	// go through Jt files
	for (const auto& aarg : files) {

        outStream << '{' << endl;
        indention++;


		TCollection_ExtendedString aFileName(aarg.c_str(), Standard_True);


		Handle(JtData_Model) rootModel =   new JtData_Model(aFileName);

        writeModel(rootModel, outStream, indention);

		if (rootModel.IsNull())
			return 2;

		Handle(JtNode_Partition) PartitionNode = rootModel->Init();


        RecurseDownTheTree(PartitionNode, aarg);


        outStream << '}' << endl;
        indention--;
		
	}

    if (files.size() > 1) {
        outStream << ']' << endl;
        indention--;
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


void RecurseDownTheTree(const Handle(JtNode_Base)& theNodeRecord, const std::string& thePrefix)
{

    // Handle all different types of LSG Nodes
    if (theNodeRecord->IsKind(TypeOf_JtNode_Partition))
    {
        Handle(JtNode_Partition) aPartitionRecord = Handle(JtNode_Partition)::DownCast(theNodeRecord);

        aPartitionRecord->FileName();

        cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "JtNode_Partition \n";

        HandleAllChildren( aPartitionRecord, thePrefix);
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Part))
    {
        Handle(JtNode_Part) aPartRecord = Handle(JtNode_Part)::DownCast(theNodeRecord);

        const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();

        cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "JtNode_Part LateLoad: " << aLateLoaded.Count() << "\n";
        
        for (int i = 0; i < aLateLoaded.Count(); i++) {
            if (aLateLoaded[i]->getSegmentType() == SegmentType::Meta_Data) {
                if (aLateLoaded[i]->DefferedObject().IsNull())
                    aLateLoaded[i]->Load();

                Handle(JtData_Object)  prop = aLateLoaded[i]->DefferedObject();
                if (prop->IsKind(TypeOf_JtElement_ProxyMetaData)) {
                    Handle(JtElement_ProxyMetaData) aProxyMetaDataElement =
                        Handle(JtElement_ProxyMetaData)::DownCast(prop);

                    auto stream = aProxyMetaDataElement->getKeyValueStream();

                    //writeKeyValueStream(stream, cout,  indention);

                    aLateLoaded[i]->Unload();

                }
            }
        }

        HandleAllChildren(Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_MetaData))
    {
        Handle(JtNode_MetaData) aPartRecord = Handle(JtNode_MetaData)::DownCast(theNodeRecord);
        const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();

        cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "TypeOf_JtNode_MetaData " << aLateLoaded.Count() << "\n";

        for (int i = 0; i < aLateLoaded.Count(); i++) {
            if (aLateLoaded[i]->getSegmentType() == SegmentType::Meta_Data) {
                if (aLateLoaded[i]->DefferedObject().IsNull())
                    aLateLoaded[i]->Load();

                Handle(JtData_Object)  prop = aLateLoaded[i]->DefferedObject();
                if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_ProxyMetaData)) {
                    Handle(JtElement_ProxyMetaData) aProxyMetaDataElement =
                        Handle(JtElement_ProxyMetaData)::DownCast(prop);

                    auto stream = aProxyMetaDataElement->getKeyValueStream();

                    //writeKeyValueStream(stream, cout, indention);

                    aLateLoaded[i]->Unload();

                }
            }
        }

        HandleAllChildren(Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_RangeLOD))
    {
        Handle(JtNode_RangeLOD) aRangeLODRecord =
            Handle(JtNode_RangeLOD)::DownCast(theNodeRecord);

        cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "JtNode_RangeLOD \n";

        HandleAllChildren(aRangeLODRecord, thePrefix);

        /*
        Eigen::Vector4f(
            static_cast<Standard_ShortReal> (aRangeLODRecord->Center().X),
            static_cast<Standard_ShortReal> (aRangeLODRecord->Center().Y),
            static_cast<Standard_ShortReal> (aRangeLODRecord->Center().Z),
            1.0);

        if (!aRangeLODRecord->RangeLimits().IsEmpty())
        {
            for (Standard_Integer anIdx = 1; anIdx <= aRangeLODRecord->RangeLimits().Count(); ++anIdx)
            {
                aRangeLOD->Ranges().push_back(aRangeLODRecord->RangeLimits()[anIdx]);
            }
        }
        else
        {
            aRangeLOD->Ranges().push_back(std::numeric_limits<Standard_ShortReal>::max());

            for (Standard_Integer anIdx = 1; anIdx < (Standard_Integer)aRangeLODRecord->Children().Count(); ++anIdx)
            {
                aRangeLOD->Ranges().push_back(0.0);
            }
        }

        */
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_LOD))
    {
        cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "JtNode_LOD \n";
        HandleAllChildren(Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Group))
    {
        cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "JtNode_Group \n";
        HandleAllChildren( Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Instance))
    {
        Handle(JtNode_Instance) anInstance = Handle(JtNode_Instance)::DownCast(theNodeRecord);

        // Note: To support JT Viewer operations (such as object hiding) it is convenient
        // to eliminate instance nodes from the scene graph. In result, using of sub-tree
        // references becomes impossible. But the actual geometric data is not duplicated
        // by decomposing the node on the description part and data source part.

        Handle(JtNode_Base) aNode = Handle(JtNode_Base)::DownCast(anInstance->Object());

        cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "JtNode_Instance \n";

        indention++;
        RecurseDownTheTree(aNode, thePrefix);
        indention--;

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Shape_Vertex))
    {
        Handle(JtNode_Shape_Vertex) aShapeRecord = Handle(JtNode_Shape_Vertex)::DownCast(theNodeRecord);

        if (aShapeRecord->IsKind(TypeOf_JtNode_Shape_TriStripSet))
        {
            Handle(JtNode_Shape_TriStripSet) aMeshRecord = Handle(JtNode_Shape_TriStripSet)::DownCast(aShapeRecord);

            aMeshRecord->Bounds();
            const JtData_Object::VectorOfLateLoads& aLateLoaded = aMeshRecord->LateLoads();

            cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "TypeOf_JtNode_Shape_Vertex->JtNode_Shape_TriStripSet LateLoad: " << aLateLoaded.Count() << "\n";

            for (int i = 0; i < aLateLoaded.Count(); i++) {
                
                if (aLateLoaded[i]->DefferedObject().IsNull())
                    aLateLoaded[i]->Load();

                Handle(JtData_Object) prop = aLateLoaded[i]->DefferedObject();
                if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_ShapeLOD_Vertex)) {
                    Handle(JtElement_ShapeLOD_Vertex) aProxyMetaDataElement = Handle(JtElement_ShapeLOD_Vertex)::DownCast(prop);

                    cout << indentOp(indention + 1) << "Vertices: " << aProxyMetaDataElement->Vertices().Count() << "\n";
                    aProxyMetaDataElement;
                
                }

            }


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
         
        }else
            cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "TypeOf_JtNode_Shape_Vertex->Unknown \n";
    }
    else
    {
        cout << indentOp(indention) << "'" << theNodeRecord->Name() << "' " << "Unknown node type \n";
    }


    // Extract attributes

    if (theNodeRecord->Attributes().IsEmpty())
        return ;

    for (Standard_Integer anIdx = 0; anIdx < (Standard_Integer)theNodeRecord->Attributes().Count(); ++anIdx)
    {
        const Handle(JtData_Object)& anObject = theNodeRecord->Attributes()[anIdx];

        if (anObject.IsNull())
            continue;

        Handle(JtAttribute_Base) anAttrib = Handle(JtAttribute_Base)::DownCast(anObject);

        if(anAttrib.IsNull())
            cerr << "Error! Invalid node attribute\n";

        indention++;

        if (anAttrib->IsKind(TypeOf_JtAttribute_GeometricTransform))
        {
            Handle(JtAttribute_GeometricTransform) aTransform =
                Handle(JtAttribute_GeometricTransform)::DownCast(anAttrib);
            cout << indentOp(indention) << "TypeOf_JtAttribute_GeometricTransform [";

            for(int i = 0; i<16; i++)
                cout << aTransform->GetTrsf()[i] << ", ";
            
            cout << "]\n";

            
        }
        else if (anAttrib->IsKind(TypeOf_JtAttribute_Material))
        {
            Handle(JtAttribute_Material) aMaterial =
                Handle(JtAttribute_Material)::DownCast(anAttrib);
            cout << indentOp(indention) << "TypeOf_JtAttribute_Material [";

            for (int i = 0; i < 4; i++)
                cout << aMaterial->DiffuseColor()[i] << ", ";

            cout << "]\n";
            aMaterial->AmbientColor();
            aMaterial->SpecularColor();
            aMaterial->EmissionColor();
            
        }
        else {
            cout << indentOp(indention) << "Unknown Attribute type \n";
        }
        indention--;
    }

}

void HandleAllChildren(const Handle(JtNode_Group)& theGroupRecord, const std::string& thePrefix)
{
    if (theGroupRecord->Children().IsEmpty())
        return;

    indention++;

    for (Standard_Integer aChildIdx = 0; aChildIdx < (Standard_Integer)theGroupRecord->Children().Count(); ++aChildIdx)
    {
        Handle(JtNode_Base) aChildRecord = Handle(JtNode_Base)::DownCast(theGroupRecord->Children()[aChildIdx]);

        if (aChildRecord.IsNull())
        {
            continue;
        }

        RecurseDownTheTree(aChildRecord, thePrefix);

        //JTData_NodePtr aChildNode = PushNode(aChildRecord, thePrefix);

        //Q_ASSERT_X(!aChildNode.isNull(),
        //    "PushNode", "Error! Failed to extract node from LSG segment");

       //if (!aChildNode.isNull())
        //{       
        //  theGroupNode->Children.push_back(aChildNode);
        //}
    }

    indention--;
}