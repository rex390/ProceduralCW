////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_Terrain = 0;
	m_Timer = 0;
	m_Position = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_FontShader = 0;
	m_Text = 0;
	m_TerrainShader = 0;
	m_Light = 0;
	ValDisplay = 3.0f;
	ValChance = 60;
	DrawMode = Terrain;
	Pressed = false;
	FaultAmount =1;
	post = false;
	m_RenderTexture = 0;
	m_targetWind = 0;
	m_horizontalBlurShader = 0; 
	m_verticalBlurShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;
	float cameraX, cameraY, cameraZ;
	D3DXMATRIX baseViewMatrix;
	char videoCard[128];
	int videoMemory;

	
	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Set the initial position of the camera.
	cameraX = 50.0f;
	cameraY = 2.0f;
	cameraZ = -7.0f;

	m_Camera->SetPosition(cameraX, cameraY, cameraZ);




	// Create the terrain object.
	m_Terrain = new TerrainClass;
	if(!m_Terrain)
	{
		return false;
	}



	// Initialize the terrain object.
//	result = m_Terrain->Initialize(m_Direct3D->GetDevice(), "../Engine/data/heightmap01.bmp");
	result = m_Terrain->InitializeTerrain(m_Direct3D->GetDevice(), 128,128);   //initialise the flat terrain.
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// Create the timer object.
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition(cameraX, cameraY, cameraZ);

	// Create the fps object.
	m_Fps = new FpsClass;
	if(!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CpuClass;
	if(!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if(!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the text object.
	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Retrieve the video card information.
	m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());
	if(!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}

	// Create the terrain shader object.
	m_TerrainShader = new TerrainShaderClass;
	if(!m_TerrainShader)
	{
		return false;
	}

	// Initialize the terrain shader object.
	result = m_TerrainShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f,0.0f, 0.0f);


	// Create the color shader object used for render to texture
	m_TextureShader = new TextureShaderClass;

	// Initialize the color shader object.
	m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	//target window
	m_targetWind = new TargetWindowClass;

	// Initialize the render window shader object.
	m_targetWind->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth/2, screenHeight/2);
	
	// post processing initialise
	int downSampleWidth, downSampleHeight;

	// Set the size to sample down to.
	downSampleWidth = screenWidth / 2;
	downSampleHeight = screenHeight / 2;

	//intialising of render to texture , target window classes and shaders which are used in post processing.
	m_RenderTexture = new RenderTextureClass;
	m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);

	m_downSampleTexture = new RenderTextureClass;
	m_downSampleTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	m_downSampleWindow = new TargetWindowClass;
	m_downSampleWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, downSampleWidth, downSampleHeight);

	
	m_horizontalBlurTexture = new RenderTextureClass;
	m_horizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	m_horizontalBlurShader = new HorizontalBlurShaderClass;
	m_horizontalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
		
	m_verticalBlurTexture = new RenderTextureClass;
	m_verticalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);

	m_upSampleTexture = new RenderTextureClass;
	m_upSampleTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);

	
	m_upSampleWindow = new TargetWindowClass;
	m_upSampleWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);
	
	m_verticalBlurShader = new VerticalBlurShaderClass;
	m_verticalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the terrain shader object.
	if(m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	// Release the text object.
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the font shader object.
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the cpu object.
	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the position object.
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the timer object.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the terrain object.
	if(m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	if(m_horizontalBlurShader)
	{
		m_horizontalBlurShader->Shutdown();
		delete m_horizontalBlurShader;
		m_horizontalBlurShader = 0;
	}

	if(m_verticalBlurShader)
	{
		m_verticalBlurShader->Shutdown();
		delete m_verticalBlurShader;
		m_verticalBlurShader = 0;
	}

	return;
}


bool ApplicationClass::Frame()
{
	bool result;


	// Read the user input.
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}
	
	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Update the FPS value in the text object.
	result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}
	
	// Update the CPU usage value in the text object.
	result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
	if(!result)
	{
		return false;
	}

	// Render the graphics.
	result = RenderGraphics();
	if(!result)
	{
		return false;
	}

	return result;
}


bool ApplicationClass::HandleInput(float frameTime)
{
	bool keyDown, result;
	float posX, posY, posZ, rotX, rotY, rotZ;


	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_Input->IsSpacePressed();
	m_Terrain->GenerateHeightMap(m_Direct3D->GetDevice(), keyDown,"Normal");	

	keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = m_Input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = m_Input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = m_Input->IsZPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = m_Input->IsXPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = m_Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = m_Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	//increase the max height
	if(keyDown = m_Input->IsHPressed())
	{	
		if(ValDisplay >= 1.0f)
		{
			m_Terrain->ReduceVal();
			ValDisplay -= 1.0f;
		}
	}
	//press to increase turn chance
	if(keyDown = m_Input->IsPPressed())
	{
		if(ValChance < 99.0f)
		{
			m_Terrain->IncreaseChance();
			ValChance += 1.0f;
		}
	}
	//press to decrease turn chance
	if(keyDown = m_Input->IsOPressed())
	{
		if(ValChance >= 1.0f)
		{
			m_Terrain->ReduceChance();
			ValChance -= 1.0f;
		}
	}

	//increase he max height
	if(keyDown = m_Input->IsJPressed())
	{
		if(ValDisplay <= 100.0f)
		{
			m_Terrain->IncreaseVal();
			ValDisplay += 1.0f;
		}
	}
	//press to fault
	if(keyDown = m_Input->IsFPressed() && (!Pressed))
	{
		m_Terrain->GenerateHeightMap(m_Direct3D->GetDevice(), keyDown,"Fault");	
		Pressed = true;
	}
	

	//decrease drawmode
	if(keyDown = m_Input->IsNPressed() && (!Pressed))
	{
		
		if(DrawMode > 0)
		{
			m_Terrain->DecreaseMode();
			DrawMode = (Mode)((int)DrawMode - 1);
			m_TerrainShader->DecreaseMode();
		}
		Pressed = true;
	}
	//this turns of if none of the keys listed are pressed ( makes these keys one press only, so no more than one value increase on holding key once)
	if((keyDown = !m_Input->IsNPressed()) && (keyDown = !m_Input->IsMPressed()) && (keyDown = !m_Input->IsFPressed()) && (keyDown = !m_Input->IsUPressed()) && (keyDown = !m_Input->IsQPressed()) && (keyDown = !m_Input->IsEPressed()))
	{
		Pressed = false;
	}


	//increase drawmode
	if(keyDown = m_Input->IsMPressed()  && (!Pressed) )
	{
		
		if(DrawMode < 2)
		{
			m_Terrain->IncreaseMode();
			DrawMode = (Mode)((int)DrawMode + 1);
			m_TerrainShader->IncreaseMode();
		}
		Pressed = true;
	}
	
	//smooth the map
	if(keyDown = m_Input->IsUPressed()  && (!Pressed) )
	{
		m_Terrain->GenerateHeightMap(m_Direct3D->GetDevice(), keyDown,"Smooth");	
		Pressed = true;
	}

	//used to increase fault amount
	if(keyDown = m_Input->IsTPressed())
	{
		if(FaultAmount  < 100)
		{
			FaultAmount++;
			m_Terrain->IncreaseFaultAmount();
		}
	}
	//used to Decrease fault amount
	if(keyDown = m_Input->IsYPressed())
	{
		if(FaultAmount >1)
		{
			FaultAmount--;
			m_Terrain->DecreaseFaultAmount();
		}
	}

	if(DrawMode == 2)
	{
		//movement of diggerPosition
		if(keyDown = m_Input->IsAPressed())
		{
			if(m_TerrainShader->GetDiggerX() < 128)
			{
				m_TerrainShader->IncreaseDiggerX();
				m_Terrain->IncreasePositionX();
			}
		}

		if(keyDown = m_Input->IsDPressed())
		{
			if(m_TerrainShader->GetDiggerX() > 0)
			{
				m_TerrainShader->DecreaseDiggerX();
				m_Terrain->DecreasePositionX();
			}
		}
		if(keyDown = m_Input->IsSPressed())
		{
			if(m_TerrainShader->GetDiggerZ() < 128)
			{
				m_TerrainShader->IncreaseDiggerZ();
				m_Terrain->IncreasePositionZ();
			}
		}
		if(keyDown = m_Input->IsWPressed())
		{
			if(m_TerrainShader->GetDiggerZ() >0)
			{
				m_TerrainShader->DecreaseDiggerZ();
				m_Terrain->DecreasePositionZ();
			}
		}
		//end of movement input	
		//controls increasing of terrain by cursor
		if(keyDown = m_Input->IsVPressed())
		{
			m_Terrain->GenerateHeightMap(m_Direct3D->GetDevice(), keyDown,"Increase");
		}
		else if(keyDown = m_Input->IsBPressed())
		{
			m_Terrain->GenerateHeightMap(m_Direct3D->GetDevice(), keyDown,"Decrease");	
		}
	}

	//enabling and disabling of input
	if(m_Input->IsEnterPressed() && !m_Input->held[0])
	{
		if(post)
		{
			post = false;
		}
		else
		{
			post = true;
		}
		m_Input->held[0] = true;
	}
	if(!m_Input->IsEnterPressed())
	{
		m_Input->held[0] = false;
	}

	//controls for increasing and decreasing size of the digger
	if(m_Terrain->GetDiggerWidthHeight() < 30)
	{
		if(m_Input->IsEPressed()  && (!Pressed))
		{
			Pressed = true;
			m_Terrain->IncreaseDiggerWidthHeight();
			m_TerrainShader->IncreaseDiggerWidthHeight();

		
		}
	}
	//controls for increasing and decreasing size of the digger
	if(m_Terrain->GetDiggerWidthHeight() > 2)
	{
		if(m_Input->IsQPressed()  && (!Pressed))
		{
			Pressed = true;
			m_Terrain->DecreaseDiggerWidthHeight();
			m_TerrainShader->DecreaseDiggerWidthHeight();
		}
	}





	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Update the position values in the text object.
	result = m_Text->SetCameraPosition(DrawMode, FaultAmount, m_Terrain->GetDiggerWidthHeight(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}


	// Update the rotation values in the text object.
	result = m_Text->SetCameraRotation(rotX, ValChance, ValDisplay, m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	return true;
}



bool ApplicationClass::RenderSceneToTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	RenderScene();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;

}

bool ApplicationClass::RenderScene()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;


	// Clear the scene.
	m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Render the terrain buffers.
	m_Terrain->Render(m_Direct3D->GetDeviceContext());
	// Render the terrain using the terrain shader.
	result = m_TerrainShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
									 m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());
	if(!result)
	{
		return false;
	}

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();
		
	// Turn on the alpha blending before rendering the text.
	m_Direct3D->TurnOnAlphaBlending();

	// Render the text user interface elements.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	// Turn off alpha blending after rendering the text.
	m_Direct3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	//m_Direct3D->EndScene();

	return true;
}

bool ApplicationClass::DownSampleTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// Set the render target to be the render to texture.
	m_downSampleTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_downSampleTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	m_downSampleTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_downSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the small ortho window using the texture shader and the render to texture of the scene as the texture resource.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_downSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView());
	if(!result)
	{
	return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool ApplicationClass::RenderHorizontalBlurToTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;


	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeX = (float)m_horizontalBlurTexture->GetTextureWidth();

	// Set the render target to be the render to texture.
	m_horizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_horizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_horizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the down sample window vertex buffer on the graphics pipeline to prepare them for drawing.
	m_downSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the down sample window using the horizontal blur shader and the down sampled render to texture resource.
	m_horizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_downSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_downSampleTexture->GetShaderResourceView(), screenSizeX);
	

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}
bool ApplicationClass::RenderVerticalBlurToTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;


	// Store the screen height in a float that will be used in the vertical blur shader.
	screenSizeY = (float)m_verticalBlurTexture->GetTextureHeight();

	// Set the render target to be the render to texture.
	m_verticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_verticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();
	
	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_verticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the down sample window vertex bufferon the graphics pipeline to prepare them for drawing.
	m_downSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the down sample window using the vertical blur shader and the horizontal blurred render to texture resource.
	m_verticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_downSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_horizontalBlurTexture->GetShaderResourceView(), screenSizeY);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool ApplicationClass::UpSampleTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	// Set the render target to be the render to texture.
	m_upSampleTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_upSampleTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_upSampleTexture->GetOrthoMatrix(orthoMatrix);
	
	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the up sample window vertex buffer on the graphics pipeline to prepare them for drawing.
	m_upSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the up sample window using the texture shader and the small sized final blurred render to texture resource.
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_upSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, 
	
	m_verticalBlurTexture->GetShaderResourceView());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;

}
bool ApplicationClass::Render2DTextureScene()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(1.0f, 0.0f, 0.0f, 0.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the up sample window vertex buffer on the graphics pipeline to prepare them for drawing.
	m_upSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the up sample window using the texture shader and the full screen sized blurred render to texture resource.
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_upSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, 
	m_upSampleTexture->GetShaderResourceView());


	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

bool ApplicationClass::RenderGraphics()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix;
	bool result;
	
	if(post)
	{

		// First render the scene to a render texture.
		result = RenderSceneToTexture();
		if(!result)
		{
		return false;
		}
	
		// Next down sample the render texture to a smaller sized texture.
		result = DownSampleTexture();
		if(!result)
		{
			return false;
		}
	
		// Perform a horizontal blur on the down sampled render texture.
		result = RenderHorizontalBlurToTexture();
		if(!result)
		{
			return false;
		}

	
		// Now perform a vertical blur on the horizontal blur render texture.
		result = RenderVerticalBlurToTexture();
		if(!result)
		{
			return false;
		}
	
		// Up sample the final blurred render texture to screen size again.
		result = UpSampleTexture();
		if(!result)
		{
			return false;
		}
		// Render the blurred up sampled render texture to the screen.
		result = Render2DTextureScene();
		if(!result)
		{
			return false;
		}

	}
	else
	{
		
		//if post processing is not enabled then render normal scene
		m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

		RenderScene();

		// Present the rendered scene to the screen.
		m_Direct3D->EndScene();

	}
	

	return true;

}