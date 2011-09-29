/*******************************************
	Messenger.h

	Entity messenger class definitions
********************************************/

#pragma once

#include <map>
#include "Defines.h"
#include "TankDefines.h"
#include "CVector3.h"
#include "CMatrix4x4.h"
#include "Entity.h"
#include "MessageTypes.h"
using namespace gen;

#undef SENDMESSAGE

namespace malaz
{

class CEntity;

// Messenger class allows the sending and receipt of messages between entities - addressed
// by UID
class CMessenger
{
/////////////////////////////////////
//	Constructors/Destructors
public:
	// Default constructor
	CMessenger() {}

	// No destructor needed

private:
	// Disallow use of copy constructor and assignment operator (private and not defined)
	CMessenger( const CMessenger& );
	CMessenger& operator=( const CMessenger& );


/////////////////////////////////////
//	Public interface
public:

	/////////////////////////////////////
	// Message sending/receiving

	// Send the given message to a particular UID, does not check if the UID exists
	void SendMessage( EntityNameUID to, CMsg* msg );

	// Fetch the next available message for the given UID, returns the message through the given 
	// pointer. Returns false if there are no messages for this UID
	CMsg* FetchMessage( EntityNameUID to );


/////////////////////////////////////
//	Private interface
private:

	// A multimap has properties similar to a hash map - mapping a key to a value. Here we
	// have the key as an entity UID and the value as a message for that UID. The stored
	// key/value pairs in a multimap are sorted by key, which means all the messages for a
	// particular UID are together. Key look-up is somewhat slower than for a hash map though
	// Define some types to make usage easier
	typedef multimap<EntityNameUID, CMsg*> TMessages;
	typedef TMessages::iterator TMessageIter;
    typedef pair<EntityNameUID, CMsg*> UIDMsgPair; // The type stored by the multimap

	TMessages m_Messages;
};


} // namespace gen
