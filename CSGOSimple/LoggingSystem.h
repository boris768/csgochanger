#pragma once
#include "SourceEngine/Color.hpp"
#include <string>

namespace SourceEngine
{
	typedef int LoggingChannelID_t;
	constexpr LoggingChannelID_t INVALID_LOGGING_CHANNEL_ID = -1;
	constexpr Color UNSPECIFIED_LOGGING_COLOR(0, 0, 0, 0);
	constexpr int MAX_LOGGING_IDENTIFIER_LENGTH = 32;
	constexpr int MAX_LOGGING_MESSAGE_LENGTH = 2048;
	constexpr int MAX_LOGGING_CHANNEL_COUNT = 256;

	enum LoggingSeverity_t
	{
		//-----------------------------------------------------------------------------
		// An informative logging message.
		//-----------------------------------------------------------------------------
		LS_MESSAGE = 0,

		//-----------------------------------------------------------------------------
		// A warning, typically non-fatal
		//-----------------------------------------------------------------------------
		LS_WARNING = 1,

		//-----------------------------------------------------------------------------
		// A message caused by an Assert**() operation.
		//-----------------------------------------------------------------------------
		LS_ASSERT = 2,

		//-----------------------------------------------------------------------------
		// An error, typically fatal/unrecoverable.
		//-----------------------------------------------------------------------------
		LS_ERROR = 3,

		//-----------------------------------------------------------------------------
		// A placeholder level, higher than any legal value.
		// Not a real severity value!
		//-----------------------------------------------------------------------------
		LS_HIGHEST_SEVERITY = 4,
	};

	enum LoggingResponse_t
	{
		LR_CONTINUE,
		LR_DEBUGGER,
		LR_ABORT,
	};

	enum LoggingChannelFlags_t
	{
		//-----------------------------------------------------------------------------
		// Indicates that the spew is only relevant to interactive consoles.
		//-----------------------------------------------------------------------------
		LCF_CONSOLE_ONLY = 0x00000001,

		//-----------------------------------------------------------------------------
		// Indicates that spew should not be echoed to any output devices.
		// A suitable logging listener must be registered which respects this flag 
		// (e.g. a file logger).
		//-----------------------------------------------------------------------------
		LCF_DO_NOT_ECHO = 0x00000002,
	};

	typedef void (__cdecl *RegisterTagsFunc)();

	struct LoggingContext_t
	{
		// ID of the channel being logged to.
		LoggingChannelID_t m_ChannelID;
		// Flags associated with the channel.
		LoggingChannelFlags_t m_Flags;
		// Severity of the logging event.
		LoggingSeverity_t m_Severity;
		// Color of logging message if one was specified to Log_****() macro.
		// If not specified, falls back to channel color.
		// If channel color is not specified, this value is UNSPECIFIED_LOGGING_COLOR
		// and indicates that a suitable default should be chosen.
		int m_Color[4];
	};

	// Internal data to represent a logging tag
	struct LoggingTag_t
	{
		const char* m_pTagName;
		LoggingTag_t* m_pNextTag;
	};

	// Internal data to represent a logging channel.
	struct LoggingChannel_t
	{
		bool HasTag(const char* pTag) const
		{
			LoggingTag_t* pCurrentTag = m_pFirstTag;
			while (pCurrentTag != nullptr)
			{
				if (strstr(pCurrentTag->m_pTagName, pTag))
				{
					return true;
				}
				pCurrentTag = pCurrentTag->m_pNextTag;
			}
			return false;
		}

		bool IsEnabled(LoggingSeverity_t severity) const { return severity >= m_MinimumSeverity; }
		void SetSpewLevel(LoggingSeverity_t severity) { m_MinimumSeverity = severity; }

		LoggingChannelID_t m_ID;
		LoggingChannelFlags_t m_Flags; // an OR'd combination of LoggingChannelFlags_t
		LoggingSeverity_t m_MinimumSeverity; // The minimum severity level required to activate this channel.
		Color m_SpewColor;
		char m_Name[MAX_LOGGING_IDENTIFIER_LENGTH];
		LoggingTag_t* m_pFirstTag;
	};

	class ILoggingListener
	{
	public:
		virtual void Log(LoggingContext_t* pContext, const char* pMessage) = 0;
	};

	//class SteamDatagramClient_listener : public ILoggingListener
	//{
	//public:
	//	SteamDatagramClient_listener();
	//	void Log(LoggingContext_t* pContext, const char* pMessage) override;
	//private:
	//	LoggingChannel_t *SteamDatagramClientChannel;
	//	LoggingChannelID_t SteamDatagramClientID;
	//};

	using oLoggingSystem_FindChannel = LoggingChannelID_t(__cdecl*)(const char* name);
	using oLoggingSystem_SetChannelSpewLevel = void(__cdecl*)(LoggingChannelID_t, LoggingSeverity_t);
	using oLoggingSystem_IsChannelEnabled = bool(__cdecl*)(LoggingChannelID_t channelID, LoggingSeverity_t severity);
	using oLoggingSystem_RegisterLoggingChannel = LoggingChannelID_t(__cdecl*)(const char* pChannelName, RegisterTagsFunc registerTagsFunc, int flags, LoggingSeverity_t minimumSeverity, Color spewColor);
	using oLoggingSystem_LogDirect = LoggingResponse_t(__cdecl*)(LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const char* pMessage);
	using oLoggingSystem_GetChannel = LoggingChannel_t*(__cdecl*)(LoggingChannelID_t channelID);
	using oLoggingSystem_RegisterLoggingListener = void(__cdecl*)(ILoggingListener* pListener);

	class LoggingSystem
	{
	public:
		constexpr LoggingSystem()
			: SetChannelSpewLevelFn(nullptr),
			  IsChannelEnabledFn(nullptr),
			  RegisterLoggingChannelFn(nullptr),
			  LoggingSystem_LogDirectFn(nullptr),
			  FindChannelFn(nullptr),
			  GetChannelFn(nullptr),
			  RegisterLoggingListenerFn(nullptr)
		{
		}

		void SetChannelSpewLevel(LoggingChannelID_t id, LoggingSeverity_t severity);
		bool IsChannelEnabled(LoggingChannelID_t ID, LoggingSeverity_t severity);
		int RegisterLoggingChannel(const char* pChannelName, RegisterTagsFunc registerTagsFunc, int flags = 0, LoggingSeverity_t minimumSeverity = LS_MESSAGE, Color spewColor = UNSPECIFIED_LOGGING_COLOR);
		LoggingResponse_t LogDirect(LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const char* pMessage);
		LoggingChannelID_t FindChannel(const char*);
		LoggingChannel_t* GetChannel(LoggingChannelID_t channelID);
		void RegisterLoggingListener(ILoggingListener* pListener);


	private:
		oLoggingSystem_SetChannelSpewLevel SetChannelSpewLevelFn;
		oLoggingSystem_IsChannelEnabled IsChannelEnabledFn;
		oLoggingSystem_RegisterLoggingChannel RegisterLoggingChannelFn;
		oLoggingSystem_LogDirect LoggingSystem_LogDirectFn;
		oLoggingSystem_FindChannel FindChannelFn;
		oLoggingSystem_GetChannel GetChannelFn;
		oLoggingSystem_RegisterLoggingListener RegisterLoggingListenerFn;
	};

	class log
	{
	public:
		log(LoggingSeverity_t);

		void msg(const std::string& msg);
		void warning(const std::string& msg);
		void error(const std::string& msg);

	private:
		LoggingSystem logging_system_;
		LoggingChannelID_t channel_id_{};
	};
}
