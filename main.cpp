


#include <irrlicht.h>
#include <ctime>
#include "driverChoice.h"
#include "fileSphere.h"
#include "folderSphere.h"
#include "diskEntry.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

IrrlichtDevice *device;
scene::ISceneManager *smgr;
gui::IGUIEnvironment *guienv;
video::IVideoDriver *driver;

video::SColor bgColor;
folderSphere *root;

scene::ICameraSceneNode *freeCam, *mayaCam;

class myReceiver : public IEventReceiver
{
private:
public:
	myReceiver()
	{
	}
	bool OnEvent(const SEvent& event)
	{
		if (event.EventType==EET_KEY_INPUT_EVENT)
		{
			if (event.KeyInput.PressedDown)
			{
				switch (event.KeyInput.Key)
				{
				case KEY_RETURN:
					{
						scene::ISceneNode *node=root->getGlobalRoot()->getNode();
						if (node->getAnimators().getSize())
							node->removeAnimators();
						else
						{
							scene::ISceneNodeAnimator *anim=smgr->createRotationAnimator(core::vector3df(0, 0.1f, 0));
							node->addAnimator(anim);
							anim->drop();
						}
					}
					break;
				case KEY_ESCAPE:
					device->closeDevice();
					break;
				case KEY_SPACE:
					if (smgr->getActiveCamera()==freeCam)
					{
						mayaCam->setPosition(freeCam->getPosition());
						mayaCam->setTarget(freeCam->getTarget());
						smgr->setActiveCamera(mayaCam);
					}
					else
					{
						freeCam->setPosition(mayaCam->getPosition());
						freeCam->setTarget(mayaCam->getTarget());
						smgr->setActiveCamera(freeCam);
					}
					break;
				case KEY_BACK:
					if (bgColor.getLuminance()==0)
						bgColor.set(255, 255, 255, 255);
					else
						bgColor.set(255, 0, 0, 0);
					break;
				}
			}
		}
		if (root)
			return root->onEvent(event);
		return false;
	}
};

int main()
{
	srand(std::time(0));

	video::E_DRIVER_TYPE driverType=video::EDT_DIRECT3D9;//driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	myReceiver recv;
	device = createDevice(driverType,
		core::dimension2d<u32>(800, 600), 32, false, true, true, &recv);

	if (device == 0)
		return 1; // could not create selected driver.

	device->setWindowCaption(L"FileSystem Visualizer");

	driver=device->getVideoDriver();
	smgr=device->getSceneManager();
	guienv=device->getGUIEnvironment();

	device->getFileSystem()->changeWorkingDirectoryTo("data");
	
	freeCam=smgr->addCameraSceneNodeFPS(0, 100.0f, 0.05f);
	mayaCam=smgr->addCameraSceneNodeMaya();
	mayaCam->setPosition(core::vector3df(30, 30, 30));
	mayaCam->setTarget(core::vector3df(0, 0, 0));

	// create our root sphere...
	root=new folderSphere(new folderEntry(core::stringw(L"root"), core::stringw(L"c:/tmp/")), device);
	//cam->setParent(root->getSphere());

	//core::vector2di centre(driver->getScreenSize().Width/2,driver->getScreenSize().Height/2);

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			u32 time = device->getTimer()->getTime();

			driver->beginScene(true, true, bgColor);
			smgr->drawAll();
			root->process();
			driver->endScene();
		}
	}

	delete root;
	device->drop();

	return 0;
}
