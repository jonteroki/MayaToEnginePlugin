#include <iostream>
#include "mayaIncludes.h"
#include "ComLib.h"
#include <maya/MTimer.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>

using namespace std;
MCallbackIdArray callbackIdArray;
MObject m_node;
MStatus status = MS::kSuccess;
bool initBool = false;

enum NODE_TYPE { TRANSFORM, MESH };
MTimer gTimer;

MString transformChangedCheck, renameChangedCheck, globalTransformChangedCheck;

// keep track of created meshes to maintain them
queue<MObject> newMeshes;

enum MSG_TYPE {
	NEW_MESH,
	RENAME_MESH,
	REMOVE_MESH,
	CHANGE_TRANSFORM,
	CHANGE_VERTEX,
	NEW_MATERIAL,
	CHANGE_MATERIAL,
	CHANGE_CAMERA};

struct MSG_HEADER
{
	MSG_TYPE msgType;
	char name[256];
};
//--===ComLib===--

//ComLib* producer;
//bool produce(char* message, int sizeMessage, MSG_HEADER header);

//--======||======--

/*
void nodeAttributeChanged(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void* x)
{
	// cout << plug.name() << ": " << plug.partialName() << endl;
}
*/

/*
 * how Maya calls this method when a node is added.
 * new POLY mesh: kPolyXXX, kTransform, kMesh
 * new MATERIAL : kBlinn, kShadingEngine, kMaterialInfo
 * new LIGHT    : kTransform, [kPointLight, kDirLight, kAmbientLight]
 * new JOINT    : kJoint
 */
void callbackNodeAdded(MObject &node, void *clientData)
{
	if (node.hasFn(MFn::Type::kTransform))
	{
		MFnTransform transform(node);
		MString added = ("\n---=== added Transform Node: ")+transform.absoluteName() + (" ===---");

		MGlobal::displayInfo(added);
	}
	if (node.hasFn(MFn::Type::kMesh))
	{
		MFnMesh mesh(node);
		MString added = ("\n---=== added Mesh Node: ") + mesh.absoluteName() + (" ===---");

		MGlobal::displayInfo(added);
		
	}

}
void callbackNodeRemoved(MObject &node, void *clientData)
{
	if (node.hasFn(MFn::Type::kTransform))
	{
		MFnTransform transform(node);
		MString removed = ("\n---=== Removed Transform Node: ");
		removed += transform.name() + (" ===---");
		
		MGlobal::displayInfo(removed);
	}
	else if (node.hasFn(MFn::Type::kPolyMesh))
	{
		MFnMesh mesh(node);
		MString removed = ("\n---=== Removed PolyMesh Node: ");
		removed += mesh.name() + (" ===---");

		MGlobal::displayInfo(removed);
	}
	else if (node.hasFn(MFn::Type::kLight))
	{
		MFnLight light(node);
		MString removed = ("\n---=== Removed Light Node: ");
		removed += light.name() + (" ===---");

		MGlobal::displayInfo(removed);
	}
}
void callbackNodeRenamed(MObject &node, const MString &str, void *clientData)
{
	if (node.hasFn(MFn::Type::kTransform))
	{
		MString previousName = ("\n---=== Previous Name of Transform: ");
		MString newName = ("\n---=== New Name of Transform: ");
	
		MFnTransform trans(node);
		newName += trans.name() + (" ===---");
		previousName += str + " ===---";

		if (renameChangedCheck != newName)
		{
			MSG_HEADER myHeader;
			myHeader.msgType = MSG_TYPE::RENAME_MESH;
			strcpy_s(myHeader.name, strlen(myHeader.name), (char*)trans.name().asChar());
			
			std::string mm = "Hello";
			char* myMsg = (char*)mm.c_str();
			//produce(myMsg, mm.size(), myHeader);
			MGlobal::displayInfo(previousName + newName);
			renameChangedCheck = newName;
		}
	}
	if (node.hasFn(MFn::Type::kMesh))
	{
		MString previousName = ("\n---=== Previous Name of Mesh: ");
		MString newName = ("\n---=== New Name of Mesh: ");

		MFnMesh mesh(node);
		newName += mesh.name() + (" ===---");
		previousName += str + " ===---";

		if (renameChangedCheck != newName)
		{
			MGlobal::displayInfo(previousName + newName);
			renameChangedCheck = newName;
		}
	}
	if (node.hasFn(MFn::Type::kCamera))
	{
		MString previousName = ("\n---=== Previous Name of Camera: ");
		MString newName = ("\n---=== New Name of Camera: ");

		MFnCamera cam(node);
		newName += cam.name() + (" ===---");
		previousName += str + " ===---";

		if (renameChangedCheck != newName)
		{
			MGlobal::displayInfo(previousName + newName);
			renameChangedCheck = newName;
		}
	}

}

void callbackVertexChanged(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData)
{	
	MDagPath path;
	MFnDagNode dag(plug.node());
	dag.getPath(path);

	if (path.hasFn(MFn::Type::kMesh))
	{
		//if (plug.partialName() == "pt")
		//{
			MFnMesh mesh(path);
			MPointArray data;
			mesh.getPoints(data/*, MSpace::kWorld*/);
			MString vertexData = ("---=== Vertices : ===---\n");
			MString normalData = ("---=== Normals : ===---\n");
			MString uvData = ("---=== UVs : ===---\n");


			MFloatVectorArray normals;
			mesh.getNormals(normals);
			
			MFloatArray uvsU, uvsV;
			mesh.getUVs(uvsU, uvsV);


			float arr[4];
			float arr2[4];
			float arr3[2];

			for (int i = 0; i < mesh.numVertices(); i++)
			{

				data[i].get(arr);
				vertexData += ("  ");
				vertexData += i+1;
				vertexData += (" X ");
				vertexData += arr[0];
				vertexData += (" Y ");
				vertexData += arr[1];
				vertexData += (" Z ");
				vertexData += arr[2];
				vertexData += ("\n ");

				normals[i].get(arr2);
				normalData += ("  ");
				normalData += i + 1;
				normalData += (" X ");
				normalData += arr2[0];
				normalData += (" Y ");
				normalData += arr2[1];
				normalData += (" Z ");
				normalData += arr2[2];
				normalData += ("\n ");


			}
		
			for (int i = 0; i < mesh.numUVs(); i++)
			{
				uvsU[i];
				uvData += ("  ");
				uvData += i + 1;
				uvData += (" X ");
				uvData += uvsU[i];
				uvData += (" Y ");
				uvData += uvsV[i];
				uvData += "\n";
			}

			MGlobal::displayInfo(vertexData);
			MGlobal::displayInfo(normalData);
			MGlobal::displayInfo(uvData);
		//}
	}

}	
void callbackTransformChanged(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData)
{
	MDagPath path;
	MFnDagNode dag(plug.node());
	dag.getPath(path);
	

	MStatus status;
	MMatrix transformation = dag.transformationMatrix(&status);
	
	unsigned int nrOfParents = dag.parentCount();

	if (path.hasFn(MFn::Type::kTransform))
	{

	
		MFnTransform transform;
		MString transformChange = ("---=== Local Transform: ");
		transformChange += transform.name();
		transformChange += (" Matrix ===---\n");

		for (int i = 0; i < 4; i++)
		{
			transformChange += ("		- ");
			for (int j = 0; j < 4; j++)
			{

				transformChange += transformation[i][j];
				transformChange += (" | ");

			}
			transformChange += ("\n");
		}

		if (transformChange != transformChangedCheck)
		{
			transformChangedCheck = transformChange; 

		}

		MGlobal::displayInfo(transformChange);


		MMatrix GlobalTransformMatrix;
		GlobalTransformMatrix = transformation;

		for (int i = 0; i < nrOfParents; i++)
		{
			MObject parent = dag.parent(i, &status);
			if (status == MS::kSuccess)
			{
				MFnDagNode parentNode(parent);
				GlobalTransformMatrix *= parentNode.transformationMatrix();
			}

		}

		MString globalTransformChange = ("---=== World Transform: ");
		globalTransformChange += transform.name();
		globalTransformChange += (" Matrix ===---\n");

		for (int i = 0; i < 4; i++)
		{
			globalTransformChange += ("		- ");
			for (int j = 0; j < 4; j++)
			{

				globalTransformChange += GlobalTransformMatrix[i][j];
				globalTransformChange += (" | ");

			}
			globalTransformChange += ("\n");
		}

		if (globalTransformChange != globalTransformChangedCheck)
		{
			globalTransformChangedCheck = globalTransformChange;

		}

		MGlobal::displayInfo(globalTransformChange);

	}
}

void callbackTimerChanged(float elapsedTime, float lastTime, void*clientData)
{
	MString msg("Elapsed time: ");
	msg += lastTime + elapsedTime;
	MGlobal::displayInfo(msg);
	lastTime += elapsedTime;
}

/*
 * Plugin entry point
 * For remote control of maya
 * open command port: commandPort -nr -name ":1234"
 * close command port: commandPort -cl -name ":1234"
 * send command: see loadPlugin.py and unloadPlugin.py
 */
EXPORT MStatus initializePlugin(MObject obj) {
	MStatus res = MS::kSuccess;
	MFnPlugin myPlugin(obj, "level editor", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
		return res;
	}

	//producer = new ComLib("LenoOchRockisSharedMemory", 10, ComLib::PRODUCER);




	// redirect cout to cerr, so that when we do cout goes to cerr
	// in the maya output window (not the scripting output!)
	MGlobal::displayError(" ---===Maya plugin loaded!===---");
	MStatus status = MS::kSuccess;

	MObject Name, TChanged, VChanged, Removed, Added;
	MCallbackId id;

	//id = MTimerMessage::addTimerCallback(5.0, callbackTimerChanged, NULL, &status);
	//if (status = MS::kSuccess)
		//callbackIdArray.append(id);
	
	/*
	id = MDGMessage::addNodeAddedCallback(callbackNodeAdded, "dependNode", NULL, &status);
	if (status = MS::kSuccess)
		callbackIdArray.append(id);
	id = MDGMessage::addNodeRemovedCallback(callbackNodeRemoved, "dependNode", NULL, &status);
	if (status = MS::kSuccess)
		callbackIdArray.append(id);

	id = MNodeMessage::addNameChangedCallback(Name, callbackNodeRenamed, NULL, &status);
	if (status = MS::kSuccess)
		callbackIdArray.append(id);

	id = MNodeMessage::addAttributeChangedCallback(VChanged, callbackVertexChanged, NULL, &status);
	if (status = MS::kSuccess)
		callbackIdArray.append(id);

	id = MNodeMessage::addAttributeChangedCallback(TChanged, callbackTransformChanged, NULL, &status);
	if (status = MS::kSuccess)
		callbackIdArray.append(id);

		*/


	// a handy timer, courtesy of Maya
	/*gTimer.clear();
	gTimer.beginTimer();*/

	return res;
}


EXPORT MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);
	MGlobal::displayInfo(" // ---=== Plugin Unloaded ===---");

	//delete producer;

	//MMessage::removeCallbacks(callbackIdArray);
	return MS::kSuccess;
}

/*
bool produce(char* message, int sizeMessage, MSG_HEADER header)
{
	bool hasProduced = false;
	
	size_t msgSize = sizeof(MSG_HEADER) + sizeMessage;

	char *msg = new char[msgSize];
	memcpy(msg, &header, sizeof(header));
	memcpy(msg + sizeof(header), message, sizeMessage);
	
	if (producer->send(msg, msgSize))
	{
		std::cout << msg << std::endl;
		hasProduced = true;
	}
	delete[] msg;

	return hasProduced;
}
*/
