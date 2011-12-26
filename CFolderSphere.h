

#ifndef __CFOLDERSPHERE_H
#define __CFOLDERSPHERE_H

class folderSphere;
#include "CEntrySphere.h"

class folderSphere : public entrySphere
{
protected:

public:
	// create the root node and scan and open
	folderSphere(CSphereSystem *system, io::path fullPath, core::stringw name, entrySphere *parent);

	// create non-root node
	folderSphere(CSphereSystem *system, io::path fullPath, core::stringw name);

	virtual void process();

	virtual bool onEvent(const SEvent &event);

	virtual void setParent(entrySphere *parent);

	virtual ~folderSphere();
};

#endif