

#include "CFileSphere.h"

// create non-root (of course!)) node
fileSphere::fileSphere(CSphereSystem *system, io::path fullPath, core::stringw name, entrySphere *parent)
	: entrySphere(system, fullPath, name, parent)
{
	if (!parent) // we cant be non-parent...
		return;

	createNode();
	setParent(parent);
}

bool fileSphere::onEvent(const SEvent &event)
{
	if (entrySphere::onEvent(event))
		return true;
	if (event.EventType==EET_MOUSE_INPUT_EVENT)// && selected && event.MouseInput.isLeftPressed())
	{
		if (event.MouseInput.Event==EMIE_LMOUSE_DOUBLE_CLICK)
		{
			if (active && selected)
			{
				ShellExecuteW(0, NULL, core::stringw(fullPath).c_str(), L"", L"", SW_SHOW);
				return true;
			}
		}
	}
	return false;
}

void fileSphere::setParent(entrySphere *parent)
{
	entrySphere::setParent(parent);

	sphere->setMaterialTexture(0, system->getVideoDriver()->getTexture(system->getTexturePath()+"red.png"));
}

fileSphere::~fileSphere()
{
}