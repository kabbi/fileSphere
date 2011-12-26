

#include "CMainPositioner.h"

int sgn(int x)
{
	if (x==0)
		return 0;
	return (x>0)?1:-1;
}

void CMainPositioner::recalcPositions(scene::ISceneManager *smgr, entrySphere *current, core::array<entrySphere*> &children, f32 startAngle, f32 endAngle)
{
	int size=children.size();

	// lots of mathematical functions...
	updateAbsolutePosition(smgr, current->getSphere());
	f32 curScale=current->getSphere()->getAbsoluteTransformation().getScale().X;
	f32 curRadius=sphereOffset*curScale;
	f32 childDiameter=sphereRadius*2 * curScale*sphereScale;
	f32 deltaAngle=abs(endAngle-startAngle);

	f32 circleLength=2.0f*core::PI*curRadius / (2*core::PI) * deltaAngle;
	u32 numSegments=circleLength/childDiameter;
	f32 step=deltaAngle/numSegments; // in radians
	int curChild=0;
	int maxLevel=sphereOffset*2/childDiameter;

	// 0 - center, +1 - above it, -1 - below it etc...
	int level=0;
	int levelInc=1;
	while (abs(level)<maxLevel)
	{
		// and here again, recount number of entries fitting on each level, and position them
		float curDistance=sphereOffset*sin(core::PI/2-step*level);
		curDistance*=curScale;

		int curNumSegments=2.0f*core::PI*curDistance/childDiameter / (2*core::PI) * deltaAngle;
		curNumSegments=core::min_(curNumSegments, size-curChild);

		float curStep=deltaAngle/curNumSegments; // also in radians

		for (int i=0; i<curNumSegments; i++)
		{
			// spherical coordinates transform
			float fi=startAngle+curStep*i;
			float si=core::PI/2-step*level;
			float r=sphereOffset;
			children[curChild]->getSphere()->setPosition(core::vector3df(r*sin(si)*cos(fi), r*cos(si), r*sin(si)*sin(fi)));

			core::vector3df rot(children[curChild]->getSphere()->getPosition().getHorizontalAngle());
			//rot.Y-=45;
			children[curChild]->getSphere()->setRotation(rot);
			curChild++;
		}

		level+=levelInc;
		levelInc=-(levelInc+sgn(levelInc)*1); // why have I written 1 here??
	}
}

void CMainPositioner::recalcPositions(scene::ISceneManager *smgr, entrySphere *current, core::array<entrySphere*> &children)
{
	if (!current->getParent())
		recalcPositions(smgr, current, children, 0, 2*core::PI); // full sphere covered
	else
	{
		// find the right angle
		f32 startAngle=/*core::degToRad(current->getSphere()->getPosition().getHorizontalAngle().Y)*/0;
		f32 endAngle=startAngle+core::PI/2;
		recalcPositions(smgr, current, children, startAngle, endAngle); // cool "shield" positioning
	}
}