

#ifndef __CFILESPHERE_H
#define __CFILESPHERE_H

#include "CEntrySphere.h"

class fileSphere : public entrySphere
{
protected:
public:
	// create non-root (of course!)) node
	fileSphere(fileEntry *entry, IrrlichtDevice *device, entrySphere *parent);

	virtual bool onEvent(const SEvent &event);

	virtual void setParent(entrySphere *parent);

	virtual ~fileSphere();
};

#endif