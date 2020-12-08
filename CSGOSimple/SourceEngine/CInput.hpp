#pragma once

#include "QAngle.hpp"
#include "CRC.hpp"
#include "IAppSystem.hpp"
#include <string>
#include "UtlVector.hpp"

#define MAX_SPLITSCREEN_CLIENT_BITS 2
// this should == MAX_JOYSTICKS in InputEnums.h
#define MAX_SPLITSCREEN_CLIENTS	( 1 << MAX_SPLITSCREEN_CLIENT_BITS ) // 4

#define JOYSTICK_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_BUTTON + ((_joystick) * JOYSTICK_MAX_BUTTON_COUNT) + (_button) )
#define JOYSTICK_POV_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_POV_BUTTON + ((_joystick) * JOYSTICK_POV_BUTTON_COUNT) + (_button) )
#define JOYSTICK_AXIS_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_AXIS_BUTTON + ((_joystick) * JOYSTICK_AXIS_BUTTON_COUNT) + (_button) )

#define JOYSTICK_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_BUTTON_INTERNAL( _joystick, _button ) )
#define JOYSTICK_POV_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_POV_BUTTON_INTERNAL( _joystick, _button ) )
#define JOYSTICK_AXIS_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_AXIS_BUTTON_INTERNAL( _joystick, _button ) )

#define INVALID_USER_ID		-1

#define IN_ATTACK        (1 << 0)
#define IN_JUMP               (1 << 1)
#define IN_DUCK               (1 << 2)
#define IN_FORWARD       (1 << 3)
#define IN_BACK               (1 << 4)
#define IN_USE           (1 << 5)
#define IN_CANCEL        (1 << 6)
#define IN_LEFT               (1 << 7)
#define IN_RIGHT         (1 << 8)
#define IN_MOVELEFT      (1 << 9)
#define IN_MOVERIGHT     (1 << 10)
#define IN_ATTACK2       (1 << 11)
#define IN_RUN           (1 << 12)
#define IN_RELOAD        (1 << 13)
#define IN_ALT1               (1 << 14)
#define IN_ALT2               (1 << 15)
#define IN_SCORE         (1 << 16)   // Used by client.dll for when scoreboard is held down
#define IN_SPEED         (1 << 17) // Player is holding the speed key
#define IN_WALK               (1 << 18) // Player holding walk key
#define IN_ZOOM               (1 << 19) // Zoom key for HUD zoom
#define IN_WEAPON1       (1 << 20) // weapon defines these bits
#define IN_WEAPON2       (1 << 21) // weapon defines these bits
#define IN_BULLRUSH      (1 << 22)
#define IN_GRENADE1      (1 << 23) // grenade 1
#define IN_GRENADE2      (1 << 24) // grenade 2
#define   IN_LOOKSPIN         (1 << 25)

enum
{
	MAX_JOYSTICKS = MAX_SPLITSCREEN_CLIENTS,
	MOUSE_BUTTON_COUNT = 5,
};

enum JoystickAxis_t
{
	JOY_AXIS_X = 0,
	JOY_AXIS_Y,
	JOY_AXIS_Z,
	JOY_AXIS_R,
	JOY_AXIS_U,
	JOY_AXIS_V,
	MAX_JOYSTICK_AXES,
};

enum
{
	JOYSTICK_MAX_BUTTON_COUNT = 32,
	JOYSTICK_POV_BUTTON_COUNT = 4,
	JOYSTICK_AXIS_BUTTON_COUNT = MAX_JOYSTICK_AXES * 2,
};

enum ButtonCode_t :int
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP, // A fake button which is 'pressed' and 'released' when the wheel is moved up
	MOUSE_WHEEL_DOWN, // A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1,

	// Joystick
	JOYSTICK_FIRST = MOUSE_LAST + 1,

	JOYSTICK_FIRST_BUTTON = JOYSTICK_FIRST,
	JOYSTICK_LAST_BUTTON = JOYSTICK_BUTTON_INTERNAL(MAX_JOYSTICKS - 1, JOYSTICK_MAX_BUTTON_COUNT - 1),
	JOYSTICK_FIRST_POV_BUTTON,
	JOYSTICK_LAST_POV_BUTTON = JOYSTICK_POV_BUTTON_INTERNAL(MAX_JOYSTICKS - 1, JOYSTICK_POV_BUTTON_COUNT - 1),
	JOYSTICK_FIRST_AXIS_BUTTON,
	JOYSTICK_LAST_AXIS_BUTTON = JOYSTICK_AXIS_BUTTON_INTERNAL(MAX_JOYSTICKS - 1, JOYSTICK_AXIS_BUTTON_COUNT - 1),

	JOYSTICK_LAST = JOYSTICK_LAST_AXIS_BUTTON,

	BUTTON_CODE_LAST,
	BUTTON_CODE_COUNT = BUTTON_CODE_LAST - KEY_FIRST + 1,

	// Helpers for XBox 360
	KEY_XBUTTON_UP = JOYSTICK_FIRST_POV_BUTTON, // POV buttons
	KEY_XBUTTON_RIGHT,
	KEY_XBUTTON_DOWN,
	KEY_XBUTTON_LEFT,

	KEY_XBUTTON_A = JOYSTICK_FIRST_BUTTON, // Buttons
	KEY_XBUTTON_B,
	KEY_XBUTTON_X,
	KEY_XBUTTON_Y,
	KEY_XBUTTON_LEFT_SHOULDER,
	KEY_XBUTTON_RIGHT_SHOULDER,
	KEY_XBUTTON_BACK,
	KEY_XBUTTON_START,
	KEY_XBUTTON_STICK1,
	KEY_XBUTTON_STICK2,
	KEY_XBUTTON_INACTIVE_START,

	KEY_XSTICK1_RIGHT = JOYSTICK_FIRST_AXIS_BUTTON, // XAXIS POSITIVE
	KEY_XSTICK1_LEFT, // XAXIS NEGATIVE
	KEY_XSTICK1_DOWN, // YAXIS POSITIVE
	KEY_XSTICK1_UP, // YAXIS NEGATIVE
	KEY_XBUTTON_LTRIGGER, // ZAXIS POSITIVE
	KEY_XBUTTON_RTRIGGER, // ZAXIS NEGATIVE
	KEY_XSTICK2_RIGHT, // UAXIS POSITIVE
	KEY_XSTICK2_LEFT, // UAXIS NEGATIVE
	KEY_XSTICK2_DOWN, // VAXIS POSITIVE
	KEY_XSTICK2_UP, // VAXIS NEGATIVE
};

extern const char* s_pButtonCodeName[];

enum MouseCodeState_t
{
	BUTTON_RELEASED = 0,
	BUTTON_PRESSED,
	BUTTON_DOUBLECLICKED,
};

#define JOYSTICK_AXIS_INTERNAL( _joystick, _axis ) ( JOYSTICK_FIRST_AXIS + ((_joystick) * MAX_JOYSTICK_AXES) + (_axis) )
#define JOYSTICK_AXIS( _joystick, _axis ) ( (AnalogCode_t)JOYSTICK_AXIS_INTERNAL( _joystick, _axis ) )

enum AnalogCode_t
{
	ANALOG_CODE_INVALID = -1,
	MOUSE_X = 0,
	MOUSE_Y,
	MOUSE_XY, // Invoked when either x or y changes
	MOUSE_WHEEL,

	JOYSTICK_FIRST_AXIS,
	JOYSTICK_LAST_AXIS = JOYSTICK_AXIS_INTERNAL(MAX_JOYSTICKS - 1, MAX_JOYSTICK_AXES - 1),

	ANALOG_CODE_LAST,
};

struct InputEvent_t
{
	int m_nType; // Type of the event (see InputEventType_t)
	int m_nTick; // Tick on which the event occurred
	int m_nData; // Generic 32-bit data, what it contains depends on the event
	int m_nData2; // Generic 32-bit data, what it contains depends on the event
	int m_nData3; // Generic 32-bit data, what it contains depends on the event
};

namespace SourceEngine
{
	class bf_read;
	class bf_write;
	
	struct InputState_t
	{
		// Analog states
		void* m_ButtonState;
		int m_ButtonPressedTick[BUTTON_CODE_LAST];
		int m_ButtonReleasedTick[BUTTON_CODE_LAST];
		int m_pAnalogDelta[ANALOG_CODE_LAST];
		int m_pAnalogValue[ANALOG_CODE_LAST];
		CUtlVector< InputEvent_t > m_Events;
		bool m_bDirty;
	};
	class CMoveData
	{
	public:
		bool m_bFirstRunOfFunctions : 1;
		bool m_bGameCodeMovedPlayer : 1;
		bool m_bNoAirControl : 1;

		unsigned long m_nPlayerHandle;
		int m_nImpulseCommand;
		QAngle m_vecViewAngles;
		QAngle m_vecAbsViewAngles;
		int m_nButtons;
		int m_nOldButtons;
		float m_flForwardMove;
		float m_flSideMove;
		float m_flUpMove;

		float m_flMaxSpeed;
		float m_flClientMaxSpeed;

		Vector m_vecVelocity;
		Vector m_vecOldVelocity;
		float somefloat;
		QAngle m_vecAngles;
		QAngle m_vecOldAngles;

		float m_outStepHeight;
		Vector m_outWishVel;
		Vector m_outJumpVel;

		Vector m_vecConstraintCenter;
		float m_flConstraintRadius;
		float m_flConstraintWidth;
		float m_flConstraintSpeedFactor;
		bool m_bConstraintPastRadius;

		void SetAbsOrigin(const Vector& vec);
		const Vector& GetAbsOrigin() const;

	private:
		Vector m_vecAbsOrigin; // edict::origin
	};


	class CUserCmd
	{
	public:
		virtual ~CUserCmd()
		{
		};

		[[nodiscard]] CRC32_t GetChecksum() const
		{
			CRC32_t crc;
			CRC32_Init(&crc);

			CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
			CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
			CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
			CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
			CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
			CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
			CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
			CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
			CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
			CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
			CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
			CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
			CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
			CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

			CRC32_Final(&crc);
			return crc;
		}

		int command_number; // 0x04
		int tick_count; // 0x08
		QAngle viewangles; // 0x0C
		Vector aimdirection; // 0x18
		float forwardmove; // 0x24
		float sidemove; // 0x28
		float upmove; // 0x2C
		int buttons; // 0x30
		char impulse; // 0x34
		int weaponselect; // 0x38
		int weaponsubtype; // 0x3C
		int random_seed; // 0x40
		short mousedx; // 0x44
		short mousedy; // 0x46
		bool hasbeenpredicted; // 0x48
		char pad_0x4C[0x18]; // 0x4C
	};

	class CVerifiedUserCmd
	{
	public:
		CUserCmd m_cmd;
		CRC32_t m_crc;
	};

	class CInput
	{
	public:
		void* pvftable; //0x00
		bool m_fTrackIRAvailable; //0x04
		bool m_bMouseInitialized; //0x05
		bool m_bMouseActive; //0x06
		bool m_fJoystickAdvancedInit; //0x07
		char pad_0x08[0x2C]; //0x08
		void* m_pKeys; //0x34
		char pad_0x38[0x64]; //0x38
		bool m_fCameraInterceptingMouse; //0x9C
		bool m_fCameraInThirdPerson; //0x9D
		bool m_fCameraMovingWithMouse; //0x9E
		Vector m_vecCameraOffset; //0xA0
		bool m_fCameraDistanceMove; //0xAC
		int m_nCameraOldX; //0xB0
		int m_nCameraOldY; //0xB4
		int m_nCameraX; //0xB8
		int m_nCameraY; //0xBC
		bool m_CameraIsOrthographic; //0xC0
		QAngle m_angPreviousViewAngles; //0xC4
		QAngle m_angPreviousViewAnglesTilt; //0xD0
		float m_flLastForwardMove; //0xDC
		int m_nClearInputState; //0xE0
		char pad_0xE4[0x8]; //0xE4
		CUserCmd* m_pCommands; //0xEC
		CVerifiedUserCmd* m_pVerifiedCommands; //0xF0
	};

	class IInputSystem : public IAppSystem
	{
	public:
		// Attach, detach input system from a particular window
		// This window should be the root window for the application
		// Only 1 window should be attached at any given time.
		virtual void AttachToWindow(void* hWnd) = 0;//9
		virtual void DetachFromWindow() = 0;

		// Enables/disables input. PollInputState will not update current 
		// button/analog states when it is called if the system is disabled.
		virtual void EnableInput(bool bEnable) = 0;//11

		// Enables/disables the windows message pump. PollInputState will not
		// Peek/Dispatch messages if this is disabled
		virtual void EnableMessagePump(bool bEnable) = 0;

		// Polls the current input state
		virtual void PollInputState() = 0;

		// Gets the time of the last polling in ms
		virtual int GetPollTick() const = 0;

		// Is a button down? "Buttons" are binary-state input devices (mouse buttons, keyboard keys)
		virtual bool IsButtonDown(ButtonCode_t code) const = 0;//15

		// Returns the tick at which the button was pressed and released
		virtual int GetButtonPressedTick(ButtonCode_t code) const = 0;
		virtual int GetButtonReleasedTick(ButtonCode_t code) const = 0;

		// Gets the value of an analog input device this frame
		// Includes joysticks, mousewheel, mouse
		virtual int GetAnalogValue(AnalogCode_t code) const = 0;

		// Gets the change in a particular analog input device this frame
		// Includes joysticks, mousewheel, mouse
		virtual int GetAnalogDelta(AnalogCode_t code) const = 0;

		// Returns the input events since the last poll
		virtual int GetEventCount() const = 0;
		virtual const InputEvent_t* GetEventData() const = 0;

		// Posts a user-defined event into the event queue; this is expected
		// to be called in overridden wndprocs connected to the root panel.
		virtual void PostUserEvent(const InputEvent_t& event) = 0;

		// Returns the number of joysticks
		virtual int GetJoystickCount() const = 0;

		// Enable/disable joystick, it has perf costs
		virtual void EnableJoystickInput(int nJoystick, bool bEnable) = 0;

		// Enable/disable diagonal joystick POV (simultaneous POV buttons down)
		virtual void EnableJoystickDiagonalPOV(int nJoystick, bool bEnable) = 0;

		// Sample the joystick and append events to the input queue
		virtual void SampleDevices() = 0;

		// FIXME: Currently force-feedback is only supported on the Xbox 360
		virtual void SetRumble(float fLeftMotor, float fRightMotor, int userId = INVALID_USER_ID) = 0;
		virtual void StopRumble() = 0;

		// Resets the input state
		virtual void ResetInputState() = 0;

		// Sets a player as the primary user - all other controllers will be ignored.
		virtual void SetPrimaryUserId(int userId) = 0;

		// Convert back + forth between ButtonCode/AnalogCode + strings
		virtual const char* ButtonCodeToString(ButtonCode_t code) const = 0;
		virtual const char* AnalogCodeToString(AnalogCode_t code) const = 0;
		virtual ButtonCode_t StringToButtonCode(const char* pString) const = 0;
		virtual AnalogCode_t StringToAnalogCode(const char* pString) const = 0;

		// Sleeps until input happens. Pass a negative number to sleep infinitely
		virtual void SleepUntilInput(int nMaxSleepTimeMS = -1) = 0;

		// Convert back + forth between virtual codes + button codes
		// FIXME: This is a temporary piece of code
		virtual ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey) const = 0;
		virtual int ButtonCodeToVirtualKey(ButtonCode_t code) const = 0;
		virtual ButtonCode_t ScanCodeToButtonCode(int lParam) const = 0;

		// How many times have we called PollInputState?
		virtual int GetPollCount() const = 0;

		// Sets the cursor position
		virtual void SetCursorPosition(int x, int y) = 0;

		// NVNT get address to haptics interface
		virtual void* GetHapticsInterfaceAddress() const = 0;

		virtual void SetNovintPure(bool bPure) = 0;

		// read and clear accumulated raw input values
		virtual bool GetRawMouseAccumulators(int& accumX, int& accumY) = 0;

		// tell the input system that we're not a game, we're console text mode.
		// this is used for dedicated servers to not initialize joystick system.
		// this needs to be called before CInputSystem::Init (e.g. in PreInit of
		// some system) if you want ot prevent the joystick system from ever
		// being initialized.
		virtual void SetConsoleTextMode(bool bConsoleTextMode) = 0;

		WNDPROC m_ChainedWndProc;
		HWND m_hAttachedHWnd;
		bool m_bEnabled;
		bool m_bPumpEnabled;
		bool m_bIsPolling;
		enum
		{
			INPUT_STATE_QUEUED = 0,
			INPUT_STATE_CURRENT,

			INPUT_STATE_COUNT,
		};

		// Current button state
		InputState_t m_InputState[INPUT_STATE_COUNT];

		DWORD m_StartupTimeTick;
		int m_nLastPollTick;
		int m_nLastSampleTick;
		int m_nPollCount;

		// Mouse wheel hack
		UINT m_uiMouseWheel;
	};


	ButtonCode_t GetButtonCode_tFromString(const std::string& pString);
	const char* GetStringFromButtonCode_t(const ButtonCode_t& code);
	ButtonCode_t ButtonCode_VirtualKeyToButtonCode(int keyCode);
	int ButtonCode_ButtonCodeToVirtualKey(ButtonCode_t code);
}
