

#ifndef __ENTRYSPHERE_H
#define __ENTRYSPHERE_H

#include "diskEntry.h"
#include <irrlicht.h>
#include <windows.h>
#include <iostream>
#include "scaleAnimator.h"
using namespace irr;

const core::vector3df sphereOffset(15, 0, 0);
const core::vector3df sphereScale(0.2f, 0.2f, 0.2f);
const core::vector3df sphereScaleDef(1, 1, 1);
const core::vector3df nullVector(0, 0, 0);
const f32 sphereRadius=10;
const u32 sphereDensity=32;
//const video::E_MATERIAL_FLAG selFlag=video::EMF_POINTCLOUD;

void moveCam(scene::ISceneManager *smgr, core::vector3df moveTo)
{
	scene::ISceneNodeAnimator *anim=smgr->createFlyStraightAnimator(smgr->getActiveCamera()->getPosition(), moveTo, 1000);
	smgr->getActiveCamera()->addAnimator(anim);
	anim->drop();
}
void updateAbsolutePosition(scene::ISceneManager *smgr, scene::ISceneNode *node)
{
	if (node->getParent()!=smgr->getRootSceneNode())
		updateAbsolutePosition(smgr, node->getParent());
	node->updateAbsolutePosition();
}

class entrySphere
{
protected:
	static entrySphere *globalParent;
	static entrySphere *globalRoot;
	static io::path texPath;

	IrrlichtDevice *device;
	scene::ISceneManager *smgr;

	diskEntry *entry;
	scene::ISceneNode *node;
	scene::ISceneNode *dummy;
	scene::IMeshSceneNode *sphere;
	scene::ITextSceneNode *label;

	entrySphere *parent;

	bool deleteNodes;
	bool selected;
	bool active;

	// create the 3D representation
	virtual void createNode()
	{
		node=smgr->addEmptySceneNode(parent?parent->getDummy():0);

		// we need to store it, because working directory will be changed...
		if (texPath.size()==0)
			texPath=device->getFileSystem()->getWorkingDirectory()+"/";

		scene::IMesh *mesh=smgr->addSphereMesh("entrySphere", sphereRadius, sphereDensity, sphereDensity);
		//smgr->getMeshManipulator()->flipSurfaces(mesh);

		sphere=smgr->addMeshSceneNode(mesh, node);
		sphere->setMaterialFlag(video::EMF_LIGHTING, false);
		sphere->setMaterialFlag(video::EMF_POINTCLOUD, true);
		setSelected(false);

		dummy=smgr->addEmptySceneNode(sphere);

		scene::ITriangleSelector *selector=smgr->createTriangleSelector(mesh, sphere);
		sphere->setTriangleSelector(selector);
		selector->drop();

		sphere->setMaterialTexture(0, device->getVideoDriver()->getTexture(texPath+"black.png"));

		label=smgr->addTextSceneNode(device->getGUIEnvironment()->getBuiltInFont(), entry->getName().c_str(), video::SColor(255, 255, 255, 255), sphere);
	}
public:
	entrySphere(diskEntry *entry, IrrlichtDevice *device, entrySphere *parent)
		: device(device), smgr(device->getSceneManager()), parent(parent),
		  selected(false), active(true), entry(entry), deleteNodes(true)
	{
		if (!globalRoot) // first-time init, we are ROOT
			globalRoot=globalParent=this;
	}

	virtual bool onEvent(const SEvent &event)
	{
		if (!selected)
			return false;

		if (event.EventType==EET_KEY_INPUT_EVENT)
		{
			switch (event.KeyInput.Key)
			{
			case KEY_LEFT:
			case KEY_RIGHT:
				{
					core::vector3df rot(0, 5.0f*(event.KeyInput.Key==KEY_LEFT?-1:1), 0);
					sphere->setRotation(sphere->getRotation()+rot);
				}
				break;
			}
		}
		if (event.EventType==EET_MOUSE_INPUT_EVENT)
		{
			if (event.MouseInput.Event==EMIE_LMOUSE_PRESSED_DOWN)
			{
				updateAbsolutePosition(smgr, sphere);
				//smgr->getActiveCamera()->setParent(sphere);
				smgr->getActiveCamera()->setTarget(sphere->getAbsolutePosition());
				//smgr->getActiveCamera()->setPosition(core::vector3df(500, 500, 500)*sphere->getAbsoluteTransformation().getScale());
				//smgr->getActiveCamera()->setRotation(nullVector);

				//smgr->getActiveCamera()->setTarget(sphere->getAbsolutePosition());
				//smgr->getActiveCamera()->setRotation(nullVector);
				//moveCam(smgr, sphere->getAbsolutePosition()+core::vector3df(30, 50, 30));
			}
		}
		return false;
	}

	virtual scene::ISceneNode *getNode()
	{
		return node;
	}

	virtual scene::ISceneNode *getSphere()
	{
		return sphere;
	}

	virtual scene::ISceneNode *getDummy()
	{
		return dummy;
	}

	virtual void setParent(entrySphere *parent)
	{
		node->setParent(parent ? parent->getDummy() : smgr->getRootSceneNode());
		sphere->setPosition(parent ? sphereOffset : nullVector);
		sphere->setScale(parent ? sphereScale : sphereScaleDef);
		sphere->setMaterialTexture(0, device->getVideoDriver()->getTexture(texPath+"black.png"));

		if (this->parent && !parent)
		{
			globalRoot=this;
			globalParent=this->parent;
			this->parent->setActive(false); // set the old parent not visible
		}

		this->parent=parent;
	}

	virtual entrySphere *getParent()
	{
		return parent;
	}

	virtual void process()
	{
		video::IVideoDriver *driver=device->getVideoDriver();
		if (!parent)
		{
			//driver->draw3DLine(core::vector3df(-10, 0, 0), core::vector3df(10, 0, 0), video::SColor(255, 255, 0, 0));
			//driver->draw3DLine(core::vector3df(0, -10, 0), core::vector3df(0, 10, 0), video::SColor(255, 0, 255, 0));
			//driver->draw3DLine(core::vector3df(0, 0, -10), core::vector3df(0, 0, 10), video::SColor(255, 0, 0, 255));
		}
		else
		{
			// draw two lines from the parent
			// ??????????????????????????????
			// how could i do so?????????????
			f32 deg=360.0f / (2.0f*core::PI*(sphereRadius+5)/(2*sphereRadius/5)) / 2; // delta angle for the sphere
			updateAbsolutePosition(smgr, sphere);
			driver->draw3DLine(parent->getSphere()->getAbsolutePosition(), sphere->getAbsolutePosition(), video::SColor(255, 0, 255, 255));

			core::vector3df vec=sphere->getAbsolutePosition()-parent->getSphere()->getAbsolutePosition();
			vec.rotateXZBy(-deg);
			driver->draw3DLine(parent->getSphere()->getAbsolutePosition(), vec*1.5f, video::SColor(255, 255, 0, 255));
			vec.rotateXZBy(deg*2);
			driver->draw3DLine(parent->getSphere()->getAbsolutePosition(), vec*1.5f, video::SColor(255, 255, 0, 255));
		}
	}

	virtual void setSelected(scene::ISceneNode *node)
	{
		setSelected(node==sphere);
	}

	virtual void setSelected(bool sel)
	{
		if (selected==sel)
			return;

		selected=sel;
		//sphere->getMaterial(0).setFlag(selFlag, !selected);
		sphere->setDebugDataVisible(selected?scene::EDS_BBOX:0);
	}
	virtual bool isSelected()
	{
		return selected;
	}

	virtual void setActive(bool active)
	{
		this->active=active;
		node->setVisible(active);
	}
	virtual bool isActive()
	{
		return active;
	}

	virtual diskEntry *getEntry()
	{
		return entry;
	}

	virtual entrySphere *getGlobalRoot()
	{
		return globalRoot;
	}
	virtual entrySphere *getGlobalParent()
	{
		return globalParent;
	}

	virtual void setDeleteNodes(bool del)
	{
		deleteNodes=del;
	}
	virtual bool areNodesDeleted()
	{
		return deleteNodes;
	}

	virtual ~entrySphere()
	{
		if (deleteNodes && node)
			node->remove();
	}
};

io::path entrySphere::texPath;
entrySphere *entrySphere::globalRoot;
entrySphere *entrySphere::globalParent;

#endif