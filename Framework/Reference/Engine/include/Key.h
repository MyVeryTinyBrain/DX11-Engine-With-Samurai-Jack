#pragma once

ENGINE_BEGIN
enum class Key : unsigned char
{
	LeftMouse = 0x01,
	RightMouse,

	WhileMouse = 0x04,

	Tab = 0x09,
	Return = 0x0D,
	LShift = 0x10,
	LCtrl = 0x11,
	Escape = 0x1B,
	Space = 0x20,

	Left = 0x25,
	Up, Right,
	Down = 0x28,

	Delete = 0x2E,

	Zero = 0x30,
	One, Two, Three, Four, Five, Six, Seven, Eight,
	Nine = 0x39,

	A = 0x41,
	B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y,
	Z = 0x5A,
	LWin = 0x5B,
	RWin = 0x5C,

	Num0 = 0x60,
	Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8,
	Num9 = 0x69,

	NumMultiply = 0x6A,
	NumAdd, NumSeparator, NumSubtract, NumDecimal,
	NumDivide = 0x6F,

	F1 = 0x70,
	F2, F3, F4, F5, F6, F7, F8, F9, F10, F11,
	F12 = 0X7B,
};
ENGINE_END
