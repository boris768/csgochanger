// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "LoggingSystem.h"
#include "SourceEngine/SDK.hpp"
#include "CustomWinAPI.h"
#include "XorStr.hpp"

//SourceEngine::SteamDatagramClient_listener::SteamDatagramClient_listener()
//{
//	auto loggingsystem = interfaces.GameLoggingSystem();
//	SteamDatagramClientID = loggingsystem->FindChannel(XorStr("SteamDatagramClient"));
//	SteamDatagramClientChannel = loggingsystem->GetChannel(SteamDatagramClientID);
//	loggingsystem->RegisterLoggingListener(this);
//}

//void SourceEngine::SteamDatagramClient_listener::Log(LoggingContext_t* pContext, const char* pMessage)
//{
//	if (pContext->m_ChannelID != SteamDatagramClientID)
//		return;
//	const std::string msg_analize = pMessage;
//	//Received Steam datagram ticket for server [A:1:81630212:8573].  match_id=3211145072087138398
//	std::string::size_type pos = msg_analize.find(XorStr("ticket for server"));
//	if (pos != std::string::npos)
//	{
//		const std::string::size_type pos2 = msg_analize.find(XorStr(".  match_id"));
//		if (pos2)
//		{
//			pos += 18;
//			current_server = msg_analize.substr(pos, pos2 - pos);
//		}
//	}
//}

void SourceEngine::LoggingSystem::SetChannelSpewLevel(LoggingChannelID_t id, LoggingSeverity_t severity)
{
	if (!SetChannelSpewLevelFn)
	{
		SetChannelSpewLevelFn = static_cast<oLoggingSystem_SetChannelSpewLevel>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(memory.tier0().lpBaseOfDll), XorStr("LoggingSystem_SetChannelSpewLevel")));
	}
	SetChannelSpewLevelFn(id, severity);
}

bool SourceEngine::LoggingSystem::IsChannelEnabled(LoggingChannelID_t ID, LoggingSeverity_t severity)
{
	if (!IsChannelEnabledFn)
	{
		IsChannelEnabledFn = static_cast<oLoggingSystem_IsChannelEnabled>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(memory.tier0().lpBaseOfDll), XorStr("LoggingSystem_IsChannelEnabled")));
	}
	return IsChannelEnabledFn(ID, severity);
}

int SourceEngine::LoggingSystem::RegisterLoggingChannel(const char* pChannelName, RegisterTagsFunc registerTagsFunc,
                                                        int flags, LoggingSeverity_t minimumSeverity, Color spewColor)
{
	if (!RegisterLoggingChannelFn)
	{
		RegisterLoggingChannelFn = static_cast<oLoggingSystem_RegisterLoggingChannel>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(memory.tier0().lpBaseOfDll), XorStr("LoggingSystem_RegisterLoggingChannel")));
	}
	return RegisterLoggingChannelFn(pChannelName, registerTagsFunc, flags, minimumSeverity, spewColor);
}

SourceEngine::LoggingResponse_t SourceEngine::LoggingSystem::LogDirect(LoggingChannelID_t channelID,
                                                                       LoggingSeverity_t severity, Color spewColor,
                                                                       const char* pMessage)
{
	if (!LoggingSystem_LogDirectFn)
	{
		LoggingSystem_LogDirectFn = static_cast<oLoggingSystem_LogDirect>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(memory.tier0().lpBaseOfDll), XorStr("LoggingSystem_LogDirect")));
	}
	return LoggingSystem_LogDirectFn(channelID, severity, spewColor, pMessage);
}

SourceEngine::LoggingChannelID_t SourceEngine::LoggingSystem::FindChannel(const char* name)
{
	if (!FindChannelFn)
	{
		FindChannelFn = static_cast<oLoggingSystem_FindChannel>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(memory.tier0().lpBaseOfDll), XorStr("LoggingSystem_FindChannel")));
	}
	return FindChannelFn(name);
}

SourceEngine::LoggingChannel_t* SourceEngine::LoggingSystem::GetChannel(LoggingChannelID_t channelID)
{
	if (!GetChannelFn)
	{
		GetChannelFn = static_cast<oLoggingSystem_GetChannel>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(memory.tier0().lpBaseOfDll), XorStr("LoggingSystem_GetChannel")));
	}
	return GetChannelFn(channelID);
}

void SourceEngine::LoggingSystem::RegisterLoggingListener(ILoggingListener* pListener)
{
	if (!RegisterLoggingListenerFn)
	{
		RegisterLoggingListenerFn = static_cast<oLoggingSystem_RegisterLoggingListener>(g_pWinApi->GetProcAddress(
			static_cast<HMODULE>(memory.tier0().lpBaseOfDll), XorStr("LoggingSystem_RegisterLoggingListener")));
	}
	RegisterLoggingListenerFn(pListener);
}

SourceEngine::log::log(LoggingSeverity_t flag_logging_severity)
{
	channel_id_ = logging_system_.RegisterLoggingChannel(XorStr("Changer"), nullptr, 0, flag_logging_severity, Color(0,255,0));
}

void SourceEngine::log::msg(const std::string& msg)
{
	logging_system_.LogDirect(channel_id_, LS_MESSAGE, UNSPECIFIED_LOGGING_COLOR, msg.c_str());
}

void SourceEngine::log::warning(const std::string& msg)
{
	logging_system_.LogDirect(channel_id_, LS_WARNING, UNSPECIFIED_LOGGING_COLOR, msg.c_str());
}

void SourceEngine::log::error(const std::string& msg)
{
	logging_system_.LogDirect(channel_id_, LS_ERROR, UNSPECIFIED_LOGGING_COLOR, msg.c_str());
}
