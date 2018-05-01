////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "terrainshaderclass.h"
#include "lightclass.h"
#include "horizontalblurshaderclass.h"
#include "verticalblurshaderclass.h"
#include "targetwindowclass.h"
#include "rendertextureclass.h"
#include "textureshaderclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////

enum Mode
{
	Terrain = 0,
	SewerSystem_CaveSystem = 1,
	Creation = 2,

};
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	//declaration of function used for post processing
	bool RenderSceneToTexture();
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();
	bool RenderScene();

private:
	bool HandleInput(float);
	bool RenderGraphics();


private:
	InputClass* m_Input;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TerrainClass* m_Terrain;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	FontShaderClass* m_FontShader;
	TextClass* m_Text;
	TerrainShaderClass* m_TerrainShader;
	LightClass* m_Light;
	float ValDisplay;
	int ValChance;
	Mode DrawMode;
	bool Pressed;
	int FaultAmount;


	HorizontalBlurShaderClass* m_horizontalBlurShader; //used for horizontal blur
	VerticalBlurShaderClass* m_verticalBlurShader;// used for vertical blur shader

	//declaration of render to texture classes 
	RenderTextureClass* m_horizontalBlurTexture;
	RenderTextureClass* m_verticalBlurTexture;
	RenderTextureClass* m_RenderTexture;
	RenderTextureClass* m_downSampleTexture;
	RenderTextureClass* m_upSampleTexture;

	//declaration of shaders
	TextureShaderClass* m_TextureShader;


	//declaration of target window class
	TargetWindowClass* m_targetWind;
	TargetWindowClass* m_downSampleWindow;
	TargetWindowClass* m_upSampleWindow;




	bool post;


};

#endif