

#ifndef __FOLDER_SPHERE_H
#define __FOLDER_SPHERE_H

#include "entrySphere.h"
#include "fileSphere.h"

const int maxLevel=3;

class folderSphere : public entrySphere
{
protected:

	core::array<entrySphere*> children;

	// recalc the position of all subnodes
	void recalcPos()
	{
		int size=children.size();

		if (!parent)
		{
			// count folders...
			int folderSize=0;
			for (int i=0; i<size; i++)
				if (children[i]->getEntry()->getType()==FOLDER_ENTRY)
					folderSize++;

			int numSegments=2.0f*core::PI*(sphereRadius+5)/(10*2/5);
			numSegments=core::min_(numSegments, size);
			float step=360.f/numSegments;
			int folderStep=folderSize?(numSegments/folderSize):0;
			for (int i=0, folderCnt=0, fileCnt=folderSize; i<numSegments; i++)
			{
				if (folderCnt<folderSize && i%folderStep==0)
					children[folderCnt++]->getNode()->setRotation(core::vector3df(0, step*i, 0));
				else if (fileCnt<size)
					children[fileCnt++]->getNode()->setRotation(core::vector3df(0, step*i, 0));
			}
		}

		f32 startAngle=0, endAngle=core::PI; // by default, half of the sphere


		int numLevels = (maxLevel-1)/2+1; // number of levels in one direction
		int *numEntries = new int[numLevels]; // it will contain max nuber of spheres on each level
		numEntries[0]=2.0f*core::PI*sphereRadius / (2*core::PI) * abs(endAngle-startAngle);
		// 0 - center, 1 - above it, 2 - below it, 3 - above 1, 4 - below 2 etc...
		for (int level=0; level<maxLevel; level++)
		{

		}
		delete[] numEntries;
	}

	void scanFolder()
	{
		io::IFileSystem *fs=device->getFileSystem();
		// save cur dir to restore it later
		io::path oldDir=fs->getWorkingDirectory();
		fs->changeWorkingDirectoryTo(io::path(entry->getFullPath()).c_str());

		io::IFileList *list=fs->createFileList();

		int fileCount=list->getFileCount();
		if (!list || fileCount==0) // not sure...
			return;

		for (int i=0; i<fileCount; i++)
		{
			if (list->getFileName(i)==L".." || list->getFileName(i)==L".")
				continue;

			core::stringw name(list->getFileName(i));
			if (list->isDirectory(i))
			{
				folderEntry *subEntry=new folderEntry(name, core::stringw(list->getFullFileName(i)));
				dynamic_cast<folderEntry*>(entry)->getSubItems().push_back(subEntry); // populate subfolders list
				children.push_back(new folderSphere(subEntry, device, this));
			}
			else
			{
				fileEntry *subEntry=new fileEntry(name, list->getFileSize(i), core::stringw(list->getFullFileName(i)));
				children.push_back(new fileSphere(subEntry, device, this));
			}
		}

		recalcPos();

		list->drop();
		fs->changeWorkingDirectoryTo(oldDir);
	}

public:
	// create the root node and scan and open
	folderSphere(folderEntry *entry, IrrlichtDevice *device)
		: entrySphere(entry, device, 0)
	{
		createNode();
		setParent(parent);
	}

	// create non-root node
	folderSphere(folderEntry *entry, IrrlichtDevice *device, entrySphere *parent)
		: entrySphere(entry, device, parent)
	{
		createNode();
		setParent(parent);
	}

	virtual void openFolder()
	{
		scanFolder();

		dummy->setScale(nullVector);
		scene::ISceneNodeAnimator *anim=scene::createFixedScaleAnimator(device, nullVector, sphereScaleDef/100, sphereScaleDef);
		dummy->addAnimator(anim);
		anim->drop();
	}
	virtual void closeFolder()
	{
		// animate deletion
		scene::ISceneNodeAnimator *scaleAnim=scene::createFixedScaleAnimator(device, sphereScaleDef, -sphereScaleDef/100, nullVector);
		dummy->addAnimator(scaleAnim);
		scaleAnim->drop();

		// and this would delete everything else
		for (u32 i=0; i<children.size(); i++)
		{
			// we need to animate them, so...
			children[i]->setDeleteNodes(false);

			scene::ISceneNodeAnimator *deleteAnim=smgr->createDeleteAnimator(1000);
			children[i]->getNode()->addAnimator(deleteAnim);
			deleteAnim->drop();

			delete children[i];
		}
		children.clear();
	}

	virtual bool onEvent(const SEvent &event)
	{
		if (entrySphere::onEvent(event))
			return true;

		for (u32 i=0; i<children.size(); i++)
			if (children[i]->onEvent(event))
				return true;

		if (event.EventType==EET_MOUSE_INPUT_EVENT)
		{
			if (event.MouseInput.Event==EMIE_LMOUSE_DOUBLE_CLICK)
			{
				if (selected && active)
				{
					setSelected(false);

					if (children.size())
						closeFolder();
					else
						openFolder();

					return true;
				}
			}
		}

		if (event.EventType==EET_KEY_INPUT_EVENT)
		{
			if (event.KeyInput.Key==KEY_KEY_D && event.KeyInput.PressedDown && selected)
			{
				fileEntry *subEntry=new fileEntry(core::stringw(L"debug"), 1, core::stringw(L"veryDebug"));
				children.push_back(new fileSphere(subEntry, device, this));
				recalcPos();
			}
		}
		return false;
	}

	virtual void process()
	{
		entrySphere::process();

		for (u32 i=0; i<children.size(); i++)
			children[i]->process();

		if (!parent)
		{
			core::vector2di mousePos=device->getCursorControl()->getPosition();
			core::line3df ray=smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(mousePos);

			core::vector3df collPoint;
			core::triangle3df collTrian;
			scene::ISceneNode *collNode=smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, collPoint, collTrian);

			setSelected(collNode);

			device->getVideoDriver()->draw3DTriangle(collTrian, video::SColor(255, 0, 255, 255));
		}
	}

	virtual void setSelected(scene::ISceneNode *node)
	{
		entrySphere::setSelected(node);

		for (u32 i=0; i<children.size(); i++)
			children[i]->setSelected(node);
	}

	virtual void setParent(entrySphere *parent)
	{
		entrySphere *oldParent=this->parent;
		entrySphere::setParent(parent);

		// if we are new root
		if (!parent)
			scanFolder();

		sphere->setMaterialTexture(0, device->getVideoDriver()->getTexture(texPath+"grey.png"));
	}

	// return the first parent's parent, and set it as parent
	virtual entrySphere *stepUp(int level=0)
	{
		return 0;
	}

	virtual ~folderSphere()
	{
		if (children.size())
			closeFolder();
		//for (int i=0; i<children.size(); i++)
		//	delete children[i];
	}
};

#endif