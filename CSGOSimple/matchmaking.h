#pragma once

namespace SourceEngine {
	class KeyValues;
}

class IMatchEventsSink
{
public:
	virtual void OnEvent(SourceEngine::KeyValues* pEvent) = 0;

	//
	// List of events
	//

/*
	"OnSysStorageDevicesChanged"
		Signalled when system storage device change is detected.
	params:
		void

	"OnSysSigninChange"
		Signalled when one or more users sign out.
	params:
		string "action"		- signin change event: "signin", "signout"
		int "numUsers"		- how many users signed in/out (defines valid user0 - userN-1 fields)
		int "mask"			- bitmask of controllers affected
		int	"user%d"		- controller index affected

	"OnSysXUIEvent"
		Signalled when an XUI event occurs.
	params:
		string "action"		- XUI action type: "opening", "closed"

	"OnSysMuteListChanged"
		Signalled when system mute list change occurs.
	params:
		void

	"OnSysInputDevicesChanged"
		Signalled when input device disconnection is detected.
	params:
		int "mask"			- bitmask of which slot's controller was disconnected [0-1]



	"OnEngineLevelLoadingStarted"
		Signalled when a level starts loading.
	params:
		string "name"		- level name

	"OnEngineListenServerStarted"
		Signalled when a listen server level loads enough to accept client connections.
	params:
		void

	"OnEngineLevelLoadingTick"
		Signalled periodically while a level is loading,
		after loading started and before loading finished.
	params:
		void

	"OnEngineLevelLoadingFinished"
		Signalled when a level is finished loading.
	params:
		int "error"			- whether an extended error occurred
		string "reason"		- reason description

	"OnEngineClientSignonStateChange"
		Signalled when client's signon state is changing.
	params:
		int "slot"			- client ss slot
		int "old"			- old state
		int "new"			- new state
		int "count"			- count

	"OnEngineDisconnectReason"
		Signalled before a disconnect is going to occur and a reason
		for disconnect is available.
	params:
		string "reason"		- reason description

	"OnEngineEndGame"
		Signalled before a disconnect is going to occur and notifies the members
		of the game that the game has reached a conclusion or a vote to end the
		game has passed and the game should terminate and return to lobby if possible.
	params:
		string "reason"		- reason description



	"OnMatchPlayerMgrUpdate"
		Signalled when a player manager update occurs.
	params:
		string "update" =	- update type
							"searchstarted"		- search started
							"searchfinished"	- search finished
							"friend"			- friend details updated
		uint64 "xuid"		- xuid of a player if applicable

	"OnMatchPlayerMgrReset"
		Signalled when the game needs to go into attract mode.
	params:
		string "reason"		- one of the following reasons:
						"GuestSignedIn"			- guest user signed in
						"GameUserSignedOut"		- user involved in game has signed out

	"OnMatchServerMgrUpdate"
		Signalled when a server manager update occurs.
	params:
		string "update" =	- update type
							"searchstarted"		- search started
							"searchfinished"	- search finished
							"server"			- server details updated
		uint64 "xuid"		- xuid of a server if applicable

	"OnMatchSessionUpdate"
		Signalled when a session changes.
	params:
		strings "state" =	- new state of the session
							"ready"			- session is completely initialized and ready
							"updated"		- session settings have been updated



	"OnNetLanConnectionlessPacket"
		Signalled when a lan network packet is received.
	params:
		string "from"		- netadr of sender as recorded by network layer
		subkey				- packet message



	"OnProfilesChanged"
		Signalled when new number of game users are set for the game.
	params:
		int "numProfiles"	- number of game users set for the game

	"OnProfileDataLoaded"
		Signalled when a user profile title data is loaded.
	params:
		int "iController"	- index of controller whose title data is now loaded

	"OnProfileStorageAvailable"
		Signalled when a user profile storage device is selected.
	params:
		int "iController"	- index of controller whose storage device is now selected

	"OnProfileUnavailable"
		Signalled when a user profile is detected as unavailable.
	params:
		int "iController"	- index of controller whose profile was detected as unavailable



	"OnPlayerUpdated"
		Signalled when information about a player changes.
	params:
		uint64 "xuid"		- XUID of the player updated

	"OnPlayerRemoved"
		Signalled when a player is removed from the game.
	params:
		uint64 "xuid"		- XUID of the player removed

	"OnPlayerMachinesConnected"
		Signalled when new machines become part of the session, they will be last
		in the list of connected machines.
	params:
		int "numMachines"	- number of new machines connected

	"OnPlayerActivity"
		Signalled when a player activity is detected.
	params:
		uint64 "xuid"		- XUID of the player
		string "act"		- type of activity:
							"voice"		- player is voice chatting



	"OnMuteChanged"
		Signalled when a mute list is updated.
	params:
		void

	"OnInvite"
		Signalled when game invite event occurs.
	params:
		int "user"			- controller index accepting the invite or causing invite error
		string "sessioninfo" - session info of the invite host
		string "action" =	- invite action
							"accepted" - when an invite is accepted by user
							"storage" - when a storage device needs to be validated
							"error" - when an error occurs that prevents invite from being accepted
							"join" - when destructive actions or storage devices are confirmed by user
							"deny" - when invite is rejected by user
		string "error"		- error description: "NotOnline", "NoMultiplayer", etc.
		ptr int "confirmed"	- handler should set pointed int to 0 if confirmation is pending
								and send a "join" action OnInvite event after destructive
								actions are confirmed by user, storage devices are mounted, etc.
*/

};

class IMatchEventsSubscription
{
public:
	virtual void Subscribe(IMatchEventsSink * pSink) = 0;
	virtual void Unsubscribe(IMatchEventsSink* pSink) = 0;

	virtual void BroadcastEvent(SourceEngine::KeyValues* pEvent) = 0;

	virtual void RegisterEventData(SourceEngine::KeyValues* pEventData) = 0;
	virtual SourceEngine::KeyValues* GetEventData(char const* szEventDataKey) = 0;
};

class IMatchExtensions
{
public:
	// Registers an extension interface
	virtual void RegisterExtensionInterface(char const* szInterfaceString, void* pvInterface) = 0;

	// Unregisters an extension interface
	virtual void UnregisterExtensionInterface(char const* szInterfaceString, void* pvInterface) = 0;

	// Gets a pointer to a registered extension interface
	virtual void* GetRegisteredExtensionInterface(char const* szInterfaceString) = 0;
};

class IMatchTitle
{
public:
	// Title ID
	virtual uint64_t GetTitleID() = 0;

	// Service ID for XLSP
	virtual uint64_t GetTitleServiceID() = 0;

	// Describe title settings using a bitwise combination of flags
	virtual uint64_t GetTitleSettingsFlags() = 0;

	//not full
};

class IMatchSession
{
public:
	// Get an internal pointer to session system-specific data
	virtual SourceEngine::KeyValues* GetSessionSystemData() = 0;

	// Get an internal pointer to session settings
	virtual SourceEngine::KeyValues* GetSessionSettings() = 0;

	// Update session settings, only changing keys and values need
	// to be passed and they will be updated
	virtual void UpdateSessionSettings(SourceEngine::KeyValues* pSettings) = 0;

	// Issue a session command
	virtual void Command(SourceEngine::KeyValues* pCommand) = 0;

	// Get the lobby or XSession ID 
	virtual uint64_t GetSessionID() = 0;

	// Callback when team changes
	virtual void UpdateTeamProperties(SourceEngine::KeyValues* pTeamProperties) = 0;
};

class IMatchNetworkMsgController;
class IMatchSystem;

class IMatchFramework : public SourceEngine::IAppSystem
{
public:
	// Run frame of the matchmaking framework
	virtual void RunFrame() = 0;


	// Get matchmaking extensions
	virtual IMatchExtensions* GetMatchExtensions() = 0;

	// Get events container
	virtual IMatchEventsSubscription* GetEventsSubscription() = 0;

	// Get the matchmaking title interface
	virtual IMatchTitle* GetMatchTitle() = 0;

	// Get the match session interface of the current match framework type
	virtual IMatchSession* GetMatchSession() = 0;

	// Get the network msg encode/decode factory
	virtual IMatchNetworkMsgController* GetMatchNetworkMsgController() = 0;

	// Get the match system
	virtual IMatchSystem* GetMatchSystem() = 0;

	// Send the key values back to the server
	virtual void ApplySettings(SourceEngine::KeyValues* keyValues) = 0;

	// Entry point to create session
	virtual void CreateSession(SourceEngine::KeyValues* pSettings) = 0;

	// Entry point to match into a session
	virtual void MatchSession(SourceEngine::KeyValues* pSettings) = 0;

	// Accept invite
	virtual void AcceptInvite(int iController) = 0;

	// Close the session
	virtual void CloseSession() = 0;

	// Checks to see if the current game is being played online ( as opposed to locally against bots )
	virtual bool IsOnlineGame() = 0;

	// Called by the client to notify matchmaking that it should update matchmaking properties based
	// on player distribution among the teams.
	virtual void UpdateTeamProperties(SourceEngine::KeyValues* pTeamProperties) = 0;
};

class changer_match_events_sink : public IMatchEventsSink
{
public:
	changer_match_events_sink();
	~changer_match_events_sink();
	void OnEvent(SourceEngine::KeyValues* pEvent) override;
};
