

#ifndef _I_CONTENT_PROVIDER_H
#define _I_CONTENT_PROVIDER_H

#include <irrlicht.h>
using namespace irr;

class IContentProvider;
#include "CSphereSystem.h"

class IContentProvider
{
protected:
public:
	IContentProvider() {}
	virtual ~IContentProvider() {}

	virtual void updateContent(entrySphere *entry)=0;
	virtual entrySphere *createRootNode(CSphereSystem *sys)=0;
};

#endif