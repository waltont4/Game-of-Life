/***************************************************************************************************
 File Name:
	gobject.h

 Purpose:
	Specification file for an abstract, memory managed object base class. Defines a class called
	Gobject (game object) that will serve as the base class for all future objects that need to be
	memory managed.

 Authors:
	Igor Janjic

 Version:
	0.9 (4/17/12)
	Not done: Log error messages for collectRemainingObjects().
***************************************************************************************************/

#ifndef GOBJECT_H
#define GOBJECT_H

#include <list>
using std::list;

/***************************************************************************************************
 Class:
	Gobject

 Description:
	Serves as a base class for all derived classes that need to be memory managed. All instances of
	such objects share two statically defined lists: a list of currently live objects and dead
	objects as well as two statically defined methods for garbage collection. Each object created
	contains a count of how many things are referencing it. When the object is not needed anymore
	(nothing is referencing it), the object is added to the dead list. The dead list is flushed
	periodically. The programmer may choose to manually add and release references from gobjects or
	implement a smart pointer (smointer) system in which this is done automatically. Along with
	flushing the dead list, another method is defined for flushing the remaining objects still live.
	This method should only really be used at the end of the programs execution as a deleting live
	object that still have references to them may cause erratic run time errors. This class is part
	of the memory management system.
***************************************************************************************************/

class Gobject
{
private:

	/* A list of all gobjects currently in memory. */
	static list<Gobject*> liveGobjects;

	/* A list of all gobjects released from memory. */
	static list<Gobject*> deadGobjects;

	/* Counter for how much the gobject is being shared around. */
	long int referenceCount;

protected:

/***************************************************************************************************
 Method:
	Gobject()

 Scope:
	Protected.

 Description:
	The default constructor. Adds the newly created gobject to the list of live Gobjects.
	Initializes the number of references to the object to 0.

 Remarks:
	The default constructor is protected to prevent instantiation.
***************************************************************************************************/

	Gobject();

/***************************************************************************************************
 Method:
	virtual ~Gobject()

 Scope:
	Protected.

 Description:
	The default destructor. Left as purely virtual as derived classes are expected to implement
	their own versions.
***************************************************************************************************/

	virtual ~Gobject();

public:

/***************************************************************************************************
 Method:
	inline void addReference()

 Scope:
	Public.

 Description:
	Increments the number of references to the gobject.
***************************************************************************************************/

	void addReference();

/***************************************************************************************************
 Method:
	inline void releaseReference()

 Scope:
	Public.

 Description:
	Decrements the number of references to the gobject. If nothing is referencing it, the gobject
	is removed from the live gobjects list and moved to the dead gobjects list.
***************************************************************************************************/

	void releaseReference();

/***************************************************************************************************
 Method:
	static void collectGarbage()

 Scope:
	Public.

 Description:
	Deletes all dead gobjects and flushes the dead gobjects list.
***************************************************************************************************/

	static void collectGarbage();

/***************************************************************************************************
 Method:
	static void collectRemainingGobjects(bool emitWarnings = false)

 Scope:
	Public.

 Description:
	First collects the garbage (deletes all dead gobjects) and then deletes all live gobjects.

 Parameters:
	1.	bool warnings - Flag that triggers an error message to be logged if true.
***************************************************************************************************/

	static void collectRemainingGobjects(bool warnings);

/***************************************************************************************************
 Method:
	virtual unsigned long int size() = 0

 Scope:
	Public

 Description:
	Pure virtual function for derived classes to implement.
***************************************************************************************************/

	virtual unsigned long int size() = 0;

};

/* Quick macro that makes it easier on derived classes to return size of their gobjects. */
#define AUTO_SIZE unsigned long size() {return sizeof(*this);}

#endif
