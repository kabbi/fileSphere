

#include "CFilesystemContentProvider.h"

#include "CFileSphere.h"
#include "CFolderSphere.h"

const wchar_t *getRusName(core::stringc str,core::stringw &str2)
{
	int size=str.size();
	for (int i=0; i<size; i++)
	{
		if (str[i]>='à' && str[i]<='ÿ')
			str2[i]=str[i]+L'à'-'à';
		if (str[i]>='À' && str[i]<='ß')
			str2[i]=str[i]+L'À'-'À';
	}

	return str2.c_str();
}

CFilesystemContentProvider::CFilesystemContentProvider()
{
}

CFilesystemContentProvider::~CFilesystemContentProvider()
{
}

void CFilesystemContentProvider::updateContent(entrySphere *entry)
{
	entry->getChildren().clear();
	io::IFileSystem *fs=entry->getSystem()->getIrrlicht()->getFileSystem();

	// save cur dir to restore it later
	io::path oldDir=fs->getWorkingDirectory();
	fs->changeWorkingDirectoryTo(entry->getFullPath());

	io::IFileList *list=fs->createFileList();

	int fileCount=list->getFileCount();
	if (!list || fileCount==0) // not sure...
		return;

	for (int i=0; i<fileCount; i++)
	{
		if (list->getFileName(i)==L".." || list->getFileName(i)==L".")
			continue;

		core::stringw name(list->getFileName(i));
		core::stringw fullName(list->getFullFileName(i));
		getRusName(list->getFileName(i), name);
		getRusName(list->getFullFileName(i), fullName);
		if (list->isDirectory(i))
			entry->getChildren().push_back(new folderSphere(entry->getSystem(), fullName, name, entry));
		else
			entry->getChildren().push_back(new fileSphere(entry->getSystem(), fullName, name, entry));
	}

	entry->recalcPos();

	list->drop();
	fs->changeWorkingDirectoryTo(oldDir);
}

entrySphere *CFilesystemContentProvider::createRootNode(CSphereSystem *sys)
{
	return new folderSphere(sys, "c:/", L"root");
}