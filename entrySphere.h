

#ifndef __ENTRYSPHERE_H
#define __ENTRYSPHERE_H

#include "diskEntry.h"
#include <irrlicht.h>
#include <windows.h>
#include <iostream>
#include "fixedScaleAnimator.h"
using namespace irr;

const core::vector3df sphereOffset(15, 0, 0);
const core::vector3df sphereScale(0.2f, 0.2f, 0.2f);
const core::vector3df sphereScaleDef(1, 1, 1);
const core::vector3df nullVector(0, 0, 0);
const f32 sphereRadius=10;
const u32 sphereDensity=16;
//const video::E_MATERIAL_FLAG selFlag=video::EMF_POINTCLOUD;

// some helper functions, should not be here
void moveCam(scene::ISceneManager *smgr, core::vector3df moveTo);
void updateAbsolutePosition(scene::ISceneManager *smgr, scene::ISceneNode *node);
int sgn(int x);

class entrySphere
{
protected:
	static entrySphere *globalParent;
	static entrySphere *globalRoot;
	static io::path texPath;

	IrrlichtDevice *device;
	scene::ISceneManager *smgr;

	diskEntry *entry;
	scene::ISceneNode *node;
	scene::ISceneNode *dummy;
	scene::IMeshSceneNode *sphere;
	scene::ITextSceneNode *label;

	entrySphere *parent;

	bool drawDebugData;
	bool deleteNodes;
	bool selected;
	bool active;

	// create the 3D representation
	virtual void createNode();
public:
	entrySphere(diskEntry *entry, IrrlichtDevice *device, entrySphere *parent);

	virtual bool onEvent(const SEvent &event);

	virtual scene::ISceneNode *getNode() {return node;}

	virtual scene::ISceneNode *getSphere() {return sphere;}

	virtual scene::ISceneNode *getDummy() {return dummy;}

	virtual scene::ITextSceneNode *getLabel(){return label;}

	virtual void setParent(entrySphere *parent);
	virtual entrySphere *getParent() {return parent;}

	virtual void process();

	virtual void setSelected(scene::ISceneNode *node) {setSelected(node==sphere);}

	virtual void setSelected(bool sel);
	virtual bool isSelected() {return selected;}

	virtual void setActive(bool active);
	virtual bool isActive() {return active;}

	virtual diskEntry *getEntry() {return entry;}

	virtual entrySphere *getGlobalRoot() {return globalRoot;}
	virtual entrySphere *getGlobalParent() {return globalParent;}

	virtual void setDeleteNodes(bool del) {deleteNodes=del;}
	virtual bool areNodesDeleted() {return deleteNodes;}

	virtual void setDrawDebugData(bool flag) {drawDebugData=flag;}
	virtual bool isDrawingDebugData() {return drawDebugData;}

	virtual ~entrySphere();
};

#endif