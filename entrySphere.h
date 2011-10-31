

#ifndef __ENTRYSPHERE_H
#define __ENTRYSPHERE_H

#include "diskEntry.h"
#include <irrlicht.h>
#include <windows.h>
#include "scaleAnimator.h"
using namespace irr;

const core::vector3df sphereOffset(10, 0, 0);
const core::vector3df sphereScale(0.2f, 0.2f, 0.2f);
const core::vector3df sphereScaleDef(1, 1, 1);
const core::vector3df nullVector(0, 0, 0);
const int sphereRadius=10;
const int sphereDensity=32;
//const video::E_MATERIAL_FLAG selFlag=video::EMF_POINTCLOUD;

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
				sphere->setRotation(sphere->getRotation()+core::vector3df(0, (event.KeyInput.Key==KEY_LEFT?-5:5), 0));
				break;
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
		if (active)// && parent) // !parent means root
		{
			core::vector2di mousePos=device->getCursorControl()->getPosition();
			core::line3df ray=smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(mousePos);

			core::vector3df collPoint;
			core::triangle3df collTrian;
			scene::ISceneNode *collNode=smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, collPoint, collTrian);

			setSelected(collNode==sphere);
		}
	}

	virtual void setSelected(bool sel)
	{
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