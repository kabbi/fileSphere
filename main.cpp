


#include <irrlicht.h>
#include <ctime>
#include "driverChoice.h"
#include "CSphereSystem.h"
#include "CMainPositioner.h"
#include "CFilesystemContentProvider.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

/// Main TODO list comes here ///

/*
Better class structure (move everything from folderSphere to entrySphere)
Implement ContentProvider
Refactor IEntryPositioner
*/

IrrlichtDevice *device;
scene::ISceneManager *smgr;
gui::IGUIEnvironment *guienv;
video::IVideoDriver *driver;

video::SColor bgColor;
CSphereSystem *sys;

scene::ICameraSceneNode *freeCam, *mainCam;

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
						scene::ISceneNode *node=sys->getRoot()->getNode();
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
				case KEY_KEY_P:
					if (smgr->getActiveCamera()==freeCam)
					{
						mainCam->setPosition(freeCam->getPosition());
						mainCam->setTarget(freeCam->getTarget());
						smgr->setActiveCamera(mainCam);
					}
					else
					{
						freeCam->setPosition(mainCam->getPosition());
						freeCam->setTarget(mainCam->getTarget());
						smgr->setActiveCamera(freeCam);
					}
					break;
				case KEY_F1:
					sys->setDrawDebugData(!sys->isDrawingDebugData());
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
		if (sys)
			return sys->onEvent(event);
		return false;
	}
};

int main()
{
	srand((unsigned int)std::time(0));

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

	smgr->addSkyDomeSceneNode(driver->getTexture("ImagineWallpaper.BMP"));

	/// gui setup ///
	{
		gui::IGUIFont *font=guienv->getFont("lucida.xml");
		if (font)
			guienv->getSkin()->setFont(font);
	}
	
	freeCam=smgr->addCameraSceneNodeFPS(0, 100.0f, 0.05f);
	mainCam=smgr->addCameraSceneNodeMaya();
	mainCam->setPosition(core::vector3df(30, 30, 30));
	mainCam->setTarget(nullVector);
	mainCam->setNearValue(0.5f);
	//mainCam->setFarValue(1000);

	smgr->addLightSceneNode(mainCam);//0, mainCam->getPosition());

	// create our root sphere...

	sys=new CSphereSystem(device, new CFilesystemContentProvider(), new CMainPositioner());
	//cam->setParent(root->getSphere());

	//core::vector2di centre(driver->getScreenSize().Width/2,driver->getScreenSize().Height/2);

	// for custom drawing, reset the material
	video::SMaterial nullMaterial;
	nullMaterial.Lighting=false;

	int lastFPS=0;
	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			u32 time = device->getTimer()->getTime();

			driver->beginScene(true, true, bgColor);
			smgr->drawAll();

			driver->setMaterial(nullMaterial);
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
			sys->process();

			driver->endScene();

			if (driver->getFPS()!=lastFPS)
			{
				lastFPS=driver->getFPS();

				core::stringw str("fileSphere project ");
				str+=lastFPS;
				str+=" FPS";

				device->setWindowCaption(str.c_str());
			}
		}
	}

	device->drop();

	return 0;
}
