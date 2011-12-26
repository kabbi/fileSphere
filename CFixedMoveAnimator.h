

#ifndef __MOVEANIMATOR_H
#define __MOVEANIMATOR_H

#include <irrlicht.h>
using namespace irr;

namespace irr
{
namespace scene
{

/// Scales the node from and to specified values in specified time
class CSceneNodeAnimatorFixedMove : public ISceneNodeAnimator
{
	public:

		//! constructor
		CSceneNodeAnimatorFixedMove(u32 time, const core::vector3df& startMove, const core::vector3df& moveSpeed, const core::vector3df& endMove)
			: MoveSpeed(moveSpeed), StartMove(startMove), EndMove(endMove), StartTime(time)
		{
			#ifdef _DEBUG
			setDebugName("CSceneNodeAnimatorFixedMove");
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
					core::vector3df pos = node->getPosition() + MoveSpeed*(diffTime*0.1f);
					node->setPosition(pos);
					StartTime=timeMs;

					if (!pos.isBetweenPoints(StartMove, EndMove))
					{
						// we need this, if delta time is too large
						// and we jumped out of our interval
						node->setPosition(EndMove);
						node->removeAnimator(this);
						return;
					}
				}
			}
		}

		//! Writes attributes of the scene node animator.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
		{
			out->addVector3d("MoveSpeed", MoveSpeed);
			out->addVector3d("EndMove", EndMove);
		}

		//! Reads attributes of the scene node animator.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
		{
			MoveSpeed = in->getAttributeAsVector3d("MoveSpeed");
			EndMove = in->getAttributeAsVector3d("EndMove");
		}
		
		//! Creates a clone of this animator.
		/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer after calling this. */
		virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0)
		{
			CSceneNodeAnimatorFixedMove * newAnimator = 
				new CSceneNodeAnimatorFixedMove(StartTime, StartMove, MoveSpeed, EndMove);

			return newAnimator;
		}

	private:
		core::vector3df MoveSpeed;
		core::vector3df StartMove;
		core::vector3df EndMove;
		u32 StartTime;
};

inline ISceneNodeAnimator *createFixedMoveAnimator(IrrlichtDevice *device, const core::vector3df& startMove, const core::vector3df& moveSpeed, const core::vector3df& endMove)
{
	return new CSceneNodeAnimatorFixedMove(device->getTimer()->getTime(), startMove, moveSpeed, endMove);
}


} // end namespace scene
} // end namespace irr



#endif