/*******************************************
	Messenger.cpp

	Entity messenger class implementation
********************************************/

#include "Messenger.h"

namespace malaz
{

/////////////////////////////////////
// Global variables

// Define a single messenger object for the program
CMessenger Messenger;


/////////////////////////////////////
// Message sending/receiving

// Send the given message to a particular UID, does not check if the UID exists
void CMessenger::SendMessage( EntityNameUID to, CMsg* msg )
{
	// Simply insert the UID/message pair into the message map. It will be inserted next
	// to any other pairs with the same UID
	m_Messages.insert( UIDMsgPair( to, msg ) );
}


// Fetch the next available message for the given UID, returns the message through the given 
// pointer. Returns false if there are no messages for this UID
CMsg* CMessenger::FetchMessage( EntityNameUID to )
{
	// Find the first message for this UID in the message map
	TMessageIter itMessage = m_Messages.find( to );

	// See if no messages for this UID
	if (itMessage == m_Messages.end())
	{
		return NULL;
	}

	// Return message, then delete it
	CMsg* msg = itMessage->second;//->second;
	m_Messages.erase( itMessage );

	return msg;
}



} // namespace gen
