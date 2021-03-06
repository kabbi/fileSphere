

#ifndef __CLEANUPANIMATOR_H
#define __CLEANUPANIMATOR_H

#include <irrlicht.h>
using namespace irr;

namespace irr
{
namespace scene
{

/// Scales the node from and to specified values in specified time
class CSceneNodeAnimatorCleanup : public ISceneNodeAnimator
{
	public:

		//! constructor
		CSceneNodeAnimatorCleanup();

		//! animates a scene node
		virtual void animateNode(ISceneNode* node, u32 timeMs);

		//! Writes attributes of the scene node animator.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		//! Reads attributes of the scene node animator.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);
		
		//! Creates a clone of this animator.
		/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer after calling this. */
		virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0);

	private:
		core::vector3df ScaleSpeed;
		core::vector3df StartScale;
		core::vector3df EndScale;
		u32 StartTime;
};

inline ISceneNodeAnimator *createCleanupAnimator()
{
	return new CSceneNodeAnimatorCleanup();
}


} // end namespace scene
} // end namespace irr



#endif