

#ifndef __IENTRYPOSITIONER_H
#define __IENTRYPOSITIONER_H

#include <irrlicht.h>
using namespace std;

// pre-define
class IEntryPositioner;
#include <cmath>
#include "CEntrySphere.h"
#include "programConfig.h"

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