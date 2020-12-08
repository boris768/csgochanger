// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <malloc.h>
#include "Convar.hpp"
#include "SDK.hpp"

#define ALIGN_VALUE( val, alignment ) ( ( (val) + (alignment) - 1 ) & ~( (alignment) - 1 ) )
#define stackalloc( _size )		alloca( ALIGN_VALUE( (_size), 16 ) )

namespace SourceEngine
{
	ConCommandBase* ConCommandBase::s_pConCommandBases = nullptr;
	IConCommandBaseAccessor* ConCommandBase::s_pAccessor = nullptr;
	static int s_nCVarFlag = 0;
	static int s_nDLLIdentifier = -1; // A unique identifier indicating which DLL this convar came from
	static bool s_bRegistered = false;

	class CDefaultAccessor : public IConCommandBaseAccessor
	{
	public:
		bool RegisterConCommandBase(ConCommandBase* pVar) override
		{
			// Link to engine's list instead
			interfaces.CVar()->RegisterConCommand(pVar);
			return true;
		}
	};

	//static CDefaultAccessor s_DefaultAccessor;

	//-----------------------------------------------------------------------------
	// Called by the framework to register ConCommandBases with the ICVar
	//-----------------------------------------------------------------------------
	void ConVar_Register(const int nCVarFlag, IConCommandBaseAccessor* pAccessor)
	{
		if (!interfaces.CVar() || s_bRegistered)
			return;

		s_bRegistered = true;
		s_nCVarFlag = nCVarFlag;
		s_nDLLIdentifier = interfaces.CVar()->AllocateDLLIdentifier();

		ConCommandBase::s_pAccessor = pAccessor; //? pAccessor : reinterpret_cast<IConCommandBaseAccessor*>(&s_DefaultAccessor);
		auto pCur = ConCommandBase::s_pConCommandBases;
		while (pCur)
		{
			auto pNext = pCur->m_pNext;
			pCur->AddFlags(s_nCVarFlag);
			pCur->Init();
			pCur = pNext;
		}

		ConCommandBase::s_pConCommandBases = nullptr;
	}

	void ConVar_Unregister()
	{
		if (!interfaces.CVar() || !s_bRegistered)
			return;

		interfaces.CVar()->UnregisterConCommands(s_nDLLIdentifier);
		s_nDLLIdentifier = -1;
		s_bRegistered = false;
	}


	//-----------------------------------------------------------------------------
	// Purpose: Default constructor
	//-----------------------------------------------------------------------------
	ConCommandBase::ConCommandBase()
	{
		m_bRegistered = false;
		m_pszName = nullptr;
		m_pszHelpString = nullptr;

		m_nFlags = 0;
		m_pNext = nullptr;
	}

	//-----------------------------------------------------------------------------
	// Purpose: The base console invoked command/cvar interface
	// Input  : *pName - name of variable/command
	//			*pHelpString - help text
	//			flags - flags
	//-----------------------------------------------------------------------------
	ConCommandBase::ConCommandBase(const char* pName, const char* pHelpString /*=0*/, const int flags /*= 0*/)
	{
		ConCommandBase::Create(pName, pHelpString, flags);
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	ConCommandBase::~ConCommandBase()
	= default;

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Output : Returns true on success, false on failure.
	//-----------------------------------------------------------------------------
	bool ConCommandBase::IsCommand() const
	{
		//	Assert( 0 ); This can't assert. . causes a recursive assert in Sys_Printf, etc.
		return true;
	}


	//-----------------------------------------------------------------------------
	// Returns the DLL identifier
	//-----------------------------------------------------------------------------
	CVarDLLIdentifier_t ConCommandBase::GetDLLIdentifier() const
	{
		return s_nDLLIdentifier;
	}


	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *pName - 
	//			callback - 
	//			*pHelpString - 
	//			flags - 
	//-----------------------------------------------------------------------------
	void ConCommandBase::Create(const char* pName, const char* pHelpString /*= 0*/, const int flags /*= 0*/)
	{
		static auto empty_string = "";

		m_bRegistered = false;

		// Name should be static data
		m_pszName = pName;
		m_pszHelpString = pHelpString ? pHelpString : empty_string;

		m_nFlags = flags;

#ifdef ALLOW_DEVELOPMENT_CVARS
        m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
#endif

		if (!(m_nFlags & FCVAR_UNREGISTERED))
		{
			m_pNext = s_pConCommandBases;
			s_pConCommandBases = this;
		}
		else
		{
			// It's unregistered
			m_pNext = nullptr;
		}

		// If s_pAccessor is already set (this ConVar is not a global variable),
		//  register it.
		//if(s_pAccessor) {
		//    Init();
		//}
		interfaces.CVar()->RegisterConCommand(this);
	}


	//-----------------------------------------------------------------------------
	// Purpose: Used internally by OneTimeInit to initialize.
	//-----------------------------------------------------------------------------
	void ConCommandBase::Init()
	{
		if (s_pAccessor)
		{
			s_pAccessor->RegisterConCommandBase(this);
		}
	}

	void ConCommandBase::Shutdown()
	{
		if (interfaces.CVar())
		{
			interfaces.CVar()->UnregisterConCommand(this);
		}
	}


	//-----------------------------------------------------------------------------
	// Purpose: Return name of the command/var
	// Output : const char
	//-----------------------------------------------------------------------------
	const char* ConCommandBase::GetName() const
	{
		return m_pszName;
	}


	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : flag - 
	// Output : Returns true on success, false on failure.
	//-----------------------------------------------------------------------------
	bool ConCommandBase::IsFlagSet(const int flag) const
	{
		return (flag & m_nFlags) != 0;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : flags - 
	//-----------------------------------------------------------------------------
	void ConCommandBase::AddFlags(const int flags)
	{
		m_nFlags |= flags;

#ifdef ALLOW_DEVELOPMENT_CVARS
        m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
#endif
	}

	void ConCommandBase::RemoveFlags(const int flags)
	{
		m_nFlags &= ~flags;
	}

	int ConCommandBase::GetFlags() const
	{
		return m_nFlags;
	}


	//-----------------------------------------------------------------------------
	// Purpose: 
	// Output : const ConCommandBase
	//-----------------------------------------------------------------------------
	const ConCommandBase* ConCommandBase::GetNext() const
	{
		return m_pNext;
	}

	ConCommandBase* ConCommandBase::GetNext()
	{
		return m_pNext;
	}


	//-----------------------------------------------------------------------------
	// Purpose: Copies string using local new/delete operators
	// Input  : *from - 
	// Output : char
	//-----------------------------------------------------------------------------
	char* ConCommandBase::CopyString(const char* from)
	{
		char* to;

		const int len = strlen(from);
		if (len <= 0)
		{
			to = new char[1];
			to[0] = 0;
		}
		else
		{
			to = new char[len + 1];
			strncpy(to, from, static_cast<size_t>(len + 1));
		}
		return to;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Output : const char
	//-----------------------------------------------------------------------------
	const char* ConCommandBase::GetHelpText() const
	{
		return m_pszHelpString;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Has this cvar been registered
	// Output : Returns true on success, false on failure.
	//-----------------------------------------------------------------------------
	bool ConCommandBase::IsRegistered() const
	{
		return m_bRegistered;
	}

	//-----------------------------------------------------------------------------
	// Default console command autocompletion function 
	//-----------------------------------------------------------------------------
	int DefaultCompletionFunc(const char* partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH])
	{
		return 0;
	}


	//-----------------------------------------------------------------------------
	// Purpose: Constructs a console command
	//-----------------------------------------------------------------------------
	//ConCommand::ConCommand()
	//{
	//	m_bIsNewConCommand = true;
	//}

	ConCommand::ConCommand(const char* pName, const FnCommandCallbackV1_t callback, const char* pHelpString /*= 0*/, const int flags /*= 0*/, const FnCommandCompletionCallback completionFunc /*= 0*/)
	{
		// Set the callback
		m_fnCommandCallbackV1 = callback;
		m_bUsingNewCommandCallback = false;
		m_bUsingCommandCallbackInterface = false;
		m_fnCompletionCallback = completionFunc ? completionFunc : reinterpret_cast<FnCommandCompletionCallback>(DefaultCompletionFunc);
		m_bHasCompletionCallback = completionFunc != nullptr;

		// Setup the rest
		BaseClass::Create(pName, pHelpString, flags);
	}

	ConCommand::ConCommand(const char* pName, const FnCommandCallback_t callback, const char* pHelpString /*= 0*/, const int flags /*= 0*/, const FnCommandCompletionCallback completionFunc /*= 0*/)
	{
		// Set the callback
		m_fnCommandCallback = callback;
		m_bUsingNewCommandCallback = true;
		m_fnCompletionCallback = completionFunc ? completionFunc : reinterpret_cast<FnCommandCompletionCallback>(DefaultCompletionFunc);
		m_bHasCompletionCallback = completionFunc != nullptr;
		m_bUsingCommandCallbackInterface = false;

		// Setup the rest
		BaseClass::Create(pName, pHelpString, flags);
	}

	ConCommand::ConCommand(const char* pName, ICommandCallback* pCallback, const char* pHelpString /*= 0*/, const int flags /*= 0*/, ICommandCompletionCallback* pCompletionCallback /*= 0*/)
	{
		// Set the callback
		m_pCommandCallback = pCallback;
		m_bUsingNewCommandCallback = false;
		m_pCommandCompletionCallback = pCompletionCallback;
		m_bHasCompletionCallback = (pCompletionCallback != nullptr);
		m_bUsingCommandCallbackInterface = true;

		// Setup the rest
		BaseClass::Create(pName, pHelpString, flags);
	}

	//-----------------------------------------------------------------------------
	// Destructor
	//-----------------------------------------------------------------------------
	ConCommand::~ConCommand()
	= default;


	//-----------------------------------------------------------------------------
	// Purpose: Returns true if this is a command 
	//-----------------------------------------------------------------------------
	bool ConCommand::IsCommand() const
	{
		return true;
	}


	//-----------------------------------------------------------------------------
	// Purpose: Invoke the function if there is one
	//-----------------------------------------------------------------------------
	void ConCommand::Dispatch(const CCommand& command)
	{
		if (m_bUsingNewCommandCallback)
		{
			if (m_fnCommandCallback)
			{
				(*m_fnCommandCallback)(command);
			}
		}
		else if (m_bUsingCommandCallbackInterface)
		{
			if (m_pCommandCallback)
			{
				m_pCommandCallback->CommandCallback(command);
			}
		}
		else
		{
			if (m_fnCommandCallbackV1)
			{
				(*m_fnCommandCallbackV1)();
			}
		}

		// Command without callback!!!
		//AssertMsg(0, ("Encountered ConCommand without a callback!\n"));
	}


	//-----------------------------------------------------------------------------
	// Purpose: Calls the autocompletion method to get autocompletion suggestions
	//-----------------------------------------------------------------------------
	int ConCommand::AutoCompleteSuggest(const char* partial, CUtlVector<CUtlString>& commands)
	{
		if (m_bUsingCommandCallbackInterface)
		{
			if (!m_pCommandCompletionCallback)
				return 0;
			return m_pCommandCompletionCallback->CommandCompletionCallback(partial, commands);
		}

		if (!m_fnCompletionCallback)
			return 0;

		char rgpchCommands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH];
		const auto iret = (m_fnCompletionCallback)(partial, rgpchCommands);
		for (auto i = 0; i < iret; ++i)
		{
			const CUtlString str = rgpchCommands[i];
			commands.AddToTail(str);
		}
		return iret;
	}


	//-----------------------------------------------------------------------------
	// Returns true if the console command can autocomplete 
	//-----------------------------------------------------------------------------
	bool ConCommand::CanAutoComplete()
	{
		return m_bHasCompletionCallback;
	}


	//-----------------------------------------------------------------------------
	//
	// Console Variables
	//
	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	// Various constructors
	//-----------------------------------------------------------------------------
	ConVar::ConVar(const char* pName, const char* pDefaultValue, const int flags /* = 0 */)
	{
		ConVar::Create(pName, pDefaultValue, flags);
	}

	ConVar::ConVar(const char* pName, const char* pDefaultValue, const int flags, const char* pHelpString)
	{
		ConVar::Create(pName, pDefaultValue, flags, pHelpString);
	}

	ConVar::ConVar(const char* pName, const char* pDefaultValue, const int flags, const char* pHelpString, const bool bMin, const float fMin, const bool bMax, const float fMax)
	{
		ConVar::Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax);
	}

	ConVar::ConVar(const char* pName, const char* pDefaultValue, const int flags, const char* pHelpString, const FnChangeCallback_t callback)
	{
		ConVar::Create(pName, pDefaultValue, flags, pHelpString, false, 0.0, false, 0.0, callback);
	}

	ConVar::ConVar(const char* pName, const char* pDefaultValue, const int flags, const char* pHelpString, const bool bMin, const float fMin, const bool bMax, const float fMax, const FnChangeCallback_t callback)
	{
		ConVar::Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, callback);
	}


	//-----------------------------------------------------------------------------
	// Destructor
	//-----------------------------------------------------------------------------
	ConVar::~ConVar()
	{
		//if(IsRegistered())
		//    interfaces.CVar()->UnregisterConCommand(this);
		if (m_Value.m_pszString)
		{
			delete[] m_Value.m_pszString;
			m_Value.m_pszString = nullptr;
		}
	}


	//-----------------------------------------------------------------------------
	// Install a change callback (there shouldn't already be one....)
	//-----------------------------------------------------------------------------
	void ConVar::InstallChangeCallback(const FnChangeCallback_t callback, const bool bInvoke)
	{
		if (callback)
		{
			if (m_fnChangeCallbacks.Find(callback) != -1)
			{
				m_fnChangeCallbacks.AddToTail(callback);
				if (bInvoke)
					callback(this, m_Value.m_pszString, m_Value.m_fValue);
			}
			else
			{
				//Warning("InstallChangeCallback ignoring duplicate change callback!!!\n");
			}
		}
		else
		{
			//Warning("InstallChangeCallback called with NULL callback, ignoring!!!\n");
		}
	}

	void ConVar::RemoveChangeCallback(const FnChangeCallback_t callbackToRemove)
	{
		if(callbackToRemove)
		{
			const auto callbackid = m_fnChangeCallbacks.Find(callbackToRemove);
			if(callbackid)
			{
				m_fnChangeCallbacks.FastRemove(callbackid);
			}
		}
	}

	bool ConVar::IsFlagSet(const int flag) const
	{
		return (flag & m_pParent->m_nFlags) != 0;
	}

	const char* ConVar::GetHelpText() const
	{
		return m_pParent->m_pszHelpString;
	}

	void ConVar::SetFlags(const int flags) const
	{
		m_pParent->m_nFlags = flags;
	}

	void ConVar::AddFlags(const int flags)
	{
		m_pParent->m_nFlags |= flags;

#ifdef ALLOW_DEVELOPMENT_CVARS
        m_pParent->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
#endif
	}

	int ConVar::GetFlags() const
	{
		return m_pParent->m_nFlags;
	}

	bool ConVar::IsRegistered() const
	{
		return m_pParent->m_bRegistered;
	}

	const char* ConVar::GetName() const
	{
		return m_pParent->m_pszName;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Output : Returns true on success, false on failure.
	//-----------------------------------------------------------------------------
	bool ConVar::IsCommand() const
	{
		return false;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : 
	//-----------------------------------------------------------------------------
	void ConVar::Init()
	{
		BaseClass::Init();
	}

	const char* ConVar::GetBaseName() const
	{
		return GetName();//m_pParent->m_pszName;
	}

	int ConVar::GetSplitScreenPlayerSlot() const
	{
		return 0;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *value - 
	//-----------------------------------------------------------------------------
	void ConVar::InternalSetValue(const char* value)
	{
		char tempVal[32];

		const auto flOldValue = m_Value.m_fValue;

		auto val = const_cast<char *>(value);
		auto fNewValue = static_cast<float>(atof(value));

		if (ClampValue(fNewValue))
		{
			snprintf(tempVal, sizeof(tempVal), "%f", fNewValue);
			val = tempVal;
		}

		// Redetermine value
		m_Value.m_fValue = fNewValue;
		m_Value.m_nValue = static_cast<int>(fNewValue);

		if (!(m_nFlags & FCVAR_NEVER_AS_STRING))
		{
			ChangeStringValue(val, flOldValue);
		}
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *tempVal - 
	//-----------------------------------------------------------------------------
	void ConVar::ChangeStringValue(const char* tempVal, float flOldValue)
	{
		auto pszOldValue = static_cast<char*>(_alloca(m_Value.m_StringLength));
		if (!m_Value.m_pszString)
			return;
		memcpy(pszOldValue, m_Value.m_pszString, static_cast<size_t>(m_Value.m_StringLength));

		const int len = strlen(tempVal) + 1;

		if (len > m_Value.m_StringLength)
		{
			//if(m_Value.m_pszString) {
			delete[] m_Value.m_pszString;
			//}

			m_Value.m_pszString = new char[len];
			m_Value.m_StringLength = len;
		}

		memcpy(m_Value.m_pszString, tempVal, static_cast<size_t>(len));

		// Invoke any necessary callback function
		for (auto i = 0; i < m_fnChangeCallbacks.Count(); i++)
		{
			m_fnChangeCallbacks[i](this, pszOldValue, flOldValue);
		}

		if (interfaces.CVar())
			interfaces.CVar()->CallGlobalChangeCallbacks(this, pszOldValue, flOldValue);
	}

	//-----------------------------------------------------------------------------
	// Purpose: Check whether to clamp and then perform clamp
	// Input  : value - 
	// Output : Returns true if value changed
	//-----------------------------------------------------------------------------
	bool ConVar::ClampValue(float& value)
	{
		if (m_bHasMin && (value < m_fMinVal))
		{
			value = m_fMinVal;
			return true;
		}

		if (m_bHasMax && (value > m_fMaxVal))
		{
			value = m_fMaxVal;
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *value - 
	//-----------------------------------------------------------------------------
	void ConVar::InternalSetFloatValue(float fNewValue)
	{
		if (fabs(fNewValue - m_Value.m_fValue) < FLT_EPSILON)
			return;

		ClampValue(fNewValue);

		// Redetermine value
		const auto flOldValue = m_Value.m_fValue;
		*reinterpret_cast<DWORD*>(&m_Value.m_fValue) = *reinterpret_cast<DWORD*>(&fNewValue) ^ reinterpret_cast<DWORD>(this);
		*reinterpret_cast<DWORD*>(&m_Value.m_nValue) = *reinterpret_cast<DWORD*>(&fNewValue) ^ reinterpret_cast<DWORD>(this);

		if (!(m_nFlags & FCVAR_NEVER_AS_STRING))
		{
			char tempVal[32];
			snprintf(tempVal, sizeof(tempVal), "%f", m_Value.m_fValue);
			ChangeStringValue(tempVal, flOldValue);
		}
		else
		{
			//Assert(m_fnChangeCallbacks.Count() == 0);
		}
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *value - 
	//-----------------------------------------------------------------------------
	void ConVar::InternalSetIntValue(int nValue)
	{
		//std::cout << nValue << std::endl;
		if (nValue == m_Value.m_nValue)
			return;

		auto fValue = static_cast<float>(nValue);
		if (ClampValue(fValue))
		{
			nValue = static_cast<int>(fValue);
		}
		// std::cout << fValue << std::endl;

		// Redetermine value
		const auto flOldValue = m_Value.m_fValue;
		//std::cout << flOldValue << std::endl;
		*reinterpret_cast<DWORD*>(&m_Value.m_fValue) = *reinterpret_cast<DWORD*>(&fValue) ^ reinterpret_cast<DWORD>(this);
		*reinterpret_cast<DWORD*>(&m_Value.m_nValue) = *reinterpret_cast<DWORD*>(&nValue) ^ reinterpret_cast<DWORD>(this);

		//std::cout << m_Value.m_fValue << std::endl;
		//std::cout << m_Value.m_nValue << std::endl;
		if (!(m_nFlags & FCVAR_NEVER_AS_STRING))
		{
			char tempVal[32];
			snprintf(tempVal, sizeof(tempVal), "%d", m_Value.m_nValue);
			ChangeStringValue(tempVal, flOldValue);
		}
		else
		{
			//Assert(m_fnChangeCallbacks.Count() == 0);
		}
		//std::cout << "end" << std::endl;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *value - 
	//-----------------------------------------------------------------------------
	void ConVar::InternalSetColorValue(Color cValue)
	{
		const auto color = static_cast<int>(cValue.GetAsARGB());
		InternalSetIntValue(color);
	}

	//-----------------------------------------------------------------------------
	// Purpose: Private creation
	//-----------------------------------------------------------------------------
	void ConVar::Create(const char* pName, const char* pDefaultValue, int flags /*= 0*/,
	                    const char* pHelpString /*= NULL*/, bool bMin /*= false*/, float fMin /*= 0.0*/,
	                    bool bMax /*= false*/, float fMax /*= false*/, FnChangeCallback_t callback /*= NULL*/)
	{
		static auto empty_string = "";

		m_pParent = this;

		// Name should be static data
		m_pszDefaultValue = pDefaultValue ? pDefaultValue : empty_string;

		m_Value.m_StringLength = strlen(m_pszDefaultValue) + 1;
		m_Value.m_pszString = new char[m_Value.m_StringLength];
		memcpy(m_Value.m_pszString, m_pszDefaultValue, static_cast<size_t>(m_Value.m_StringLength));

		m_bHasMin = bMin;
		m_fMinVal = fMin;
		m_bHasMax = bMax;
		m_fMaxVal = fMax;

		if (callback)
			m_fnChangeCallbacks.AddToTail(callback);

		auto value = static_cast<float>(atof(m_Value.m_pszString));
		*reinterpret_cast<DWORD*>(&m_Value.m_fValue) = *reinterpret_cast<DWORD*>(&value) ^ reinterpret_cast<DWORD>(this);

		// Bounds Check, should never happen, if it does, no big deal
		if (m_bHasMin && (m_Value.m_fValue < m_fMinVal))
		{
			//Assert(0);
		}

		if (m_bHasMax && (m_Value.m_fValue > m_fMaxVal))
		{
			//Assert(0);
		}

		*reinterpret_cast<DWORD*>(&m_Value.m_nValue) = *reinterpret_cast<DWORD*>(&m_Value.m_fValue);

		BaseClass::Create(pName, pHelpString, flags);
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *value - 
	//-----------------------------------------------------------------------------
	void ConVar::SetValue(const char* value)
	{
		auto var = m_pParent;
		var->InternalSetValue(value);
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : value - 
	//-----------------------------------------------------------------------------
	void ConVar::SetValue(float value)
	{
		auto var = m_pParent;
		var->InternalSetFloatValue(value);
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : value - 
	//-----------------------------------------------------------------------------
	void ConVar::SetValue(int value)
	{
		//std::cout << value << std::endl;
		auto var = m_pParent;
		//std::cout << var << std::endl;
		var->InternalSetIntValue(value);
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : value - 
	//-----------------------------------------------------------------------------
	void ConVar::SetValue(Color value)
	{
		auto var = m_pParent;
		var->InternalSetColorValue(value);
	}

	//-----------------------------------------------------------------------------
	// Purpose: Reset to default value
	//-----------------------------------------------------------------------------
	void ConVar::Revert() const
	{
		// Force default value again
		auto var = m_pParent;
		var->SetValue(var->m_pszDefaultValue);
	}

	bool ConVar::HasMin() const
	{
		return m_bHasMin;
	}

	bool ConVar::HasMax() const
	{
		return m_bHasMax;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : minVal - 
	// Output : true if there is a min set
	//-----------------------------------------------------------------------------
	bool ConVar::GetMin(float& minVal) const
	{
		minVal = m_pParent->m_fMinVal;
		return m_pParent->m_bHasMin;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : maxVal - 
	//-----------------------------------------------------------------------------
	bool ConVar::GetMax(float& maxVal) const
	{
		maxVal = m_pParent->m_fMaxVal;
		return m_pParent->m_bHasMax;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Output : const char
	//-----------------------------------------------------------------------------
	const char* ConVar::GetDefault() const
	{
		return m_pParent->m_pszDefaultValue;
	}
}
