

#ifndef __CENTRYSPHERE_H
#define __CENTRYSPHERE_H

class entrySphere;

#include <irrlicht.h>
#include <windows.h>
#include <iostream>
#include "diskEntry.h"
#include "CFixedScaleAnimator.h"
#include "IEntryPositioner.h"
#include "programConfig.h"
using namespace irr;

// some helper functions, should not be here
void moveCam(scene::ISceneManager *smgr, core::vector3df moveTo);
void updateAbsolutePosition(scene::ISceneManager *smgr, scene::ISceneNode *node);
int sgn(int x);
void createMegaScaleAnimator(scene::ISceneManager *smgr, scene::ISceneNode *node, u32 time);

class entrySphere
{
protected:
	static entrySphere *globalParent;
	static entrySphere *globalRoot;
	static io::path texPath;

	static bool drawDebugData;
	static IEntryPositioner *positioner;

	IrrlichtDevice *device;
	scene::ISceneManager *smgr;

	diskEntry *entry;
	scene::ISceneNode *node;
	scene::ISceneNode *dummy;
	scene::IMeshSceneNode *sphere;
	scene::ITextSceneNode *label;

	entrySphere *parent;

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

	virtual void setDrawDebugData(bool flag) {drawDebugData=flag;node->setDebugDataVisible(flag?scene::EDS_FULL:scene::EDS_OFF);dummy->setDebugDataVisible(flag?scene::EDS_FULL:scene::EDS_OFF);}
	virtual bool isDrawingDebugData() {return drawDebugData;}

	virtual void setPositioner(IEntryPositioner *pos) {positioner=pos;}
	virtual IEntryPositioner *getPositioner() {return positioner;}

	virtual void setDeleteNodes(bool del) {deleteNodes=del;}
	virtual bool areNodesDeleted() {return deleteNodes;}

	virtual ~entrySphere();
};

#endif