

#ifndef __SCALEANIMATOR_H
#define __SCALEANIMATOR_H

#include <irrlicht.h>
using namespace irr;

namespace irr
{
namespace scene
{

/// Scales the node from and to specified values in specified time
class CSceneNodeAnimatorFixedScale : public ISceneNodeAnimator
{
	public:

		//! constructor
		CSceneNodeAnimatorFixedScale(u32 time, const core::vector3df& startScale, const core::vector3df& scaleSpeed, const core::vector3df& endScale)
			: ScaleSpeed(scaleSpeed), StartScale(startScale), EndScale(endScale), StartTime(time)
		{
			#ifdef _DEBUG
			setDebugName("CSceneNodeAnimatorFixedScale");
			#endif
		}

		//! animates a scene node
		virtual void animateNode(ISceneNode* node, u32 timeMs)
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
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
		{
			out->addVector3d("ScaleSpeed", ScaleSpeed);
			out->addVector3d("EndScale", EndScale);
		}

		//! Reads attributes of the scene node animator.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
		{
			ScaleSpeed = in->getAttributeAsVector3d("ScaleSpeed");
			EndScale = in->getAttributeAsVector3d("EndScale");
		}
		
		//! Creates a clone of this animator.
		/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer after calling this. */
		virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0)
		{
			CSceneNodeAnimatorFixedScale * newAnimator = 
				new CSceneNodeAnimatorFixedScale(StartTime, StartScale, ScaleSpeed, EndScale);

			return newAnimator;
		}

	private:
		core::vector3df ScaleSpeed;
		core::vector3df StartScale;
		core::vector3df EndScale;
		u32 StartTime;
};

inline ISceneNodeAnimator *createFixedScaleAnimator(IrrlichtDevice *device, const core::vector3df& startScale, const core::vector3df& scaleSpeed, const core::vector3df& endScale)
{
	return new CSceneNodeAnimatorFixedScale(device->getTimer()->getTime(), startScale, scaleSpeed, endScale);
}


} // end namespace scene
} // end namespace irr



#endif