

#include "CEntrySphere.h"

#include "CFixedMoveAnimator.h"
#include "CFixedScaleAnimator.h"

/// Some helper functions, must be refactored ///
void updateAbsolutePosition(scene::ISceneManager *smgr, scene::ISceneNode *node)
{
	if (node->getParent()!=smgr->getRootSceneNode())
		updateAbsolutePosition(smgr, node->getParent());
	node->updateAbsolutePosition();
}

/// The class functions, entrySphere ///

entrySphere::entrySphere(CSphereSystem *system, io::path path, core::stringw name, entrySphere *parent) // selected is set true, to allow the constructor to set it false and thus to make function work (it checks the equality of the new state and the old one)
		: system(system), parent(parent), fullPath(path), name(name),
		  selected(true), active(true), deleteNodes(true), opened(false)
{
}

// create the 3D representation
void entrySphere::createNode()
{
	// node is our center, not really used now, but allows creation of additional animation/transformation layer
	node=system->getSceneManager()->addEmptySceneNode(parent?parent->getDummy():0);

	scene::IMesh *mesh=system->getSceneManager()->addSphereMesh("entrySphere", sphereRadius, sphereDensity, sphereDensity);
	//smgr->getMeshManipulator()->flipSurfaces(mesh);

	// sphere is out main visual representation
	sphere=system->getSceneManager()->addMeshSceneNode(mesh, node);
	//sphere->setMaterialFlag(video::EMF_LIGHTING, false);
	//sphere->setMaterialFlag(video::EMF_POINTCLOUD, true);
	// deault one, is often overided by childrens' createNode function
	sphere->setMaterialTexture(0, system->getVideoDriver()->getTexture(system->getTexturePath()+"black.png"));

	// dummy is used to animate children without touching this node
	dummy=system->getSceneManager()->addEmptySceneNode(sphere);

	// selector is for selection (ray intersection) purpose
	scene::ITriangleSelector *selector=system->getSceneManager()->createTriangleSelector(mesh, sphere);
	sphere->setTriangleSelector(selector);
	selector->drop();

	// this node displays the name of our sphere
	label=system->getSceneManager()->addTextSceneNode(system->getGUIEnvironment()->getFont("lucida.xml"), name.c_str(), video::SColor(255, 0, 0, 0), sphere);

	setSelected(false);
}

void entrySphere::recalcPos()
{
	if (system->getPositioner())
		system->getPositioner()->recalcPositions(system->getSceneManager(), this, children); // TODO! fix
}

void entrySphere::openEntry(bool updateContent, bool animate)
{
	opened=true;
	if (updateContent)
		system->updateContent(this);

	if (animate)
	{
		dummy->setScale(nullVector);
		scene::ISceneNodeAnimator *anim=scene::createFixedScaleAnimator(system->getIrrlicht(), nullVector, sphereScaleDef/100, sphereScaleDef);
		dummy->addAnimator(anim);
		anim->drop();
	}

	//if (children.size())
	//	sphere->setMaterialFlag(video::EMF_POINTCLOUD, true);
}
void entrySphere::closeEntry(bool clear, bool animate)
{
	opened=false;

	if (animate)
	{
		// animate deletion
		scene::ISceneNodeAnimator *scaleAnim=scene::createFixedScaleAnimator(system->getIrrlicht(), sphereScaleDef, -sphereScaleDef/100, nullVector);
		dummy->addAnimator(scaleAnim);
		scaleAnim->drop();
	}

	if (!clear)
		return;

	// and this would delete everything else
	for (u32 i=0; i<children.size(); i++)
	{
		if (animate)
		{
			// we need to animate them, so...
			children[i]->setDeleteNodes(false);

			scene::ISceneNodeAnimator *deleteAnim=system->getSceneManager()->createDeleteAnimator(1000);
			children[i]->getNode()->addAnimator(deleteAnim);
			deleteAnim->drop();
		}

		delete children[i];
	}
	children.clear();

	//sphere->setMaterialFlag(video::EMF_POINTCLOUD, false);
}

void entrySphere::updateContent(IContentProvider *provider)
{
	provider->updateContent(this);
}

bool entrySphere::onEvent(const SEvent &event)
{
	for (u32 i=0; i<children.size(); i++)
		if (children[i]->onEvent(event))
			return true;

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
	return false;
}

// also updates lots of 3d position and animation stuff
void entrySphere::setParent(entrySphere *parent)
{
	updateAbsolutePosition(system->getSceneManager(), sphere);
	core::vector3df oldPos=sphere->getAbsolutePosition();

	node->setParent(parent ? parent->getDummy() : system->getSceneManager()->getRootSceneNode());
	
	sphere->setRotation(nullVector);

	sphere->setPosition(parent ? sphereOffsetVec : nullVector);
	sphere->setScale(parent ? sphereScaleVec : sphereScaleDef);

	if (this->parent && !parent)
	{
		system->getFolderHistory().push_back(this);
		this->parent->setActive(false); // set the old parent not visible
		setActive(true);
	}
	else if (!this->parent && parent)
	{
		system->getFolderHistory().erase(system->getFolderHistory().size()-1);
		parent->setActive(true);
	}

	this->parent=parent;
}

void entrySphere::process()
{
	for (u32 i=0; i<children.size(); i++)
		children[i]->process();

	video::IVideoDriver *driver=system->getVideoDriver();
	if (system->isDrawingDebugData())
	{
		if (!parent)
		{
			driver->draw3DLine(core::vector3df(-10, 0, 0), core::vector3df(10, 0, 0), video::SColor(255, 255, 0, 0));
			driver->draw3DLine(core::vector3df(0, -10, 0), core::vector3df(0, 10, 0), video::SColor(255, 0, 255, 0));
			driver->draw3DLine(core::vector3df(0, 0, -10), core::vector3df(0, 0, 10), video::SColor(255, 0, 0, 255));
		}
		else
		{
			// draw two lines from the parent
			f32 deg=360.0f / (2.0f*core::PI*(sphereRadius+5)/(2*sphereRadius/5)) / 2; // delta angle for the sphere
			updateAbsolutePosition(system->getSceneManager(), sphere);
			driver->draw3DLine(parent->getSphere()->getAbsolutePosition(), sphere->getAbsolutePosition(), video::SColor(255, 0, 255, 255));

			core::vector3df vec=sphere->getAbsolutePosition()-parent->getSphere()->getAbsolutePosition();

			vec.rotateXZBy(-deg);
			driver->draw3DLine(parent->getSphere()->getAbsolutePosition(),
				parent->getSphere()->getAbsolutePosition()+vec*1.5f, video::SColor(255, 255, 0, 255));

			vec.rotateXZBy(deg*2);
			driver->draw3DLine(parent->getSphere()->getAbsolutePosition(),
				parent->getSphere()->getAbsolutePosition()+ vec*1.5f, video::SColor(255, 255, 0, 255));
		}
	}
		// show or hide our text node based on the distance from the cam
	updateAbsolutePosition(system->getSceneManager(), label);
	float dist=label->getAbsolutePosition().getDistanceFrom(system->getSceneManager()->getActiveCamera()->getAbsolutePosition());
	label->setVisible(dist<30);
}

void entrySphere::setSelected(bool sel)
{
	if (selected==sel)
		return;

	selected=sel;
	//sphere->getMaterial(0).setFlag(selFlag, !selected);
	sphere->setDebugDataVisible(selected?scene::EDS_BBOX:0);
	//sphere->getMaterial(0).MaterialType=selected?video::EMT_SOLID:video::EMT_TRANSPARENT_ADD_COLOR;
}

void entrySphere::setActive(bool active)
{
	for (u32 i=0; i<children.size(); i++)
		children[i]->setActive(active);

	this->active=active;
	node->setVisible(active);
	// to avoid the collision detection of invisible sphere,
	// we should explicitly call it here
	sphere->setVisible(active);
}

entrySphere::~entrySphere()
{
	if (deleteNodes && node)
		node->remove();
	
	if (children.size())
		closeEntry();
}