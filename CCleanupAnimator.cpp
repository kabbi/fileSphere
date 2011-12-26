

#include "CCleanupAnimator.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorCleanup::CSceneNodeAnimatorCleanup()
{
#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorCleanup");
#endif
}

//! animates a scene node
void CSceneNodeAnimatorCleanup::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node) // thanks to warui for this fix
	{
		const core::list<ISceneNodeAnimator*> &animators=node->getAnimators();
		core::list<ISceneNodeAnimator*>::ConstIterator it;
		for (it=animators.begin(); it!=animators.end(); ++it)
			if ((*it)->hasFinished())
			{
				node->removeAnimator(*it);
				break;
			}
	}
}

//! Writes attributes of the scene node animator.
void CSceneNodeAnimatorCleanup::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
}

//! Reads attributes of the scene node animator.
void CSceneNodeAnimatorCleanup::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
}

//! Creates a clone of this animator.
/** Please note that you will have to drop
(IReferenceCounted::drop()) the returned pointer after calling this. */
ISceneNodeAnimator* CSceneNodeAnimatorCleanup::createClone(ISceneNode* node, ISceneManager* newManager)
{
	CSceneNodeAnimatorCleanup * newAnimator = 
		new CSceneNodeAnimatorCleanup();

	return newAnimator;
}

} // end namespace scene
} // end namespace irr
