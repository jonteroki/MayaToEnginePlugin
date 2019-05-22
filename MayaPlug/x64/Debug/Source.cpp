#include <iostream>
#include "mayaIncludes.h"

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

// keep track of created meshes to maintain them
queue<MObject> newMeshes;

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
void nodeAdded(MObject &node, void * clientData) {
	//... implement this and other callbacks
}
void callBackVertexMoved()
{

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

	// redirect cout to cerr, so that when we do cout goes to cerr
	// in the maya output window (not the scripting output!)
	cout.rdbuf(cerr.rdbuf());
	cout << "Plugin loaded ===========================" << endl;



	// register callbacks here for
	// MDGMessage::addNodeAddedCallback(nodedAdded, "dependNode", NULL, &status);
	// MDGMessage::addNodeRemovedCallback(nodeRemoved, "dependNode", NULL, &status);
	// MTimerMessage::addTimerCallback(0.1, timerCallback, NULL, &status) );

	// a handy timer, courtesy of Maya
	gTimer.clear();
	gTimer.beginTimer();

	return res;
}


EXPORT MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);
	cout << "Plugin unloaded =========================" << endl;
	MMessage::removeCallbacks(callbackIdArray);
	return MS::kSuccess;
}