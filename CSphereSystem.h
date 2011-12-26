

#ifndef __C_SPHERE_SYSTEM_H
#define __C_SPHERE_SYSTEM_H

#include <irrlicht.h>
using namespace irr;

// exception
class CSphereSystemException
{
};

// pre-define
class CSphereSystem;
#include "IEntryPositioner.h"
#include "IContentProvider.h"
#include "CEntrySphere.h"

class CSphereSystem
{
protected:
	// Engine pointers
	IrrlichtDevice *device;
	scene::ISceneManager *smgr;
	gui::IGUIEnvironment *guienv;
	video::IVideoDriver *driver;

	// global parameters for all subnodes
	core::array<entrySphere*> folderHistory;
	io::path texPath;
	bool drawDebugData;
	IEntryPositioner *positioner;
	entrySphere *selectedEntry;

	// and other things...
	entrySphere *root;
	IContentProvider *provider;
public:
	CSphereSystem(IrrlichtDevice *device, IContentProvider *provider, IEntryPositioner *poser);
	virtual ~CSphereSystem();

	// lots of getters and setters
	virtual entrySphere *getRoot()
	{
		if (folderHistory.size()>0)
			return folderHistory[folderHistory.size()-1];
		else
			return 0;
	}
	virtual entrySphere *getParent()
	{
		if (folderHistory.size()>1)
			return folderHistory[folderHistory.size()-2];
		else
			return 0;
	}

	virtual core::array<entrySphere*> &getFolderHistory() {return folderHistory;}
	virtual bool isDrawingDebugData() {return drawDebugData;}
	virtual IEntryPositioner *getPositioner() {return positioner;}
	virtual io::path getTexturePath() {return texPath;}

	virtual void setDrawDebugData(bool flag) {drawDebugData=flag;}
	virtual void setTexturePath(io::path path) {texPath=path;}
	virtual void setEntrypositioner(IEntryPositioner *pos) {positioner=pos;}

	virtual IrrlichtDevice *getIrrlicht() {return device;}
	virtual scene::ISceneManager *getSceneManager() {return smgr;}
	virtual gui::IGUIEnvironment *getGUIEnvironment() {return guienv;}
	virtual video::IVideoDriver *getVideoDriver() {return driver;}

	virtual void setSelected(scene::ISceneNode *node) {setSelected(node, root);}
	virtual void setSelected(scene::ISceneNode *node, entrySphere *entry);
	virtual entrySphere *getSelected() {return selectedEntry;}

	virtual void changeParent(entrySphere *newParent);

	virtual void updateContent(entrySphere *entry);

	// some general system methods
	virtual bool onEvent(const SEvent &event);
	virtual void process();
};

#endif