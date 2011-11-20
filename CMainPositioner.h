

#ifndef __CMAINPOSITIONER_H
#define __CMAINPOSITIONER_H

#include "CEntrySphere.h"
#include "IEntryPositioner.h"

class CMainPositioner : public IEntryPositioner
{
private:
	virtual void recalcPositions(scene::ISceneManager *smgr, entrySphere *current, core::array<entrySphere*> &children, f32 startAngle, f32 endAngle);
public:
	virtual void recalcPositions(scene::ISceneManager *smgr, entrySphere *current, core::array<entrySphere*> &children);
};

#endif