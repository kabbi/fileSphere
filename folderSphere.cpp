

#include "folderSphere.h"

void folderSphere::recalcPos()
{
	int size=children.size();

	if (!parent)
	{
		// lots of mathematical functions...
		float distance=sphereOffset.X;
		int numSegments=2.0f*core::PI*distance/(10*2/5);
		float step=360.f/numSegments;
		int curChild=0;
		int maxLevel=distance*2/(10*2/5);

		// 0 - center, +1 - above it, -1 - below it etc...
		int level=0;
		int levelInc=1;
		while (abs(level)<maxLevel)
		{
			// and here again, recount number of entries fitting on each level, and position them
			float curDistance=distance*sin(core::PI/2-core::degToRad(step*level));

			int curNumSegments=2.0f*core::PI*curDistance/(10*2/5);
			curNumSegments=core::min_(curNumSegments, size-curChild);

			float curStep=360.f/curNumSegments;

			for (int i=0; i<curNumSegments; i++)
			{
				// spherical coordinates transform
				float fi=core::degToRad(curStep*i);
				float si=core::PI/2-core::degToRad(step*level);
				float r=distance;
				children[curChild++]->getSphere()->setPosition(core::vector3df(r*sin(si)*cos(fi), r*cos(si), r*sin(si)*sin(fi)));
			}

			level+=levelInc;
			levelInc=-(levelInc+sgn(levelInc)*1); // why have I written 1 here??
		}
	}
	else
	{
		// get current scale, recursive
		updateAbsolutePosition(smgr, sphere);
		f32 curScale=sphere->getAbsoluteTransformation().getScale().X;
		f32 curRadius=sphereOffset.X*curScale;
		f32 childDiameter=sphereRadius*curScale*sphereScale.X*2;

		f32 startAngle, endAngle; // by default, half of the sphere
		startAngle=core::degToRad(sphere->getPosition().getHorizontalAngle().X-90)+core::PI/4;
		endAngle=startAngle+core::PI/2;

		int numLevels = (maxLevel-1)/2+1; // number of levels in one direction
		int *numEntries = new int[numLevels]; // it will contain max nuber of spheres on each level

		f32 circleLength=2.0f*core::PI*curRadius / (2*core::PI) * abs(endAngle-startAngle);

		numEntries[0]=core::min_(circleLength/childDiameter, (f32)size);
		//numEntries[1]=numEntries[0];

		f32 angleStep=(endAngle-startAngle)/numEntries[0];
		angleStep=core::radToDeg(angleStep);

		// 0 - center, +1 - above it, -1 - below it etc...
		int level=0;
		int levelInc=1;
		int curChild=0;
		while (abs(level)<numLevels && curChild<size)
		{
			f32 curAngle=core::radToDeg(startAngle)+angleStep/2;

			int curNumEntries=numEntries[abs(level)];
			for (int j=0; j<curNumEntries && curChild<size; j++)
			{
				children[curChild++]->getNode()->setRotation(core::vector3df(0, curAngle, 0));
				curAngle+=angleStep;
			}
			level+=levelInc;
			levelInc=-(levelInc+sgn(levelInc)*1);
		}
		delete[] numEntries;
	}
}

void folderSphere::scanFolder()
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


// create the root node and scan and open
folderSphere::folderSphere(folderEntry *entry, IrrlichtDevice *device)
	: entrySphere(entry, device, 0)
{
	createNode();
	setParent(parent);
}

// create non-root node
folderSphere::folderSphere(folderEntry *entry, IrrlichtDevice *device, entrySphere *parent)
	: entrySphere(entry, device, parent)
{
	createNode();
	setParent(parent);
}

void folderSphere::openFolder()
{
	scanFolder();

	dummy->setScale(nullVector);
	scene::ISceneNodeAnimator *anim=scene::createFixedScaleAnimator(device, nullVector, sphereScaleDef/100, sphereScaleDef);
	dummy->addAnimator(anim);
	anim->drop();

	if (children.size())
		sphere->setMaterialFlag(video::EMF_POINTCLOUD, true);
}
void folderSphere::closeFolder()
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

	sphere->setMaterialFlag(video::EMF_POINTCLOUD, false);
}

// starting from here, we should advice almost all entrySphere's functions to every children

bool folderSphere::onEvent(const SEvent &event)
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
		if (event.KeyInput.Key==KEY_KEY_W && event.KeyInput.PressedDown && selected)
		{
			fileEntry *subEntry=new fileEntry(core::stringw(L"debug"), 1, core::stringw(L"veryDebug"));
			children.push_back(new fileSphere(subEntry, device, this));
			recalcPos();
		}
		if (event.KeyInput.Key==KEY_KEY_Q && event.KeyInput.PressedDown && selected)
		{
			folderEntry *subEntry=new folderEntry(core::stringw(L"debugFolder"), core::stringw(L"veryDebug"));
			children.push_back(new folderSphere(subEntry, device, this));
			recalcPos();
		}
	}
	return false;
}

void folderSphere::process()
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

void folderSphere::setSelected(scene::ISceneNode *node)
{
	entrySphere::setSelected(node);

	for (u32 i=0; i<children.size(); i++)
		children[i]->setSelected(node);
}

void folderSphere::setDrawDebugData(bool flag)
{
	entrySphere::setDrawDebugData(flag);
		
	for (u32 i=0; i<children.size(); i++)
		children[i]->setDrawDebugData(flag);
}

void folderSphere::setParent(entrySphere *parent)
{
	entrySphere *oldParent=this->parent;
	entrySphere::setParent(parent);

	// if we are new root
	if (!parent)
		openFolder();

	sphere->setMaterialTexture(0, device->getVideoDriver()->getTexture(texPath+"grey.png"));
}

folderSphere::~folderSphere()
{
	if (children.size())
		closeFolder();
}