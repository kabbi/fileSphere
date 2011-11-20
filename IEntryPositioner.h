

#ifndef __IENTRYPOSITIONER_H
#define __IENTRYPOSITIONER_H

#include <irrlicht.h>
#include <cmath>
#include "CEntrySphere.h"
#include "programConfig.h"
using namespace std;

class IEntryPositioner
{
private:
public:
	IEntryPositioner()
	{
	}

	virtual void recalcPositions(scene::ISceneManager *smgr, entrySphere *current, core::array<entrySphere*> &children)=0;

	~IEntryPositioner()
	{
	}
};

#endif