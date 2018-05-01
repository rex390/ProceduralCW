////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "terrainclass.h"
#include <cmath>
#include <time.h> 
#include <stdlib.h> 

TerrainClass::TerrainClass()
{
	//intializing of variables
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_heightMap = 0;
	Val = 3.0f;
	m_terrainGeneratedToggle = false;
	Movement = Forward;
	Chance = 60;
	DrawMode = Terrain;
	FaultAmount = 1;
	PositionX = 64;
	PositionZ = 64;
	DiggerWidthHeight =2;
	FaultDone = false;
}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}

bool TerrainClass::InitializeTerrain(ID3D11Device* device, int terrainWidth, int terrainHeight)
{
	int index;
	float height = 0.0;
	bool result;
	srand (time(NULL));

	// Save the dimensions of the terrain.
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	// Create the structure to hold the terrain data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	// Initialise the data in the height map (flat).
	for(int j=0; j<m_terrainHeight; j++)
	{
		for(int i=0; i<m_terrainWidth; i++)
		{			
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

		}
	}


	//even though we are generating a flat terrain, we still need to normalise it. 
	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}
bool TerrainClass::Initialize(ID3D11Device* device, char* heightMapFilename)
{
	bool result;


	// Load in the height map for the terrain.
	result = LoadHeightMap(heightMapFilename);
	if(!result)
	{
		return false;
	}

	// Normalize the height of the height map.
	NormalizeHeightMap();

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}


void TerrainClass::Shutdown()
{
	// Release the vertex and index buffer.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();

	return;
}


void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int TerrainClass::GetIndexCount()
{
	return m_indexCount;
}

bool TerrainClass::GenerateHeightMap(ID3D11Device* device, bool keydown,std::string Draw)
{

	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if(keydown&&(!m_terrainGeneratedToggle))
	{
		int index;
		float height = 0.0;
		
		
		//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
		//if the button pressed , also send a string saying normal we just generate a normal terrain 
		if(Draw.compare("Normal") == 0)
		{
			for(int j=0; j<m_terrainHeight; j++)
			{
				for(int i=0; i<m_terrainWidth; i++)
				{			
					index = (m_terrainHeight * j) + i;

					m_heightMap[index].x = (float)i;
					if(DrawMode == 0) // if mode was 0 , it means we want to randomize the height based on the max height
					{
						m_heightMap[index].y = RandomHeight(Val);
					}
					else if (DrawMode == 1) //if mode was 1, it means we use digger so we want max height to be 20
					{
						m_heightMap[index].y = 20;
					}
					m_heightMap[index].z = (float)j;
				}
			}
		}
		//if the button pressed is f then we fault the terrain
		if(Draw.compare("Fault") == 0)
		{
			TerrainFaultType;
			//reset both variable so faulting will restart again
			FaultDone = false;
			FaultCounter = 0;
			//what type of fault we do and do it until fault iteration value has been reached
			while(!FaultDone)
			{
				TerrainFaultType = (rand() % 3);;
				switch(TerrainFaultType)
				{
				case 0: FaultingTypeHorizontal();
						break;
				case 1: FaultingTypeVertical();
						break;
				case 2: FaultingTypeDiagonal();
						break;
				}
				FaultCounter++;
				//when we reach the fault amount we stop looping
				if(FaultCounter == FaultAmount)
				{
					FaultDone = true;
				}
			}
		}
		//when smoothing button is pressed
		if(DrawMode != 2)
		{
			if(Draw.compare("Smooth") == 0)
			{
					for(int j=0; j<m_terrainHeight; j++)
					{
						for(int i=0; i<m_terrainWidth; i++)
						{			
							index = (m_terrainHeight * j) + i;
							m_heightMap[index].y =Smoothing(index,Val );
						}
					}
			
			}
		}
		//if space bar is pressed and the mode was 1 we use digger code
		if(Draw.compare("Normal") == 0)
		{
			if (DrawMode == 1)
			{
				DiggerCode();
			}

		}		
		//if draw mode is 2 , it means we are in creation mode
		if(DrawMode == 2)
		{

			if(Draw.compare("Smooth") == 0)
			{
				SmoothCursor();
			}
			//if key V was pressed then we increase terrain 
			if(Draw.compare("Increase") == 0)
			{
				IncreaseTerrain();
			}
			//if key B was pressed then we decrease the terrain
			else if(Draw.compare("Decrease") == 0)
			{
				DecreaseTerrain();
			}
		}
		
		result = CalculateNormals();
		if(!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if(!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}

	


	return true;
}
bool TerrainClass::LoadHeightMap(char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;


	// Open the height map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k=0;

	// Read the image data into the height map.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			height = bitmapImage[k];
			
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			k+=3;
		}
	}

	// Release the bitmap image data.
	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}

 
void TerrainClass::NormalizeHeightMap()
{
	int i, j;


	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}

	return;
}


bool TerrainClass::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(m_terrainHeight-1) * (m_terrainWidth-1)];
	if(!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for(j=0; j<(m_terrainHeight-1); j++)
	{
		for(i=0; i<(m_terrainWidth-1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i+1);
			index3 = ((j+1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;
		
			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;
		
			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight-1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if(((i-1) >= 0) && ((j-1) >= 0))
			{
				index = ((j-1) * (m_terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if((i < (m_terrainWidth-1)) && ((j-1) >= 0))
			{
				index = ((j-1) * (m_terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if(((i-1) >= 0) && (j < (m_terrainHeight-1)))
			{
				index = (j * (m_terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if((i < (m_terrainWidth-1)) && (j < (m_terrainHeight-1)))
			{
				index = (j * (m_terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}
			
			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
			
			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete [] normals;
	normals = 0;

	return true;
}


void TerrainClass::ShutdownHeightMap()
{
	if(m_heightMap)
	{
		delete [] m_heightMap;
		m_heightMap = 0;
	}

	return;
}


bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index1, index2, index3, index4;

	ShutdownBuffers();
	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for(j=0; j<(m_terrainHeight-1); j++)
	{
		for(i=0; i<(m_terrainWidth-1); i++)
		{
			index1 = (m_terrainHeight * j) + i;          // Bottom left.
			index2 = (m_terrainHeight * j) + (i+1);      // Bottom right.
			index3 = (m_terrainHeight * (j+1)) + i;      // Upper left.
			index4 = (m_terrainHeight * (j+1)) + (i+1);  // Upper right.

			// Upper left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indices[index] = index;
			index++;

			// Upper right.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Upper right.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom right.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void TerrainClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
void TerrainClass::ReduceVal()
{
	if(Val >= 1.0f)
	{
		Val -= 1.0f;
	}
}

void TerrainClass::IncreaseVal()
{
	if(Val <= 100.0f)
	{
		Val+=1.0f;
	}
}
float TerrainClass::RandomHeight(float MaxHeight)
{
	float Height;
	Height = ((rand()%10) * 0.1) * MaxHeight;
	return Height;
}
float TerrainClass::Smoothing(int index,float MaxHeight)
{
if((index - m_terrainWidth - 1) >  0)
		{
			m_heightMap[index].y = ((m_heightMap[index-1].y + m_heightMap[index + 1].y + 
			m_heightMap[index + m_terrainWidth].y + m_heightMap[index + m_terrainWidth - 1 ].y + m_heightMap[index + m_terrainWidth + 1].y +
			m_heightMap[index - m_terrainWidth].y + m_heightMap[index - m_terrainWidth - 1 ].y + m_heightMap[index - m_terrainWidth + 1].y)/8);
		}
		else
		{
			m_heightMap[index].y = ((m_heightMap[index-1].y + m_heightMap[index + 1].y + 
			m_heightMap[index + m_terrainWidth].y + m_heightMap[index + m_terrainWidth - 1 ].y + m_heightMap[index + m_terrainWidth + 1].y)/5);
		}

		if(m_heightMap[index].y > MaxHeight)
		{
			m_heightMap[index].y = MaxHeight;
		}
		else if(m_heightMap[index].y < 0)
		{
			m_heightMap[index].y =0;
		}
	return m_heightMap[index].y;
}



void TerrainClass::DiggerCode()
{
	//boolean used to tell us if position should change
	KeepDigging = true;
	int positionX,positionZ,Position ;
	//randomized to tell digger to go left or right
	RightOrLeft;
	//variable used to check the digger should change position
	ShouldTurn;
	//pick a point where digger starts
	positionX = (rand() % 128);
	positionZ = (rand() % 128);
	//make position the index
	Position = (m_terrainHeight * positionZ) + positionX;
	m_heightMap[Position].y =	5;
	DiggerWidthSize = false;
	//keep looping till keep digging is false, signalling it is complete
	while(KeepDigging)
	{
		//randomize a value to be used to tell us if digger will turn
		ShouldTurn = (rand() % 100) ;
		//checks it with the chance you set to see if turn should be made
		if(ShouldTurn < Chance)
		{
			//randomize the variable this will be used with a  switch statement
			RightOrLeft = (rand() % 2);
			switch(RightOrLeft)
			{
				//turn right case statement
				case 0:	if(Movement == Left) //if left , then a right turn would make it forward
						{
							Movement = Forward;
						}
						else if(Movement == Right) //if right already  then another right turn would make it backwards
						{
							Movement = Backward;
						}
						else if(Movement!= Right) //if it is not right then make it right (only happen if direction is forward or backwards)
						{
							Movement = Right;
						}
						break;
				//turn left case statement
				case 1: if(Movement == Right) //if it is already right then left turn would make movement forward
						{
							Movement = Forward;
						}
						else if(Movement == Left) //if it is left then another left turn would make it backwards
						{
							Movement = Backward;
						}
						else if(Movement != Left) //if it is not left then make it left ( only wards if bckward or forward)
						{
							Movement = Left;
						}
						break;
			}
		}
			//we move the digger based on direction
			switch(Movement)
			{					//make sure it doesnt leave the array
				case Forward : if(Position<16255)
								{
									Position += 128;
								}
								break;
				case Right : Position += 1;
							break;
				case Left:	Position -= 1;
							break;
							//make sure it doesnt leave the array
				case Backward:if(Position>129)
								{
									Position -= 128;
								}
							break;
			}
		
		//decrease the height
		m_heightMap[Position].y =	5;
		//turn on or false manually, this will make the terrain look different
		if(DiggerWidthSize)
		{
			//if position is less than this value ( width * height - 1) (which is 128*128 -1)
			if(Position<16255)
			{
				m_heightMap[Position+128].y =	5;
			}
			//if it is greater than this value then we can do a decrease in value which wont leave array
			if(Position >129)
			{
				m_heightMap[Position-128].y =	5;
			}
		}
		//when digger position hits end of area then stop digging
		if((Position < 0 ) || (Position > 128*128))
		{
			KeepDigging = false;
		}
		//when it hits this area also stop digger
		if((Position >=0) && (Position <= 128) || (Position >= (128*128)) )
		{
			KeepDigging = false;
		}
	}
		//so we dont create so many map when button is pressed
		Sleep(100);
}
//increase the turn chance for how often digger will switch direction
void TerrainClass::IncreaseChance()
{
	Chance += 1;
}
//Decrease the turn chance for how often digger will switch direction
void TerrainClass::ReduceChance()
{
	Chance -= 1;
}
//increase the mode
void TerrainClass::IncreaseMode()
{
	DrawMode = (Mode)((int)DrawMode + 1);
}
//decrease the mode
void TerrainClass::DecreaseMode()
{
	DrawMode = (Mode)((int)DrawMode - 1);
}
//NEEDS RECODING A LITTLE
void TerrainClass::FaultingTypeDiagonal()
{
	//position at which faulting takes place
	int FaultPosition = 0;
	//holds the position for when you go down one row and need starting position back
	int FaultHoldStart = 0;
	//where faulting needs to end in row
	int EndOfRow = 0;
	//boolean controlling when faulting is done in while loop
	bool FaultDone = false;
	//randomize all variable to get v alue we need
	FaultPosition = (rand() % m_terrainWidth);
	FaultHoldStart = FaultPosition;
	int FaultCounter = (rand() % m_terrainHeight);
	//tells us if we fault upwards or downwards
	int FaultValue = 0;
	//randomize the value
	int FaultValueType = (rand() % 2);
	//increasing the terrain
	if( FaultValueType == 0)
	{
		FaultValue = (rand() % 10)+1;
	}
	//decreasing the terrain
	else
	{
		FaultValue = ((rand() % 10)+1) * (-1);
	}
	FaultDone = false;



	for(int Loop = 0; Loop < FaultCounter;Loop++)
	{
		FaultDone = false;
		while(!FaultDone)
		{

			if(FaultPosition == EndOfRow)
			{
				FaultPosition = FaultHoldStart + 127;
				FaultHoldStart += (m_terrainWidth - 1) ;
 				EndOfRow += 128;
				FaultDone = true;
			}
			if(FaultPosition <= 0 )
			{
				FaultDone = true;
			}
			else if (FaultPosition > 0 )
			{
				m_heightMap[FaultPosition].y += FaultValue;	
				FaultPosition --;
			}
			
			
		}
	}


}


void TerrainClass::FaultingTypeVertical()
{
		bool FaultDone = false;
		//how far down in the array it will go
		int FaultIteration = m_terrainHeight;
		//Where faulting will end ( acts as a counter that decreases and when hits zero it will end fault)
		int FaultEndPoint =(rand() % m_terrainWidth)   ;
		//determines if increase or decrease the position in array
		int ValueAdded = 0;
		//the position where we increment from
		int StartFault = 0; //(rand() % m_terrainWidth);
		int Position = 0;//StartFault;
		//determines which side you fault from 
		int WhichSideFault = (rand() % 2);
		int FaultValue = 0;
		//Used to determine if the fault will increase or decrease the terrain
		int TerrainFaultType =(rand() % 2);
		// 0 = fault the terrain up
		if(TerrainFaultType == 0)
		{
			FaultValue = (rand() % 10)+1;
		}
		//1 = fault the terrain downwards
		else if (TerrainFaultType == 1)
		{
			FaultValue = (-1) * ((rand() % 10) +1 );
		}
		//moves from left side of terrain to end point
		if(WhichSideFault == 0) 
		{
			StartFault =0;
			Position = StartFault;
			ValueAdded =1;
		}
		//moves from the right side of terraint o the end point
		else
		{
			//width * the height and  minus one since , zero is a value in the array   ((128 * 128) -1)
			StartFault = m_terrainWidth -1 ;
			Position = StartFault;
			ValueAdded = (-1);
		}

		//loop until fault action completed
		while(!FaultDone)
		{
			//if we hit end point then fault is complete
			if(FaultIteration <= 0)
			{

				//end the while loop
				FaultDone = true;
			}
			//makes sure it does not leave the array
			if(FaultEndPoint > 16384)
			{
				FaultEndPoint = 16384;
			}
			//tells us which side it will fault from
			if(WhichSideFault == 0)
			{
				//makes us go to next row/column based on fault type (which side we fault from)
				if(Position >= FaultEndPoint)
				{
					//take away from the counter of faulting
					FaultIteration--;
					StartFault += 128;
					Position = StartFault;
					FaultEndPoint += 128;
				}
			}
			else
			{
				//makes us go to next row/column based on fault type (which side we fault from)
				if(Position <= FaultEndPoint)
				{
					//take away from the counter of faulting
					FaultIteration--;
					StartFault += 128;
					Position = StartFault;
					FaultEndPoint += 128;
				}
			}

			
				//increment height
				m_heightMap[Position].y += FaultValue;	
				//change position to where to increment next
				Position += ValueAdded;
			
			

		}

}
void TerrainClass::FaultingTypeHorizontal()
{
		bool FaultDone = false;
		//Where faulting will end
		int FaultEndPoint =(rand() % m_terrainWidth) * 127 ;
		//determines if increase or decrease the position in array
		int ValueAdded = 0;
		//the position where we increment from
		int StartFault= 0;
		//determines which side you fault from 
		int WhichSideFault = (rand() % 2);
		int FaultValue = (rand() % 10)+1;
		//used to determine when we hit the end point
		//moves from left side of terrain to end point
		if(WhichSideFault == 0) 
		{
			StartFault =0;
			ValueAdded =1;


		}
		//moves from the right side of terraint o the end point
		else
		{
			//width * the height and  minus one since , zero is a value in the array   ((128 * 128) -1)
			StartFault = ((128 * 128) -1) ;
			ValueAdded = (-1);
		}
		//loop until fault action completed
		while(!FaultDone)
		{
			//if we hit end point then fault is complete
			if(StartFault == FaultEndPoint)
			{
				FaultDone = true;
			}

			m_heightMap[StartFault].y += FaultValue;
			
			//add the value in which direction to go
			StartFault += ValueAdded;
		}
}


void TerrainClass::IncreaseFaultAmount()
{
	FaultAmount++;
}

void TerrainClass::DecreaseFaultAmount()
{
	FaultAmount--;
}

void TerrainClass::IncreasePositionZ()
{
	PositionZ++;
}

void TerrainClass::DecreasePositionZ()
{
	PositionZ--;
}

void TerrainClass::IncreasePositionX()
{
	PositionX++;
}

void TerrainClass::DecreasePositionX()
{
	PositionX--;
}
void TerrainClass::IncreaseTerrain()
{

	CreationPosition = (m_terrainHeight * PositionZ) + PositionX;
	for(Column = 0; Column < (m_terrainHeight * DiggerWidthHeight); Column+=128)
	{
		for(Row =0; Row <DiggerWidthHeight ;Row++)
		{
			//CreationPosition = CreationPosition - Row + Column;
			TempCreationPosition = CreationPosition - Row + Column;
			if(TempCreationPosition >= 0)
			{
				if(TempCreationPosition <= (m_terrainHeight* m_terrainWidth))
				{
					m_heightMap[CreationPosition - Row + Column].y += 1;
				}
			}
		}
	}

	
}
void TerrainClass::DecreaseTerrain()
{
	CreationPosition = (m_terrainHeight * PositionZ) + PositionX;
	
	for(Column = 0; Column < (m_terrainHeight * DiggerWidthHeight); Column+=128)
	{
		for(Row =0; Row <DiggerWidthHeight ;Row++)
		{
			TempCreationPosition = CreationPosition - Row + Column;
			if(TempCreationPosition >= 0)
			{
				if(TempCreationPosition <= (m_terrainHeight* m_terrainWidth))
				{
					m_heightMap[TempCreationPosition].y -= 1;
				}
			}
		}
	}
	

}

void TerrainClass::IncreaseDiggerWidthHeight()
{
	DiggerWidthHeight++;
}

void TerrainClass::DecreaseDiggerWidthHeight()
{
	DiggerWidthHeight--;
}
int TerrainClass::GetDiggerWidthHeight()
{
	return DiggerWidthHeight;
}

void TerrainClass::SmoothCursor()
{
	CreationPosition = (m_terrainHeight * PositionZ) + PositionX;
	//go through column and row so you access all of the index of the box shape
	for(Column = 0; Column < (m_terrainHeight * DiggerWidthHeight); Column+=128)
	{
		for(Row =0; Row <DiggerWidthHeight ;Row++)
		{
			//use temp as the index
			TempCreationPosition = CreationPosition - Row + Column;
			//set the value to correct value if it tries to go out of array
			if(TempCreationPosition > (m_terrainHeight * m_terrainWidth ))
			{
				TempCreationPosition = (m_terrainHeight * m_terrainWidth );
			}
			//set value to correct value if it tries to go out of array
			else if(TempCreationPosition < 0)
			{
				TempCreationPosition =0;
			}
			//use these neighbours if it is within these indexs
			if(TempCreationPosition < 128)
			{
				m_heightMap[TempCreationPosition].y = ((m_heightMap[TempCreationPosition-1].y + m_heightMap[TempCreationPosition + 1].y + 
				m_heightMap[TempCreationPosition + m_terrainWidth].y + m_heightMap[TempCreationPosition + m_terrainWidth - 1 ].y + m_heightMap[TempCreationPosition + m_terrainWidth + 1].y )/5);

			}
			//use these neighbours if it is within these indexs
			else if(TempCreationPosition >  ((m_terrainHeight * m_terrainWidth ) - 128))
			{
				m_heightMap[TempCreationPosition].y = ((m_heightMap[TempCreationPosition-1].y + m_heightMap[TempCreationPosition + 1].y +
				m_heightMap[TempCreationPosition - m_terrainWidth].y + m_heightMap[TempCreationPosition - m_terrainWidth - 1 ].y + m_heightMap[TempCreationPosition - m_terrainWidth + 1].y)/5);
			}
			//use these neighbours if it is within these indexs
			else if((TempCreationPosition > 127) && (TempCreationPosition < ((m_terrainHeight * m_terrainWidth )-128)))
			{
					//used to smooth the position
					m_heightMap[TempCreationPosition].y = ((m_heightMap[TempCreationPosition-1].y + m_heightMap[TempCreationPosition + 1].y + 
					m_heightMap[TempCreationPosition + m_terrainWidth].y + m_heightMap[TempCreationPosition + m_terrainWidth - 1 ].y + m_heightMap[TempCreationPosition + m_terrainWidth + 1].y +
					m_heightMap[TempCreationPosition - m_terrainWidth].y + m_heightMap[TempCreationPosition - m_terrainWidth - 1 ].y + m_heightMap[TempCreationPosition - m_terrainWidth + 1].y)/8);
			}
		}
	}
}
