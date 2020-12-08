#pragma once

struct CMsgSteamDatagramGameServerAuthTicket;

class CSteamNetworkingUtils
{
public:
	virtual int GetLocalTimestamp() = 0;
	virtual bool CheckPingDataUpToDate(float date) = 0;
	virtual double GetLocalPingLocation(int a1) = 0; // always -1.0
	virtual bool IsPingMeasurementInProgress() = 0;
	virtual int EstimatePingTimeBetweenTwoLocations(int, int) = 0; //always -1
	virtual int EstimatePingTimeFromLocalHost(int) = 0; //always -1
	virtual void ReceivedTicket(CMsgSteamDatagramGameServerAuthTicket* ticket) = 0;
};