

#ifndef __CFILESPHERE_H
#define __CFILESPHERE_H

class fileSphere;
#include "CEntrySphere.h"

class fileSphere : public entrySphere
{
protected:
public:
	// create non-root (of course!)) node
	fileSphere(CSphereSystem *system, io::path fullPath, core::stringw name, entrySphere *parent);

	virtual bool onEvent(const SEvent &event);

	virtual void setParent(entrySphere *parent);

	virtual ~fileSphere();
};

#endif