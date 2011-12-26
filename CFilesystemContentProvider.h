

#ifndef _C_FILESYSTEM_CONTENT_PROVIDER_H
#define _C_FILESYSTEM_CONTENT_PROVIDER_H

#include "IContentProvider.h"

class CFilesystemContentProvider : public IContentProvider
{
protected:
public:
	CFilesystemContentProvider();
	~CFilesystemContentProvider();

	void updateContent(entrySphere *entry);
	entrySphere *createRootNode(CSphereSystem *sys);
};

#endif