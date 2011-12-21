

#include "CFixedScaleAnimator.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorFixedScale::CSceneNodeAnimatorFixedScale(u32 time, const core::vector3df& startScale, const core::vector3df& scaleSpeed, const core::vector3df& endScale)
	: ScaleSpeed(scaleSpeed), StartScale(startScale), EndScale(endScale), StartTime(time)
{
#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorFixedScale");
#endif
}

//! animates a scene node
void CSceneNodeAnimatorFixedScale::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node) // thanks to warui for this fix
	{
		const u32 diffTime = timeMs - StartTime;

		if (diffTime != 0)
		{
			core::vector3df scale = node->getScale() + ScaleSpeed*(diffTime*0.1f);
			node->setScale(scale);
			StartTime=timeMs;

			if (!scale.isBetweenPoints(StartScale, EndScale))
			{
				// we need this, if delta time is too large
				// and we jumped out of our interval
				node->setScale(EndScale);
				node->removeAnimator(this);
				return;
			}
		}
	}
}

//! Writes attributes of the scene node animator.
void CSceneNodeAnimatorFixedScale::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addVector3d("ScaleSpeed", ScaleSpeed);
	out->addVector3d("EndScale", EndScale);
}

//! Reads attributes of the scene node animator.
void CSceneNodeAnimatorFixedScale::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	ScaleSpeed = in->getAttributeAsVector3d("ScaleSpeed");
	EndScale = in->getAttributeAsVector3d("EndScale");
}

//! Creates a clone of this animator.
/** Please note that you will have to drop
(IReferenceCounted::drop()) the returned pointer after calling this. */
ISceneNodeAnimator* CSceneNodeAnimatorFixedScale::createClone(ISceneNode* node, ISceneManager* newManager)
{
	CSceneNodeAnimatorFixedScale * newAnimator = 
		new CSceneNodeAnimatorFixedScale(StartTime, StartScale, ScaleSpeed, EndScale);

	return newAnimator;
}

} // end namespace scene
} // end namespace irr
