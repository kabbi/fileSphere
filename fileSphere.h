

#ifndef __FILE_SPHERE_H
#define __FILE_SPHERE_H

#include "entrySphere.h"

class fileSphere : public entrySphere
{
protected:
public:
	// create non-root (of course!)) node
	fileSphere(fileEntry *entry, IrrlichtDevice *device, entrySphere *parent)
		: entrySphere(entry, device, parent)
	{
		if (!parent) // we cant be non-parent...
			return;
		
		createNode();
		setParent(parent);
	}

	virtual bool onEvent(const SEvent &event)
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

	virtual void setParent(entrySphere *parent)
	{
		entrySphere::setParent(parent);

		sphere->setMaterialTexture(0, device->getVideoDriver()->getTexture(texPath+"red.png"));
	}

	virtual ~fileSphere()
	{
	}
};

#endif