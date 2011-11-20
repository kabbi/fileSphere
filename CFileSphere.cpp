

#include "CFileSphere.h"

// create non-root (of course!)) node
fileSphere::fileSphere(fileEntry *entry, IrrlichtDevice *device, entrySphere *parent)
	: entrySphere(entry, device, parent)
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
				ShellExecuteW(0, NULL, entry->getFullPath().c_str(), L"", L"", SW_SHOW);
				return true;
			}
		}
	}
	return false;
}

void fileSphere::setParent(entrySphere *parent)
{
	entrySphere::setParent(parent);

	sphere->setMaterialTexture(0, device->getVideoDriver()->getTexture(texPath+"red.png"));
}

fileSphere::~fileSphere()
{
}