// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdexcept>
#include "CInput.hpp"
#include "../XorStr.hpp"
#include <cassert>


static int s_pButtonCodeToVirtual[BUTTON_CODE_LAST];
static ButtonCode_t s_pVirtualKeyToButtonCode[256];

//static const char* s_pAnalogCodeName[] =
//{
//	"MOUSE_X",		// MOUSE_X = 0,
//	"MOUSE_Y",		// MOUSE_Y,
//	"MOUSE_XY",		// MOUSE_XY,		// Invoked when either x or y changes
//	"MOUSE_WHEEL",	// MOUSE_WHEEL,
//
//	"X AXIS",		// JOY_AXIS_X
//	"Y AXIS",		// JOY_AXIS_Y
//	"Z AXIS",		// JOY_AXIS_Z
//	"R AXIS",		// JOY_AXIS_R
//	"U AXIS",		// JOY_AXIS_U
//	"V AXIS",		// JOY_AXIS_V
//};

const char* s_pButtonCodeName[] =
{
	"", // KEY_NONE
	"0", // KEY_0,
	"1", // KEY_1,
	"2", // KEY_2,
	"3", // KEY_3,
	"4", // KEY_4,
	"5", // KEY_5,
	"6", // KEY_6,
	"7", // KEY_7,
	"8", // KEY_8,
	"9", // KEY_9,
	"a", // KEY_A,
	"b", // KEY_B,
	"c", // KEY_C,
	"d", // KEY_D,
	"e", // KEY_E,
	"f", // KEY_F,
	"g", // KEY_G,
	"h", // KEY_H,
	"i", // KEY_I,
	"j", // KEY_J,
	"k", // KEY_K,
	"l", // KEY_L,
	"m", // KEY_M,
	"n", // KEY_N,
	"o", // KEY_O,
	"p", // KEY_P,
	"q", // KEY_Q,
	"r", // KEY_R,
	"s", // KEY_S,
	"t", // KEY_T,
	"u", // KEY_U,
	"v", // KEY_V,
	"w", // KEY_W,
	"x", // KEY_X,
	"y", // KEY_Y,
	"z", // KEY_Z,
	"KP_INS", // KEY_PAD_0,
	"KP_END", // KEY_PAD_1,
	"KP_DOWNARROW", // KEY_PAD_2,
	"KP_PGDN", // KEY_PAD_3,
	"KP_LEFTARROW", // KEY_PAD_4,
	"KP_5", // KEY_PAD_5,
	"KP_RIGHTARROW",// KEY_PAD_6,
	"KP_HOME", // KEY_PAD_7,
	"KP_UPARROW", // KEY_PAD_8,
	"KP_PGUP", // KEY_PAD_9,
	"KP_SLASH", // KEY_PAD_DIVIDE,
	"KP_MULTIPLY", // KEY_PAD_MULTIPLY,
	"KP_MINUS", // KEY_PAD_MINUS,
	"KP_PLUS", // KEY_PAD_PLUS,
	"KP_ENTER", // KEY_PAD_ENTER,
	"KP_DEL", // KEY_PAD_DECIMAL,
	"[", // KEY_LBRACKET,
	"]", // KEY_RBRACKET,
	"SEMICOLON", // KEY_SEMICOLON,
	"'", // KEY_APOSTROPHE,
	"`", // KEY_BACKQUOTE,
	",", // KEY_COMMA,
	".", // KEY_PERIOD,
	"/", // KEY_SLASH,
	"\\", // KEY_BACKSLASH,
	"-", // KEY_MINUS,
	"=", // KEY_EQUAL,
	"ENTER", // KEY_ENTER,
	"SPACE", // KEY_SPACE,
	"BACKSPACE", // KEY_BACKSPACE,
	"TAB", // KEY_TAB,
	"CAPSLOCK", // KEY_CAPSLOCK,
	"NUMLOCK", // KEY_NUMLOCK,
	"ESCAPE", // KEY_ESCAPE,
	"SCROLLLOCK", // KEY_SCROLLLOCK,
	"INS", // KEY_INSERT,
	"DEL", // KEY_DELETE,
	"HOME", // KEY_HOME,
	"END", // KEY_END,
	"PGUP", // KEY_PAGEUP,
	"PGDN", // KEY_PAGEDOWN,
	"PAUSE", // KEY_BREAK,
	"SHIFT", // KEY_LSHIFT,
	"RSHIFT", // KEY_RSHIFT,
	"ALT", // KEY_LALT,
	"RALT", // KEY_RALT,
	"CTRL", // KEY_LCONTROL,
	"RCTRL", // KEY_RCONTROL,
	"LWIN", // KEY_LWIN,
	"RWIN", // KEY_RWIN,
	"APP", // KEY_APP,
	"UPARROW", // KEY_UP,
	"LEFTARROW", // KEY_LEFT,
	"DOWNARROW", // KEY_DOWN,
	"RIGHTARROW", // KEY_RIGHT,
	"F1", // KEY_F1,
	"F2", // KEY_F2,
	"F3", // KEY_F3,
	"F4", // KEY_F4,
	"F5", // KEY_F5,
	"F6", // KEY_F6,
	"F7", // KEY_F7,
	"F8", // KEY_F8,
	"F9", // KEY_F9,
	"F10", // KEY_F10,
	"F11", // KEY_F11,
	"F12", // KEY_F12,

	// FIXME: CAPSLOCK/NUMLOCK/SCROLLLOCK all appear above. What are these for?!
	// They only appear in CInputWin32::UpdateToggleButtonState in vgui2
	"CAPSLOCKTOGGLE", // KEY_CAPSLOCKTOGGLE,
	"NUMLOCKTOGGLE", // KEY_NUMLOCKTOGGLE,
	"SCROLLLOCKTOGGLE", // KEY_SCROLLLOCKTOGGLE,

	// Mouse
	"MOUSE1", // MOUSE_LEFT,
	"MOUSE2", // MOUSE_RIGHT,
	"MOUSE3", // MOUSE_MIDDLE,
	"MOUSE4", // MOUSE_4,
	"MOUSE5", // MOUSE_5,

	"MWHEELUP", // MOUSE_WHEEL_UP
	"MWHEELDOWN", // MOUSE_WHEEL_DOWN

	"JOY1", // JOYSTICK_FIRST_BUTTON
	"JOY2",
	"JOY3",
	"JOY4",
	"JOY5",
	"JOY6",
	"JOY7",
	"JOY8",
	"JOY9",
	"JOY10",
	"JOY11",
	"JOY12",
	"JOY13",
	"JOY14",
	"JOY15",
	"JOY16",
	"JOY17",
	"JOY18",
	"JOY19",
	"JOY20",
	"JOY21",
	"JOY22",
	"JOY23",
	"JOY24",
	"JOY25",
	"JOY26",
	"JOY27",
	"JOY28",
	"JOY29",
	"JOY30",
	"JOY31",
	"JOY32", // JOYSTICK_LAST_BUTTON

	"POV_UP", // JOYSTICK_FIRST_POV_BUTTON
	"POV_RIGHT",
	"POV_DOWN",
	"POV_LEFT", // JOYSTICK_LAST_POV_BUTTON

	"X AXIS POS", // JOYSTICK_FIRST_AXIS_BUTTON
	"X AXIS NEG",
	"Y AXIS POS",
	"Y AXIS NEG",
	"Z AXIS POS",
	"Z AXIS NEG",
	"R AXIS POS",
	"R AXIS NEG",
	"U AXIS POS",
	"U AXIS NEG",
	"V AXIS POS",
	"V AXIS NEG", // JOYSTICK_LAST_AXIS_BUTTON
};


const char* SourceEngine::GetStringFromButtonCode_t(const ButtonCode_t& code)
{
	if (code > BUTTON_CODE_COUNT || code < BUTTON_CODE_NONE)
		throw std::runtime_error("GetStringFromButtonCode_t() : buttoncode was invalid");

	// Backward compat for screwed up previous joystick button names


//	for (size_t i = BUTTON_CODE_NONE; i < BUTTON_CODE_LAST; ++i)
//	{
//		if (s_pButtonCodeName[i] == pString)
//			return static_cast<ButtonCode_t>(i);
//	}

	return s_pButtonCodeName[static_cast<int>(code)];
}

ButtonCode_t SourceEngine::GetButtonCode_tFromString(const std::string& pString)
{
	if (pString.empty())
		return BUTTON_CODE_INVALID;

	// Backward compat for screwed up previous joystick button names
	if (pString.find(XorStr("aux")) != std::string::npos)//  !strnicmp(pString, "aux", 3))
	{
		const auto nIndex = atoi(&pString[3]);
		if (nIndex < 29)
			return JOYSTICK_BUTTON(0, nIndex);
		return nIndex <= 32 ? JOYSTICK_POV_BUTTON(0, nIndex - 29) : BUTTON_CODE_INVALID;
	}

	for (size_t i = BUTTON_CODE_NONE; i < BUTTON_CODE_LAST; ++i)
	{
		if (s_pButtonCodeName[i] == pString)
			return static_cast<ButtonCode_t>(i);
	}

	return BUTTON_CODE_INVALID;
}

void ButtonCode_InitKeyTranslationTable()
{
	//static_assert(sizeof(s_pButtonCodeName) / sizeof(const char*) == BUTTON_CODE_LAST);
	//static_assert(sizeof(s_pAnalogCodeName) / sizeof(const char*) == ANALOG_CODE_LAST);

	// set virtual key translation table
	memset(s_pVirtualKeyToButtonCode, KEY_NONE, sizeof(s_pVirtualKeyToButtonCode));

	s_pVirtualKeyToButtonCode['0'] = KEY_0;
	s_pVirtualKeyToButtonCode['1'] = KEY_1;
	s_pVirtualKeyToButtonCode['2'] = KEY_2;
	s_pVirtualKeyToButtonCode['3'] = KEY_3;
	s_pVirtualKeyToButtonCode['4'] = KEY_4;
	s_pVirtualKeyToButtonCode['5'] = KEY_5;
	s_pVirtualKeyToButtonCode['6'] = KEY_6;
	s_pVirtualKeyToButtonCode['7'] = KEY_7;
	s_pVirtualKeyToButtonCode['8'] = KEY_8;
	s_pVirtualKeyToButtonCode['9'] = KEY_9;
	s_pVirtualKeyToButtonCode['A'] = KEY_A;
	s_pVirtualKeyToButtonCode['B'] = KEY_B;
	s_pVirtualKeyToButtonCode['C'] = KEY_C;
	s_pVirtualKeyToButtonCode['D'] = KEY_D;
	s_pVirtualKeyToButtonCode['E'] = KEY_E;
	s_pVirtualKeyToButtonCode['F'] = KEY_F;
	s_pVirtualKeyToButtonCode['G'] = KEY_G;
	s_pVirtualKeyToButtonCode['H'] = KEY_H;
	s_pVirtualKeyToButtonCode['I'] = KEY_I;
	s_pVirtualKeyToButtonCode['J'] = KEY_J;
	s_pVirtualKeyToButtonCode['K'] = KEY_K;
	s_pVirtualKeyToButtonCode['L'] = KEY_L;
	s_pVirtualKeyToButtonCode['M'] = KEY_M;
	s_pVirtualKeyToButtonCode['N'] = KEY_N;
	s_pVirtualKeyToButtonCode['O'] = KEY_O;
	s_pVirtualKeyToButtonCode['P'] = KEY_P;
	s_pVirtualKeyToButtonCode['Q'] = KEY_Q;
	s_pVirtualKeyToButtonCode['R'] = KEY_R;
	s_pVirtualKeyToButtonCode['S'] = KEY_S;
	s_pVirtualKeyToButtonCode['T'] = KEY_T;
	s_pVirtualKeyToButtonCode['U'] = KEY_U;
	s_pVirtualKeyToButtonCode['V'] = KEY_V;
	s_pVirtualKeyToButtonCode['W'] = KEY_W;
	s_pVirtualKeyToButtonCode['X'] = KEY_X;
	s_pVirtualKeyToButtonCode['Y'] = KEY_Y;
	s_pVirtualKeyToButtonCode['Z'] = KEY_Z;
	s_pVirtualKeyToButtonCode[VK_NUMPAD0] = KEY_PAD_0;
	s_pVirtualKeyToButtonCode[VK_NUMPAD1] = KEY_PAD_1;
	s_pVirtualKeyToButtonCode[VK_NUMPAD2] = KEY_PAD_2;
	s_pVirtualKeyToButtonCode[VK_NUMPAD3] = KEY_PAD_3;
	s_pVirtualKeyToButtonCode[VK_NUMPAD4] = KEY_PAD_4;
	s_pVirtualKeyToButtonCode[VK_NUMPAD5] = KEY_PAD_5;
	s_pVirtualKeyToButtonCode[VK_NUMPAD6] = KEY_PAD_6;
	s_pVirtualKeyToButtonCode[VK_NUMPAD7] = KEY_PAD_7;
	s_pVirtualKeyToButtonCode[VK_NUMPAD8] = KEY_PAD_8;
	s_pVirtualKeyToButtonCode[VK_NUMPAD9] = KEY_PAD_9;
	s_pVirtualKeyToButtonCode[VK_DIVIDE] = KEY_PAD_DIVIDE;
	s_pVirtualKeyToButtonCode[VK_MULTIPLY] = KEY_PAD_MULTIPLY;
	s_pVirtualKeyToButtonCode[VK_SUBTRACT] = KEY_PAD_MINUS;
	s_pVirtualKeyToButtonCode[VK_ADD] = KEY_PAD_PLUS;
	s_pVirtualKeyToButtonCode[VK_RETURN] = KEY_PAD_ENTER;
	s_pVirtualKeyToButtonCode[VK_DECIMAL] = KEY_PAD_DECIMAL;
	s_pVirtualKeyToButtonCode[0xdb] = KEY_LBRACKET;
	s_pVirtualKeyToButtonCode[0xdd] = KEY_RBRACKET;
	s_pVirtualKeyToButtonCode[0xba] = KEY_SEMICOLON;
	s_pVirtualKeyToButtonCode[0xde] = KEY_APOSTROPHE;
	s_pVirtualKeyToButtonCode[0xc0] = KEY_BACKQUOTE;
	s_pVirtualKeyToButtonCode[0xbc] = KEY_COMMA;
	s_pVirtualKeyToButtonCode[0xbe] = KEY_PERIOD;
	s_pVirtualKeyToButtonCode[0xbf] = KEY_SLASH;
	s_pVirtualKeyToButtonCode[0xdc] = KEY_BACKSLASH;
	s_pVirtualKeyToButtonCode[0xbd] = KEY_MINUS;
	s_pVirtualKeyToButtonCode[0xbb] = KEY_EQUAL;
	s_pVirtualKeyToButtonCode[VK_RETURN] = KEY_ENTER;
	s_pVirtualKeyToButtonCode[VK_SPACE] = KEY_SPACE;
	s_pVirtualKeyToButtonCode[VK_BACK] = KEY_BACKSPACE;
	s_pVirtualKeyToButtonCode[VK_TAB] = KEY_TAB;
	s_pVirtualKeyToButtonCode[VK_CAPITAL] = KEY_CAPSLOCK;
	s_pVirtualKeyToButtonCode[VK_NUMLOCK] = KEY_NUMLOCK;
	s_pVirtualKeyToButtonCode[VK_ESCAPE] = KEY_ESCAPE;
	s_pVirtualKeyToButtonCode[VK_SCROLL] = KEY_SCROLLLOCK;
	s_pVirtualKeyToButtonCode[VK_INSERT] = KEY_INSERT;
	s_pVirtualKeyToButtonCode[VK_DELETE] = KEY_DELETE;
	s_pVirtualKeyToButtonCode[VK_HOME] = KEY_HOME;
	s_pVirtualKeyToButtonCode[VK_END] = KEY_END;
	s_pVirtualKeyToButtonCode[VK_PRIOR] = KEY_PAGEUP;
	s_pVirtualKeyToButtonCode[VK_NEXT] = KEY_PAGEDOWN;
	s_pVirtualKeyToButtonCode[VK_PAUSE] = KEY_BREAK;
	s_pVirtualKeyToButtonCode[VK_SHIFT] = KEY_RSHIFT;
	//s_pVirtualKeyToButtonCode[VK_SHIFT] = KEY_LSHIFT;	// SHIFT -> left SHIFT
	s_pVirtualKeyToButtonCode[VK_MENU] = KEY_RALT;
	//s_pVirtualKeyToButtonCode[VK_MENU] = KEY_LALT;		// ALT -> left ALT
	s_pVirtualKeyToButtonCode[VK_CONTROL] = KEY_RCONTROL;
	//s_pVirtualKeyToButtonCode[VK_CONTROL] = KEY_LCONTROL;	// CTRL -> left CTRL
	s_pVirtualKeyToButtonCode[VK_LWIN] = KEY_LWIN;
	s_pVirtualKeyToButtonCode[VK_RWIN] = KEY_RWIN;
	s_pVirtualKeyToButtonCode[VK_APPS] = KEY_APP;
	s_pVirtualKeyToButtonCode[VK_UP] = KEY_UP;
	s_pVirtualKeyToButtonCode[VK_LEFT] = KEY_LEFT;
	s_pVirtualKeyToButtonCode[VK_DOWN] = KEY_DOWN;
	s_pVirtualKeyToButtonCode[VK_RIGHT] = KEY_RIGHT;
	s_pVirtualKeyToButtonCode[VK_F1] = KEY_F1;
	s_pVirtualKeyToButtonCode[VK_F2] = KEY_F2;
	s_pVirtualKeyToButtonCode[VK_F3] = KEY_F3;
	s_pVirtualKeyToButtonCode[VK_F4] = KEY_F4;
	s_pVirtualKeyToButtonCode[VK_F5] = KEY_F5;
	s_pVirtualKeyToButtonCode[VK_F6] = KEY_F6;
	s_pVirtualKeyToButtonCode[VK_F7] = KEY_F7;
	s_pVirtualKeyToButtonCode[VK_F8] = KEY_F8;
	s_pVirtualKeyToButtonCode[VK_F9] = KEY_F9;
	s_pVirtualKeyToButtonCode[VK_F10] = KEY_F10;
	s_pVirtualKeyToButtonCode[VK_F11] = KEY_F11;
	s_pVirtualKeyToButtonCode[VK_F12] = KEY_F12;

	// create reverse table engine to virtual
	/*in volvo code here was - i < BUTTON_CODE_LAST, but this override some keys like KEY_HOME, using  256 for safe*/
	for (int i = 0; i < 256 ; i++)
	{
		s_pButtonCodeToVirtual[s_pVirtualKeyToButtonCode[i]] = i;
	}

	s_pButtonCodeToVirtual[0] = 0;
}

bool init_once = true;

ButtonCode_t SourceEngine::ButtonCode_VirtualKeyToButtonCode(int keyCode)
{
	if (init_once)
	{
		ButtonCode_InitKeyTranslationTable();
		init_once = false;
	}

	if (keyCode < 0 || keyCode >= (sizeof(s_pVirtualKeyToButtonCode) / sizeof(s_pVirtualKeyToButtonCode[0])))
	{
		assert(false);
		return KEY_NONE;
	}
	return s_pVirtualKeyToButtonCode[keyCode];
}

int SourceEngine::ButtonCode_ButtonCodeToVirtualKey(ButtonCode_t code)
{
	if (init_once)
	{
		ButtonCode_InitKeyTranslationTable();
		init_once = false;
	}
	return s_pButtonCodeToVirtual[code];
}
