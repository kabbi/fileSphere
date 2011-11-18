

#ifndef __FOLDER_SPHERE_H
#define __FOLDER_SPHERE_H

#include "entrySphere.h"
#include "fileSphere.h"

const int maxLevel=1;

class folderSphere : public entrySphere
{
protected:

	core::array<entrySphere*> children;

	// recalc the position of all subnodes, may be recursive
	void recalcPos();
	// startAngle and endAngle - angles in radians, bounaries for the nodes' placement
	void recalcChildrenPos(f32 startAngle, f32 endAngle);

	void scanFolder();

public:
	// create the root node and scan and open
	folderSphere(folderEntry *entry, IrrlichtDevice *device);

	// create non-root node
	folderSphere(folderEntry *entry, IrrlichtDevice *device, entrySphere *parent);

	virtual void openFolder();
	virtual void closeFolder();

	virtual bool onEvent(const SEvent &event);

	virtual void process();

	virtual void setSelected(scene::ISceneNode *node);

	virtual void setDrawDebugData(bool flag);

	virtual void setParent(entrySphere *parent);

	virtual ~folderSphere();
};

#endif