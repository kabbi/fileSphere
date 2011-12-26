

#include "CFolderSphere.h"

#include "CFixedScaleAnimator.h"

// create the root node and scan and open
folderSphere::folderSphere(CSphereSystem *system, io::path fullPath, core::stringw name)
	: entrySphere(system, fullPath, name, 0)
{
	createNode();
	setParent(parent);
	system->getFolderHistory().push_back(this);
}

// create non-root node
folderSphere::folderSphere(CSphereSystem *system, io::path fullPath, core::stringw name, entrySphere *parent)
	: entrySphere(system, fullPath, name, parent)
{
	createNode();
	setParent(parent);
}

void folderSphere::process()
{
	entrySphere::process();

	if (!parent)
	{
		core::vector2di mousePos=system->getIrrlicht()->getCursorControl()->getPosition();
		core::line3df ray=system->getSceneManager()->getSceneCollisionManager()->getRayFromScreenCoordinates(mousePos);

		core::vector3df collPoint;
		core::triangle3df collTrian;
		scene::ISceneNode *collNode=system->getSceneManager()->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, collPoint, collTrian);

		system->setSelected(collNode);

		system->getVideoDriver()->draw3DTriangle(collTrian, video::SColor(255, 0, 255, 255));
	}
}

bool folderSphere::onEvent(const SEvent &event)
{
	if (entrySphere::onEvent(event))
		return true;

	if (event.EventType==EET_MOUSE_INPUT_EVENT)
	{
		if (event.MouseInput.Event==EMIE_LMOUSE_DOUBLE_CLICK)
		{
			if (selected && active)
			{
				if (opened)
				{
					closeEntry(true);
					if (system->getParent()) // cant set parent to itself
						setParent(system->getParent());
				}
				else
					system->changeParent(this);

				selected=false;
				return true;
			}
		}
	}

	return false;
}

void folderSphere::setParent(entrySphere *parent)
{
	entrySphere::setParent(parent);

	sphere->setMaterialTexture(0, system->getVideoDriver()->getTexture(system->getTexturePath()+"grey.png"));
}

folderSphere::~folderSphere()
{
}