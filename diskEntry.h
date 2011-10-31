

#ifndef __FILEENTRY_H
#define __FILEENTRY_H

#include <irrlicht.h>
using namespace irr;


// not so many right now...
enum ENTRY_TYPE
{
	UNKNOWN_ENTRY,
	FILE_ENTRY,
	FOLDER_ENTRY
};



class diskEntry
{
protected:
	long long size;
	core::stringw name;
	core::stringw fullPath;
public:
	// empty entry
	diskEntry()
		: name(), size(0), fullPath()
	{
	}

	// usual disk entry
	diskEntry(core::stringw &name, long long size, core::stringw &fullPath)
		: name(name), size(size), fullPath(fullPath)
	{
	}

	// get the entry name
	virtual const core::stringw &getName()
	{
		return name;
	}

	// get the full path
	virtual const core::stringw &getFullPath()
	{
		return fullPath;
	}

	// get the size
	virtual long long getSize()
	{
		return size;
	}

	// get entry type (folder, file etc...)
	virtual ENTRY_TYPE getType()
	{
		return UNKNOWN_ENTRY;
	}

	~diskEntry()
	{
	}
};



class fileEntry : public diskEntry
{
private:
	core::array<fileEntry*> subItems;
public:
	fileEntry()
	{
	}

	fileEntry(core::stringw &name, long long size, core::stringw &fullPath)
		: diskEntry(name, size, fullPath)
	{
	}

	virtual ENTRY_TYPE getType()
	{
		return FILE_ENTRY;
	}

	virtual ~fileEntry()
	{
		// i will write something here some time...
	}
};



class folderEntry : public diskEntry
{
protected:
	core::array<diskEntry*> subItems;
public:
	folderEntry()
	{
	}

	folderEntry(core::stringw &name, core::stringw &fullPath)
		: diskEntry(name, 0, fullPath)
	{
	}

	virtual core::array<diskEntry*> &getSubItems()
	{
		return subItems;
	}

	virtual ENTRY_TYPE getType()
	{
		return FOLDER_ENTRY;
	}

	virtual long long getSize()
	{
		// recalc...
		size=0;
		for (int i=0; i<subItems.size(); i++)
			size+=subItems[i]->getSize();
		return size;
	}

	virtual ~folderEntry()
	{
		int size=subItems.size();
		for (int i=0; i<size; i++)
			delete subItems[i];
	}
};

class folderScanner
{
};

#endif