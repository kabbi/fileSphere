

#ifndef __C_ENTRY_SPHERE_H
#define __C_ENTRY_SPHERE_H

#include <windows.h>
#include <iostream>
#include <irrlicht.h>
using namespace irr;

// pre-define
class entrySphere;
#include "CSphereSystem.h"
#include "IContentProvider.h"
#include "programConfig.h"

// some helper functions, should not be here
void updateAbsolutePosition(scene::ISceneManager *smgr, scene::ISceneNode *node);
int sgn(int x);

class entrySphere
{
protected:
	CSphereSystem *system;

	io::path fullPath;
	core::stringw name;

	scene::ISceneNode *node;
	scene::ISceneNode *dummy;
	scene::IMeshSceneNode *sphere;
	scene::ITextSceneNode *label;

	core::array<entrySphere*> children;
	entrySphere *parent;

	bool deleteNodes;
	bool selected;
	bool opened;
	bool active;

	// create the 3D representation
	virtual void createNode();
public:
	entrySphere(CSphereSystem *system, io::path fullPath, core::stringw name, entrySphere *parent);

	virtual bool onEvent(const SEvent &event);

	virtual CSphereSystem *getSystem() {return system;}

	virtual scene::ISceneNode *getNode() {return node;}
	virtual scene::ISceneNode *getSphere() {return sphere;}
	virtual scene::ISceneNode *getDummy() {return dummy;}
	virtual scene::ITextSceneNode *getLabel() {return label;}

	virtual core::array<entrySphere*> &getChildren() {return children;}

	virtual void setParent(entrySphere *parent);
	virtual entrySphere *getParent() {return parent;}

	virtual void process();

	virtual void setSelected(bool sel);
	virtual bool isSelected() {return selected;}

	virtual void setActive(bool active);
	virtual bool isActive() {return active;}

	virtual io::path getFullPath() {return fullPath;}
	virtual core::stringw getName() {return name;}

	virtual bool isOpened() {return opened;}

	virtual void setDeleteNodes(bool del) {deleteNodes=del;}
	virtual bool areNodesDeleted() {return deleteNodes;}

	virtual void openEntry(bool updateContent=false, bool animate=true);
	virtual void closeEntry(bool clear=true, bool animate=true);
	virtual void updateContent(IContentProvider *provider);

	// recalc the position of all subnodes, may be recursive
	void recalcPos();

	virtual ~entrySphere();
};

#endif