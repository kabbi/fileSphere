

#include "CSphereSystem.h"

#include "CFolderSphere.h"
#include "CFileSphere.h"

#include "CFixedMoveAnimator.h"
#include "CFixedScaleAnimator.h"

CSphereSystem::CSphereSystem(IrrlichtDevice *device, IContentProvider *provider, IEntryPositioner *poser)
	: device(device), positioner(poser), drawDebugData(false), provider(provider)
{
	if (!device)
		throw CSphereSystemException();

	smgr=device->getSceneManager();
	guienv=device->getGUIEnvironment();
	driver=device->getVideoDriver();

	texPath=device->getFileSystem()->getWorkingDirectory()+io::path("/");

	root=provider->createRootNode(this);
	root->openEntry(true, true);
}

CSphereSystem::~CSphereSystem()
{
	delete provider;
	delete root;

	if (positioner)
		delete positioner;
}

bool CSphereSystem::onEvent(const SEvent &event)
{
	if (root->onEvent(event))
		return true;

	return false;
}

void CSphereSystem::process()
{
	root->process();
}

void CSphereSystem::setSelected(scene::ISceneNode *node, entrySphere *entry)
{
	if (!node)
		selectedEntry=0;

	entry->setSelected(node==entry->getSphere());
	if (node==entry->getSphere())
		selectedEntry=entry;

	int size=entry->getChildren().size();
	for (int i=0; i<size; i++)
		setSelected(node, entry->getChildren()[i]);
}

void CSphereSystem::updateContent(entrySphere *entry)
{
	provider->updateContent(entry);
}

void CSphereSystem::changeParent(entrySphere *newParent)
{
	// here we animate the change of parent and make some actions (like opening new parent)
	updateAbsolutePosition(smgr, newParent->getSphere());
	core::vector3df oldPos=newParent->getSphere()->getAbsolutePosition();
	core::vector3df oldScale=newParent->getSphere()->getScale();
	
	newParent->setParent(0);
	newParent->openEntry(true);

	// restore parameters changed by setParent(0)
	newParent->getSphere()->setScale(oldScale);

	{
		updateAbsolutePosition(smgr, newParent->getSphere());
		core::vector3df newPos=oldPos-(getParent()?getParent()->getDummy()->getAbsolutePosition():nullVector);
		newParent->getSphere()->setPosition(newPos);
		
		scene::ISceneNodeAnimator *scaleAnim=scene::createFixedScaleAnimator(device, sphereScaleVec, (sphereScaleDef-sphereScaleVec)/100, sphereScaleDef);
		scene::ISceneNodeAnimator *moveAnim=scene::createFixedMoveAnimator(device, newPos, (nullVector-newPos)/100, nullVector);
		newParent->getSphere()->addAnimator(scaleAnim);
		newParent->getSphere()->addAnimator(moveAnim);
		scaleAnim->drop();
		moveAnim->drop();
	}/*
	else
	{
		updateAbsolutePosition(system->getSceneManager(), sphere);
		core::vector3df newPos=oldPos-(parent?parent->getDummy()->getAbsolutePosition():nullVector);
		sphere->setPosition(newPos);

		scene::ISceneNodeAnimator *scaleAnim=scene::createFixedScaleAnimator(system->getIrrlicht(), sphereScaleDef, (sphereScaleVec-sphereScaleDef)/100, sphereScaleVec);
		//scene::ISceneNodeAnimator *moveAnim=scene::createFixedMoveAnimator(system->getIrrlicht(), newPos, (sphereOffsetVec-newPos)/100, sphereOffsetVec);
		sphere->setPosition(nullVector);
		scene::ISceneNodeAnimator *moveAnim=scene::createFixedMoveAnimator(system->getIrrlicht(), nullVector, (newPos-nullVector)/100, newPos);
		sphere->addAnimator(scaleAnim);
		//sphere->addAnimator(moveAnim);
		scaleAnim->drop();
		moveAnim->drop();

		//sphere->setScale(sphereScaleVec);
		//sphere->setPosition(sphereOffsetVec);
	}*/
}