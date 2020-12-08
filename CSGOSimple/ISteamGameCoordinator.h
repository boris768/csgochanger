//====== Copyright ©, Valve Corporation, All rights reserved. =======
//
// Purpose: interface to the game coordinator for this application
//
//=============================================================================

#pragma once
#include "ISteamClient.h"

// list of possible return values from the ISteamGameCoordinator API
enum EGCResults
{
    k_EGCResultOK = 0,
    k_EGCResultNoMessage = 1,			// There is no message in the queue
    k_EGCResultBufferTooSmall = 2,		// The buffer is too small for the requested message
    k_EGCResultNotLoggedOn = 3,			// The client is not logged onto Steam
    k_EGCResultInvalidMessage = 4,		// Something was wrong with the message being sent with SendMessage
};


//-----------------------------------------------------------------------------
// Purpose: Functions for sending and receiving messages from the Game Coordinator
//			for this application
//-----------------------------------------------------------------------------
class ISteamGameCoordinator
{
public:

    // sends a message to the Game Coordinator
    virtual EGCResults SteamSendMessage(__int32 unMsgType, const void *pubData, __int32 cubData) = 0;

    // returns true if there is a message waiting from the game coordinator
    virtual bool IsMessageAvailable(__int32 *pcubMsgSize) = 0;

    // fills the provided buffer with the first message in the queue and returns k_EGCResultOK or 
    // returns k_EGCResultNoMessage if there is no message waiting. pcubMsgSize is filled with the message size.
    // If the provided buffer is not large enough to fit the entire message, k_EGCResultBufferTooSmall is returned
    // and the message remains at the head of the queue.
    virtual EGCResults RetrieveMessage(__int32 *punMsgType, void *pubDest, __int32 cubDest, __int32 *pcubMsgSize) = 0;

};

// callback notification - A new message is available for reading from the message queue
struct GCMessageAvailable_t
{
    enum { k_iCallback = k_iSteamGameCoordinatorCallbacks + 1 };
    __int32 m_nMessageSize;
};

// callback notification - A message failed to make it to the GC. It may be down temporarily
struct GCMessageFailed_t
{
    enum { k_iCallback = k_iSteamGameCoordinatorCallbacks + 2 };
};