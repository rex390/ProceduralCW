////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR3 normal;
	};

	struct HeightMapType 
	{ 
		float x, y, z;
		float nx, ny, nz;
	};

	struct VectorType 
	{ 
		float x, y, z;
	};
	enum Direction
	{
		 Forward = 0,
		 Backward = 1 ,
		 Left = 2 ,
		 Right = 3
	};
	struct DiggerPosition
	{
		int Position;
	};

	enum Mode
	{
		Terrain = 0,
		SewerSystem_CaveSystem = 1,
		Creation = 2,
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, char*);
	bool InitializeTerrain(ID3D11Device*, int terrainWidth, int terrainHeight);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool GenerateHeightMap(ID3D11Device* device, bool keydown,std::string);
	int  GetIndexCount();
	void ReduceVal();
	void IncreaseVal();
	void IncreaseChance();
	void ReduceChance();
	void IncreaseMode();
	void DecreaseMode();
	void IncreaseFaultAmount();
	void DecreaseFaultAmount();
	void FaultingTypeHorizontal();
	void FaultingTypeVertical();
	void FaultingTypeDiagonal();

	void SmoothCursor();

	//movement of position of digger ( for creation mode)
	void IncreasePositionX();
	void DecreasePositionX();
	void IncreasePositionZ();
	void DecreasePositionZ();
	int GetDiggerWidthHeight();
	void IncreaseDiggerWidthHeight();
	void DecreaseDiggerWidthHeight();
private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();
	float RandomHeight(float);
	float Smoothing(int,float);
	void DiggerCode();
	void IncreaseTerrain();
	void DecreaseTerrain();

	


	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	
private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	HeightMapType* m_heightMap;
	//used for mode 0 , and to tell the randomizer the maximum number it can randomize to.
	float Val;
	//tells us if fault is done
	bool Switch,FaultDone;
	Direction Movement;
	//used with the mode 2 , this changes what it looks, set to false but can be turned to true manually in the intializion
	bool DiggerWidthSize;
	//position of the diggers
	DiggerPosition Digger;
	//Fault amount tell us how much faults with do with one button press , while chance is how often the digger will turn
	int Chance,FaultAmount;
	//the position of the cursor
	int PositionX,PositionZ;
	//Mode struct which tell the program which drawmode it is
	Mode DrawMode;
	//temp creation position is used as a index , and to do changes on  while the creation position is the original value that is never changed once generated. DiggerWidthHeight is the width and height of the cursor
	int TempCreationPosition,CreationPosition,DiggerWidthHeight;
	//terrain fault type is used to tell us if it is a vertical horizontal or diagonal fault
	int TerrainFaultType, FaultCounter; 
	//column and row are used with mode 2
	int Column, Row; 
	//randomized to tell digger to go left or right
	int RightOrLeft;
	//variable used to check the digger should change position
	int ShouldTurn;
	//boolean used to tell us if position should change
	bool KeepDigging;
	
};

#endif