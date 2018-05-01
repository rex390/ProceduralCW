////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//////////////
// INCLUDES //
//////////////
#include <dinput.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void GetMouseLocation(int&, int&);
	//create normal terrain
	bool IsSpacePressed();
	//leave the application
	bool IsEscapePressed();
	//movement of camera forward backwards and looking to the sides
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();
	//raise height or decrease height of camera position
	bool IsXPressed();
	bool IsZPressed();
	//movement of camera sight up, or down
	bool IsPgUpPressed();
	bool IsPgDownPressed();
	//keys for increasing and decreasing the max height
	bool IsHPressed();
	bool IsJPressed();
	//keys for increasing and decrease how often digger turns
	bool IsPPressed();
	bool IsOPressed();
	//keys for increasing and decreasing the mode
	bool IsNPressed();
	bool IsMPressed();
	//key for faulting 
	bool IsFPressed();
	//key for smoothing
	bool IsUPressed();
	//keys for increasing and decreasing the Fault Amount
	bool IsTPressed();
	bool IsYPressed();
	//increase or decrease digger terrain
	bool IsVPressed();
	bool IsBPressed();

	//movement of diggerPosition
	bool IsAPressed();
	bool IsSPressed();
	bool IsDPressed();
	bool IsWPressed();
	//used for enabling/disabling post processing effect
	bool IsEnterPressed();

	bool IsQPressed();
	bool IsEPressed();



	bool held[256];
private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif