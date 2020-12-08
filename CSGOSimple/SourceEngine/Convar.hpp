#pragma once

#include "ICvar.hpp"
#include "UtlVector.hpp"
#include "UtlString.hpp"

namespace SourceEngine
{
#define FORCEINLINE_CVAR inline
	//-----------------------------------------------------------------------------
	// Forward declarations
	//-----------------------------------------------------------------------------
	class ConVar;
	class CCommand;
	class ConCommand;
	class ConCommandBase;
	struct characterset_t;

	//-----------------------------------------------------------------------------
	// Any executable that wants to use ConVars need to implement one of
	// these to hook up access to console variables.
	//-----------------------------------------------------------------------------
	class IConCommandBaseAccessor
	{
	public:
		// m_unFlags is a combination of FCVAR flags in cvar.h.
		// hOut is filled in with a handle to the variable.
		virtual bool RegisterConCommandBase(ConCommandBase* pVar) = 0;
	};

	//-----------------------------------------------------------------------------
	// Called when a ConCommand needs to execute
	//-----------------------------------------------------------------------------
	typedef void (__cdecl* FnCommandCallbackV1_t)();
	typedef void (__cdecl* FnCommandCallback_t)(const CCommand& command);

	constexpr auto COMMAND_COMPLETION_MAXITEMS = 64;
	constexpr auto COMMAND_COMPLETION_ITEM_LENGTH = 64;

	//-----------------------------------------------------------------------------
	// Returns 0 to COMMAND_COMPLETION_MAXITEMS worth of completion strings
	//-----------------------------------------------------------------------------
	typedef int (__cdecl* FnCommandCompletionCallback)(const char* partial,
	                                           char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);


	//-----------------------------------------------------------------------------
	// Interface version
	//-----------------------------------------------------------------------------
	class ICommandCallback
	{
	public:
		virtual void CommandCallback(const CCommand& command) = 0;
	};

	class ICommandCompletionCallback
	{
	public:
		virtual int CommandCompletionCallback(const char* pPartial, CUtlVector<CUtlString>& commands) = 0;
	};

	//-----------------------------------------------------------------------------
	// Purpose: The base console invoked command/cvar interface
	//-----------------------------------------------------------------------------
	class ConCommandBase
	{
		friend class CCvar;
		friend class ConVar;
		friend class ConCommand;
		friend void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor* pAccessor);

		// FIXME: Remove when ConVar changes are done
		friend class CDefaultCvar;

	public:
		ConCommandBase();
		explicit ConCommandBase(const char* pName, const char* pHelpString = nullptr, int flags = 0);

		virtual ~ConCommandBase();
		virtual bool IsCommand() const;
		virtual bool IsFlagSet(int flag) const;
		virtual void AddFlags(int flags);
		virtual void RemoveFlags(int flags);
		virtual int GetFlags() const;
		virtual const char* GetName() const;
		virtual const char* GetHelpText() const;
		const ConCommandBase* GetNext() const;
		ConCommandBase* GetNext();
		virtual bool IsRegistered() const;
		virtual CVarDLLIdentifier_t GetDLLIdentifier() const;

	protected:
		virtual void Create(const char* pName, const char* pHelpString = nullptr, int flags = 0);
		virtual void Init();
		void Shutdown();
		static char* CopyString(const char* from);

		//private:
		// Next ConVar in chain
		// Prior to register, it points to the next convar in the DLL.
		// Once registered, though, m_pNext is reset to point to the next
		// convar in the global list
		ConCommandBase* m_pNext;
		bool m_bRegistered;
		const char* m_pszName;
		const char* m_pszHelpString;
		int m_nFlags;

		// ConVars add themselves to this list for the executable. 
		// Then ConVar_Register runs through  all the console variables 
		// and registers them into a global list stored in vstdlib.dll
		static ConCommandBase* s_pConCommandBases;

		// ConVars in this executable use this 'global' to access values.
		static IConCommandBaseAccessor* s_pAccessor;
	};

	//-----------------------------------------------------------------------------
	// Purpose: The console invoked command
	//-----------------------------------------------------------------------------
	class ConCommand : public ConCommandBase
	{
		friend class CCvar;

	public:
		typedef ConCommandBase BaseClass;

		ConCommand(const char* pName, FnCommandCallbackV1_t callback,
		           const char* pHelpString = nullptr, int flags = 0, FnCommandCompletionCallback completionFunc = nullptr);
		ConCommand(const char* pName, FnCommandCallback_t callback,
		           const char* pHelpString = nullptr, int flags = 0, FnCommandCompletionCallback completionFunc = nullptr);
		ConCommand(const char* pName, ICommandCallback* pCallback,
		           const char* pHelpString = nullptr, int flags = 0,
		           ICommandCompletionCallback* pCommandCompletionCallback = nullptr);

		virtual ~ConCommand();
		bool IsCommand() const override;
		virtual int AutoCompleteSuggest(const char* partial, CUtlVector<CUtlString>& commands);
		virtual bool CanAutoComplete();
		virtual void Dispatch(const CCommand& command);

	private:
		// NOTE: To maintain backward compat, we have to be very careful:
		// All public virtual methods must appear in the same order always
		// since engine code will be calling into this code, which *does not match*
		// in the mod code; it's using slightly different, but compatible versions
		// of this class. Also: Be very careful about adding new fields to this class.
		// Those fields will not exist in the version of this class that is instanced
		// in mod code.

		// Call this function when executing the command
		union
		{
			FnCommandCallbackV1_t m_fnCommandCallbackV1;
			FnCommandCallback_t m_fnCommandCallback;
			ICommandCallback* m_pCommandCallback;
		};

		union
		{
			FnCommandCompletionCallback m_fnCompletionCallback;
			ICommandCompletionCallback* m_pCommandCompletionCallback;
		};

		bool m_bHasCompletionCallback : 1;
		bool m_bUsingNewCommandCallback : 1;
		bool m_bUsingCommandCallbackInterface : 1;
	};


	//-----------------------------------------------------------------------------
	// Purpose: A console variable
	//-----------------------------------------------------------------------------
	class ConVar : public ConCommandBase, public IConVar
	{
		friend class CCvar;
		friend class ConVarRef;
		friend class SplitScreenConVarRef;

	public:
		typedef ConCommandBase BaseClass;

		ConVar(const char* pName, const char* pDefaultValue, int flags = 0);

		ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString);
		ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin,
		       bool bMax, float fMax);
		ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, FnChangeCallback_t callback);
		ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin,
		       bool bMax, float fMax, FnChangeCallback_t callback);

		virtual ~ConVar();
		bool IsFlagSet(int flag) const override;
		const char* GetHelpText() const override;
		bool IsRegistered() const override;
		const char* GetName() const override;
		const char* GetBaseName() const override;
		int GetSplitScreenPlayerSlot() const override;

		void SetFlags(const int flags) const;
		void AddFlags(int flags) override;
		int GetFlags() const override;
		bool IsCommand() const override;

		// Install a change callback (there shouldn't already be one....)
		void InstallChangeCallback(FnChangeCallback_t callback, bool bInvoke = true);
		void RemoveChangeCallback(FnChangeCallback_t callbackToRemove);

		int GetChangeCallbackCount() const { return m_pParent->m_fnChangeCallbacks.Count(); }
		FnChangeCallback_t GetChangeCallback(int slot) const { return m_pParent->m_fnChangeCallbacks[slot]; }

		// Retrieve value
		virtual float GetFloat() const;
		virtual int GetInt() const;
		bool GetBool() const { return GetInt() != 0; }
		FORCEINLINE_CVAR Color GetColor() const;
		FORCEINLINE_CVAR char const* GetString() const;

		// Compiler driven selection for template use
		template <typename T>
		T Get() const;
		template <typename T>
		T Get(T*) const;

		// Any function that allocates/frees memory needs to be virtual or else you'll have crashes
		//  from alloc/free across dll/exe boundaries.

		// These just call into the IConCommandBaseAccessor to check flags and set the var (which ends up calling InternalSetValue).
		void SetValue(const char* value) override;
		void SetValue(float value) override;
		void SetValue(int value) override;
		void SetValue(Color value) override;

		// Reset to default value
		void Revert(void) const;
		bool HasMin() const;
		bool HasMax() const;
		bool GetMin(float& minVal) const;
		bool GetMax(float& maxVal) const;
		float GetMinValue() const;
		float GetMaxValue() const;
		const char* GetDefault(void) const;

		struct CVValue_t
		{
			char* m_pszString;
			int m_StringLength;
			float m_fValue;
			int m_nValue;
		};

		CVValue_t& GetRawValue()
		{
			return m_Value;
		}

		const CVValue_t& GetRawValue() const
		{
			return m_Value;
		}

		//private:
		bool InternalSetColorFromString(const char* value);
		virtual void InternalSetValue(const char* value);
		virtual void InternalSetFloatValue(float fNewValue);
		virtual void InternalSetIntValue(int nValue);
		virtual void InternalSetColorValue(Color value);
		virtual bool ClampValue(float& value);
		virtual void ChangeStringValue(const char* tempVal, float flOldValue);
		virtual void Create(const char* pName, const char* pDefaultValue, int flags = 0, const char* pHelpString = nullptr,
		                    bool bMin = false, float fMin = 0.0, bool bMax = false, float fMax = false,
		                    FnChangeCallback_t callback = nullptr);

		// Used internally by OneTimeInit to initialize.
		void Init() override;

		//protected:
		ConVar* m_pParent;
		const char* m_pszDefaultValue;
		CVValue_t m_Value;
		bool m_bHasMin;
		float m_fMinVal;
		bool m_bHasMax;
		float m_fMaxVal;

		// Call this function when ConVar changes
		CUtlVector<FnChangeCallback_t> m_fnChangeCallbacks;
	};


	//-----------------------------------------------------------------------------
	// Purpose: Return ConVar value as a float
	// Output : float
	//-----------------------------------------------------------------------------
	FORCEINLINE_CVAR float ConVar::GetFloat() const
	{
		DWORD xored = *reinterpret_cast<DWORD*>(&m_pParent->m_Value.m_fValue) ^ reinterpret_cast<DWORD>(this);
		return *reinterpret_cast<float*>(&xored);
	}

	//-----------------------------------------------------------------------------
	// Purpose: Return ConVar value as an int
	// Output : int
	//-----------------------------------------------------------------------------
	FORCEINLINE_CVAR int ConVar::GetInt() const
	{
		return static_cast<int>(m_pParent->m_Value.m_nValue ^ reinterpret_cast<DWORD>(this));
	}

	//-----------------------------------------------------------------------------
	// Purpose: Return ConVar value as a color
	// Output : Color
	//-----------------------------------------------------------------------------
	FORCEINLINE_CVAR Color ConVar::GetColor() const
	{
		unsigned char* pColorElement = reinterpret_cast<unsigned char *>(&m_pParent->m_Value.m_nValue);
		return Color(pColorElement[0], pColorElement[1], pColorElement[2], pColorElement[3]);
	}


	//-----------------------------------------------------------------------------

	template <>
	FORCEINLINE_CVAR float ConVar::Get<float>() const { return GetFloat(); }

	template <>
	FORCEINLINE_CVAR int ConVar::Get<int>() const { return GetInt(); }

	template <>
	FORCEINLINE_CVAR bool ConVar::Get<bool>() const { return GetBool(); }

	template <>
	FORCEINLINE_CVAR const char* ConVar::Get<const char *>() const { return GetString(); }

	template <>
	FORCEINLINE_CVAR float ConVar::Get<float>(float* p) const { return *p = GetFloat(); }

	template <>
	FORCEINLINE_CVAR int ConVar::Get<int>(int* p) const { return *p = GetInt(); }

	template <>
	FORCEINLINE_CVAR bool ConVar::Get<bool>(bool* p) const { return *p = GetBool(); }

	template <>
	FORCEINLINE_CVAR const char* ConVar::Get<const char *>(char const** p) const { return *p = GetString(); }

	//-----------------------------------------------------------------------------
	// Purpose: Return ConVar value as a string, return "" for bogus string pointer, etc.
	// Output : const char *
	//-----------------------------------------------------------------------------
	FORCEINLINE_CVAR const char* ConVar::GetString() const
	{
		if (m_nFlags & FCVAR_NEVER_AS_STRING)
			return "FCVAR_NEVER_AS_STRING";
		char const* str = m_pParent->m_Value.m_pszString;
		return str ? str : "";
	}

	template< class T >
	class CConCommandMemberAccessor : public ConCommand, public ICommandCallback, public ICommandCompletionCallback
	{
		typedef ConCommand BaseClass;
		typedef void (T::* FnMemberCommandCallback_t)(const CCommand& command);
		typedef int  (T::* FnMemberCommandCompletionCallback_t)(const char* pPartial, CUtlVector< CUtlString >& commands);

	public:
		CConCommandMemberAccessor(T* pOwner, const char* pName, FnMemberCommandCallback_t callback, const char* pHelpString = 0,
			int flags = 0, FnMemberCommandCompletionCallback_t completionFunc = 0) :
			BaseClass(pName, this, pHelpString, flags, (completionFunc != 0) ? this : nullptr)
		{
			m_pOwner = pOwner;
			m_Func = callback;
			m_CompletionFunc = completionFunc;
		}

		~CConCommandMemberAccessor()
		{
			Shutdown();
		}

		void SetOwner(T* pOwner)
		{
			m_pOwner = pOwner;
		}

		void CommandCallback(const CCommand& command) override
		{
			Assert(m_pOwner && m_Func);
			(m_pOwner->*m_Func)(command);
		}

		int  CommandCompletionCallback(const char* pPartial, CUtlVector< CUtlString >& commands) override
		{
			Assert(m_pOwner && m_CompletionFunc);
			return (m_pOwner->*m_CompletionFunc)(pPartial, commands);
		}

	private:
		T* m_pOwner;
		FnMemberCommandCallback_t m_Func;
		FnMemberCommandCompletionCallback_t m_CompletionFunc;
	};

}
