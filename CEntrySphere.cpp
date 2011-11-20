

#include "CEntrySphere.h"

/// Some helper functions, must be refactored ///
void createMegaSCaleAnimator(IrrlichtDevice *device, scene::ISceneNode *node, core::vector3df newScale, u32 time)
{
	scene::ISceneManager *smgr=device->getSceneManager();

	updateAbsolutePosition(smgr, node);
	core::vector3df oldChildScale=node->getAbsoluteTransformation().getScale();
	core::vector3df relChildScale=newScale/oldChildScale;
	core::vector3df oldChildPos(node->getAbsolutePosition());

	// find the most parent parent
	scene::ISceneNode *parent=node;
	while (parent->getParent() && parent->getParent()!=smgr->getRootSceneNode())
		parent=parent->getParent();

	// find the new position
	parent->setScale(parent->getScale()*relChildScale); // scale the whole system and calc the new node's position
	updateAbsolutePosition(smgr, node);
	core::vector3df newChildPos(node->getAbsolutePosition());
	parent->setScale(parent->getScale()/relChildScale); // return all the things to the old state
	core::vector3df deltaPos(newChildPos-oldChildPos);

	// create two animators: move and scale
	scene::ISceneNodeAnimator *moveAnim=smgr->createFlyStraightAnimator(parent->getPosition(), parent->getPosition()-deltaPos, time);
	scene::ISceneNodeAnimator *scaleAnim=scene::createFixedScaleAnimator(device, oldChildScale, (newScale-oldChildScale)/(f32)time/1000.0f, newScale);
	parent->addAnimator(moveAnim);
	parent->addAnimator(scaleAnim);
	moveAnim->drop();
	scaleAnim->drop();
}
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
int sgn(int x)
{
	if (x==0)
		return 0;
	return (x>0)?1:-1;
}

/// The class functions, entrySphere ///

entrySphere::entrySphere(diskEntry *entry, IrrlichtDevice *device, entrySphere *parent) // selected is set true, to allow the constructor to set it false and thus to make function work (it checks the equality of the new state and the old one)
		: device(device), smgr(device->getSceneManager()), parent(parent),
		  selected(true), active(true), entry(entry), deleteNodes(true)
{
	if (!globalRoot) // first-time init, we are ROOT
	{
		globalRoot=globalParent=this;
		// we need to store it, because working directory will be changed...
		texPath=device->getFileSystem()->getWorkingDirectory()+io::path("/");
		drawDebugData=false;
		positioner=0;
	}
}

// create the 3D representation
void entrySphere::createNode()
{
	// node is our center, not really used now, but allows creation of additional animation/transformation layer
	node=smgr->addEmptySceneNode(parent?parent->getDummy():0);

	scene::IMesh *mesh=smgr->addSphereMesh("entrySphere", sphereRadius, sphereDensity, sphereDensity);
	//smgr->getMeshManipulator()->flipSurfaces(mesh);

	// sphere is out main visual representation
	sphere=smgr->addMeshSceneNode(mesh, node);
	//sphere->setMaterialFlag(video::EMF_LIGHTING, false);
	//sphere->setMaterialFlag(video::EMF_POINTCLOUD, true);
	// deault one, is often overided by childrens' createNode function
	sphere->setMaterialTexture(0, device->getVideoDriver()->getTexture(texPath+"black.png"));

	// dummy is used to animate children without touching this node
	dummy=smgr->addEmptySceneNode(sphere);

	// selector is for use of selection (ray intersection) functions
	scene::ITriangleSelector *selector=smgr->createTriangleSelector(mesh, sphere);
	sphere->setTriangleSelector(selector);
	selector->drop();

	// this node displays the name of our sphere
	label=smgr->addTextSceneNode(device->getGUIEnvironment()->getBuiltInFont(), entry->getName().c_str(), video::SColor(255, 0, 0, 0), sphere);

	setSelected(false);
}

bool entrySphere::onEvent(const SEvent &event)
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

void entrySphere::setParent(entrySphere *parent)
{
	node->setParent(parent ? parent->getDummy() : smgr->getRootSceneNode());
	sphere->setPosition(parent ? sphereOffsetVec : nullVector);
	sphere->setScale(parent ? sphereScaleVec : sphereScaleDef);
	sphere->setMaterialTexture(0, device->getVideoDriver()->getTexture(texPath+"black.png"));

	if (this->parent && !parent)
	{
		globalRoot=this;
		globalParent=this->parent;
		this->parent->setActive(false); // set the old parent not visible
	}

	this->parent=parent;
}

void entrySphere::process()
{
	video::IVideoDriver *driver=device->getVideoDriver();
	if (drawDebugData)
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
			updateAbsolutePosition(smgr, sphere);
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
	float dist=label->getAbsolutePosition().getDistanceFrom(smgr->getActiveCamera()->getAbsolutePosition());
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
	this->active=active;
	node->setVisible(active);
}

entrySphere::~entrySphere()
{
	if (deleteNodes && node)
		node->remove();
}

// declare class's statics
io::path entrySphere::texPath;
entrySphere *entrySphere::globalRoot;
entrySphere *entrySphere::globalParent;
bool entrySphere::drawDebugData;
IEntryPositioner *entrySphere::positioner;