#pragma once
#include <numeric>
#include <string>
#include <vector>

#include "core/event/event.hpp"
#define __TRANSLATE_KEY(keyMacro)                                                                                      \
	if (key == keyMacro)                                                                                               \
		return #keyMacro;

namespace tur
{
	enum class Key : u32
	{
		KEY_UNKNOWN = 0U,

		KEY_SPACE,
		KEY_APOSTROPHE,
		KEY_COMMA,
		KEY_MINUS,
		KEY_PERIOD,
		KEY_SLASH,
		KEY_BACKSLASH,

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

		KEY_SEMICOLON,
		KEY_EQUAL,

		KEY_A = 97U,
		KEY_B = 98U,
		KEY_C = 99U,
		KEY_D = 100U,
		KEY_E = 101U,
		KEY_F = 102U,
		KEY_G = 103U,
		KEY_H = 104U,
		KEY_I = 105U,
		KEY_J = 106U,
		KEY_K = 107U,
		KEY_L = 108U,
		KEY_M = 109U,
		KEY_N = 110U,
		KEY_O = 111U,
		KEY_P = 112U,
		KEY_Q = 113U,
		KEY_R = 114U,
		KEY_S = 115U,
		KEY_T = 116U,
		KEY_U = 117U,
		KEY_V = 118U,
		KEY_W = 119U,
		KEY_X = 120U,
		KEY_Y = 121U,
		KEY_Z = 122U,

		KEY_LEFT_BRACKET = 20U,
		KEY_RIGHT_BRACKET,

		KEY_GRAVE_ACCENT, /* ` */

		KEY_ESCAPE,
		KEY_ENTER,
		KEY_TAB,
		KEY_BACKSPACE,

		KEY_INSERT,
		KEY_DELETE,

		KEY_RIGHT,
		KEY_LEFT,
		KEY_DOWN,
		KEY_UP,

		KEY_PAGE_UP,
		KEY_PAGE_DOWN,
		KEY_HOME,
		KEY_END,

		KEY_CAPS_LOCK,
		KEY_SCROLL_LOCK,

		KEY_NUM_LOCK,
		KEY_PRINT_SCREEN,
		KEY_PAUSE,

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
		KEY_F13,
		KEY_F14,
		KEY_F15,
		KEY_F16,
		KEY_F17,
		KEY_F18,
		KEY_F19,
		KEY_F20,
		KEY_F21,
		KEY_F22,
		KEY_F23,
		KEY_F24,

		KEY_KP_0,
		KEY_KP_1,
		KEY_KP_2,
		KEY_KP_3,
		KEY_KP_4,
		KEY_KP_5,
		KEY_KP_6,
		KEY_KP_7,
		KEY_KP_8,
		KEY_KP_9,

		KEY_KP_DECIMAL,
		KEY_KP_DIVIDE,
		KEY_KP_MULTIPLY,
		KEY_KP_SUBTRACT,

		KEY_KP_ADD,
		KEY_KP_ENTER,
		KEY_KP_EQUAL,

		KEY_LEFT_SHIFT,
		KEY_LEFT_CONTROL,
		KEY_LEFT_ALT,
		KEY_LEFT_SUPER,
		KEY_RIGHT_SHIFT,
		KEY_RIGHT_CONTROL,
		KEY_RIGHT_ALT,
		KEY_RIGHT_SUPER,

		KEY_MENU,
		KEY_LAST,
	};

	enum class KeyboardMods : u32
	{
		KEY_MOD_SHIFT = 1 << 0,
		KEY_MOD_CTRL = 1 << 1,
		KEY_MOD_ALT = 1 << 2,
		KEY_MOD_SUPER = 1 << 3,
		KEY_MOD_CAPS_LOCK = 1 << 4,
		KEY_MOD_NUM_LOCK = 1 << 5,
	};

	inline std::string get_keyboard_mod_names(KeyboardMods mods)
	{
		std::vector<std::string> modResults;

		if ((u32)mods & (u32)KeyboardMods::KEY_MOD_SHIFT)
			modResults.push_back("SHIFT");

		if ((u32)mods & (u32)KeyboardMods::KEY_MOD_CTRL)
			modResults.push_back("CTRL");

		if ((u32)mods & (u32)KeyboardMods::KEY_MOD_ALT)
			modResults.push_back("ALT");

		if ((u32)mods & (u32)KeyboardMods::KEY_MOD_SUPER)
			modResults.push_back("SUPER");

		if ((u32)mods & (u32)KeyboardMods::KEY_MOD_CAPS_LOCK)
			modResults.push_back("CAPS_LOCK");

		if ((u32)mods & (u32)KeyboardMods::KEY_MOD_NUM_LOCK)
			modResults.push_back("NUM_LOCK");

		if (modResults.size() == 0)
			return "NONE";

		if (modResults.size() == 1)
			return modResults.back().c_str();

		std::string str = "";
		for (size_t i = 0; i != modResults.size(); ++i)
		{
			if (i == modResults.size() - 1)
				str += modResults[i];
			else
				str += modResults[i] + " | ";
		}

		return str;
	}

	constexpr inline const char* get_key_name(Key key)
	{
		__TRANSLATE_KEY(Key::KEY_UNKNOWN);
		__TRANSLATE_KEY(Key::KEY_SPACE);
		__TRANSLATE_KEY(Key::KEY_APOSTROPHE);
		__TRANSLATE_KEY(Key::KEY_COMMA);
		__TRANSLATE_KEY(Key::KEY_MINUS);
		__TRANSLATE_KEY(Key::KEY_PERIOD);
		__TRANSLATE_KEY(Key::KEY_SLASH);
		__TRANSLATE_KEY(Key::KEY_0);
		__TRANSLATE_KEY(Key::KEY_1);
		__TRANSLATE_KEY(Key::KEY_2);
		__TRANSLATE_KEY(Key::KEY_3);
		__TRANSLATE_KEY(Key::KEY_4);
		__TRANSLATE_KEY(Key::KEY_5);
		__TRANSLATE_KEY(Key::KEY_6);
		__TRANSLATE_KEY(Key::KEY_7);
		__TRANSLATE_KEY(Key::KEY_8);
		__TRANSLATE_KEY(Key::KEY_9);
		__TRANSLATE_KEY(Key::KEY_SEMICOLON);
		__TRANSLATE_KEY(Key::KEY_EQUAL);
		__TRANSLATE_KEY(Key::KEY_A);
		__TRANSLATE_KEY(Key::KEY_B);
		__TRANSLATE_KEY(Key::KEY_C);
		__TRANSLATE_KEY(Key::KEY_D);
		__TRANSLATE_KEY(Key::KEY_E);
		__TRANSLATE_KEY(Key::KEY_F);
		__TRANSLATE_KEY(Key::KEY_G);
		__TRANSLATE_KEY(Key::KEY_H);
		__TRANSLATE_KEY(Key::KEY_I);
		__TRANSLATE_KEY(Key::KEY_J);
		__TRANSLATE_KEY(Key::KEY_K);
		__TRANSLATE_KEY(Key::KEY_L);
		__TRANSLATE_KEY(Key::KEY_M);
		__TRANSLATE_KEY(Key::KEY_N);
		__TRANSLATE_KEY(Key::KEY_O);
		__TRANSLATE_KEY(Key::KEY_P);
		__TRANSLATE_KEY(Key::KEY_Q);
		__TRANSLATE_KEY(Key::KEY_R);
		__TRANSLATE_KEY(Key::KEY_S);
		__TRANSLATE_KEY(Key::KEY_T);
		__TRANSLATE_KEY(Key::KEY_U);
		__TRANSLATE_KEY(Key::KEY_V);
		__TRANSLATE_KEY(Key::KEY_W);
		__TRANSLATE_KEY(Key::KEY_X);
		__TRANSLATE_KEY(Key::KEY_Y);
		__TRANSLATE_KEY(Key::KEY_Z);
		__TRANSLATE_KEY(Key::KEY_LEFT_BRACKET);
		__TRANSLATE_KEY(Key::KEY_BACKSLASH);
		__TRANSLATE_KEY(Key::KEY_RIGHT_BRACKET);
		__TRANSLATE_KEY(Key::KEY_GRAVE_ACCENT);
		__TRANSLATE_KEY(Key::KEY_ESCAPE);
		__TRANSLATE_KEY(Key::KEY_ENTER);
		__TRANSLATE_KEY(Key::KEY_TAB);
		__TRANSLATE_KEY(Key::KEY_BACKSPACE);
		__TRANSLATE_KEY(Key::KEY_INSERT);
		__TRANSLATE_KEY(Key::KEY_DELETE);
		__TRANSLATE_KEY(Key::KEY_RIGHT);
		__TRANSLATE_KEY(Key::KEY_LEFT);
		__TRANSLATE_KEY(Key::KEY_DOWN);
		__TRANSLATE_KEY(Key::KEY_UP);
		__TRANSLATE_KEY(Key::KEY_PAGE_UP);
		__TRANSLATE_KEY(Key::KEY_PAGE_DOWN);
		__TRANSLATE_KEY(Key::KEY_HOME);
		__TRANSLATE_KEY(Key::KEY_END);
		__TRANSLATE_KEY(Key::KEY_CAPS_LOCK);
		__TRANSLATE_KEY(Key::KEY_SCROLL_LOCK);
		__TRANSLATE_KEY(Key::KEY_NUM_LOCK);
		__TRANSLATE_KEY(Key::KEY_PRINT_SCREEN);
		__TRANSLATE_KEY(Key::KEY_PAUSE);
		__TRANSLATE_KEY(Key::KEY_F1);
		__TRANSLATE_KEY(Key::KEY_F2);
		__TRANSLATE_KEY(Key::KEY_F3);
		__TRANSLATE_KEY(Key::KEY_F4);
		__TRANSLATE_KEY(Key::KEY_F5);
		__TRANSLATE_KEY(Key::KEY_F6);
		__TRANSLATE_KEY(Key::KEY_F7);
		__TRANSLATE_KEY(Key::KEY_F8);
		__TRANSLATE_KEY(Key::KEY_F9);
		__TRANSLATE_KEY(Key::KEY_F10);
		__TRANSLATE_KEY(Key::KEY_F11);
		__TRANSLATE_KEY(Key::KEY_F12);
		__TRANSLATE_KEY(Key::KEY_F13);
		__TRANSLATE_KEY(Key::KEY_F14);
		__TRANSLATE_KEY(Key::KEY_F15);
		__TRANSLATE_KEY(Key::KEY_F16);
		__TRANSLATE_KEY(Key::KEY_F17);
		__TRANSLATE_KEY(Key::KEY_F18);
		__TRANSLATE_KEY(Key::KEY_F19);
		__TRANSLATE_KEY(Key::KEY_F20);
		__TRANSLATE_KEY(Key::KEY_F21);
		__TRANSLATE_KEY(Key::KEY_F22);
		__TRANSLATE_KEY(Key::KEY_F23);
		__TRANSLATE_KEY(Key::KEY_F24);
		__TRANSLATE_KEY(Key::KEY_KP_0);
		__TRANSLATE_KEY(Key::KEY_KP_1);
		__TRANSLATE_KEY(Key::KEY_KP_2);
		__TRANSLATE_KEY(Key::KEY_KP_3);
		__TRANSLATE_KEY(Key::KEY_KP_4);
		__TRANSLATE_KEY(Key::KEY_KP_5);
		__TRANSLATE_KEY(Key::KEY_KP_6);
		__TRANSLATE_KEY(Key::KEY_KP_7);
		__TRANSLATE_KEY(Key::KEY_KP_8);
		__TRANSLATE_KEY(Key::KEY_KP_9);
		__TRANSLATE_KEY(Key::KEY_KP_DECIMAL);
		__TRANSLATE_KEY(Key::KEY_KP_DIVIDE);
		__TRANSLATE_KEY(Key::KEY_KP_MULTIPLY);
		__TRANSLATE_KEY(Key::KEY_KP_SUBTRACT);
		__TRANSLATE_KEY(Key::KEY_KP_ADD);
		__TRANSLATE_KEY(Key::KEY_KP_ENTER);
		__TRANSLATE_KEY(Key::KEY_KP_EQUAL);
		__TRANSLATE_KEY(Key::KEY_LEFT_SHIFT);
		__TRANSLATE_KEY(Key::KEY_LEFT_CONTROL);
		__TRANSLATE_KEY(Key::KEY_LEFT_ALT);
		__TRANSLATE_KEY(Key::KEY_LEFT_SUPER);
		__TRANSLATE_KEY(Key::KEY_RIGHT_SHIFT);
		__TRANSLATE_KEY(Key::KEY_RIGHT_CONTROL);
		__TRANSLATE_KEY(Key::KEY_RIGHT_ALT);
		__TRANSLATE_KEY(Key::KEY_RIGHT_SUPER);
		__TRANSLATE_KEY(Key::KEY_MENU);
		__TRANSLATE_KEY(Key::KEY_LAST);
	}
}