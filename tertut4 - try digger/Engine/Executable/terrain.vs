////////////////////////////////////////////////////////////////////////////////
// Filename: terrain.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ModeBuffer : register(cb1)
{
	int Mode;
	int PositionX;
	int PositionZ;
	int ExtraWidthHeight;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color: COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TerrainVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 GroundColor = float4(0.0f,0.0f,0.0f,0.0f);
	float4 MidWallColor = float4(0.0f,1.0f,0.0f,0.0f);
	float4 TopColor = float4(1.0f,1.0f,1.0f,0.0f);
	float4 textureColor;
	float slope,blendAmount;
	int ExtraWidth = 4;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	slope = input.position.y;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
	if(Mode == 1)
	{
		slope = input.position.y;
		if(slope == 5)
		{
			blendAmount = slope / 0.2f;
			//textureColor =GroundColor;
			textureColor = lerp(GroundColor, MidWallColor, blendAmount);
		}
	
		if((slope < 15) && (slope > 5))
		{
			blendAmount = (slope - 0.2f) * (1.0f / (0.7f - 0.2f));
			//textureColor = MidWallColor;
			textureColor = lerp(MidWallColor, TopColor, blendAmount);
		}
		if(slope >= 15) 
		{
			textureColor = TopColor;
		}
		output.color = textureColor;
	}
	//makes it black terrain so it easily seen
	else if(Mode ==0)
	{
		output.color = float4(1.0f,1.0f,1.0f,1.0f);
	}
	else if(Mode == 2)
	{
		
		if((input.position.x <= PositionX) &&  (input.position.x >= (PositionX - ExtraWidthHeight)) && (input.position.z > PositionZ) && (input.position.z < (PositionZ+ExtraWidthHeight)))
		{
			//blendAmount = (slope - 0.2f) * (1.0f / (0.7f - 0.2f));
			//textureColor = float4(1.0f,0.0f,1.0f,1.0f);
			//textureColor = lerp(float4(1.0f,0.0f,0.0f,1.0f), float4(0.0f,0.0f,1.0f,1.0f), blendAmount);

			blendAmount = 3 / 0.2f;
			//textureColor =GroundColor;
			textureColor = lerp(GroundColor, MidWallColor, blendAmount);
			//output.color = textureColor;
		}
		else
		{
			textureColor = TopColor;
			//textureColor =GroundColor;
			//textureColor = lerp(float4(0.0f,0.0f,1.0f,1.0f), float4(0.0f,1.0f,0.0f,1.0f), blendAmount);
		}
		
	

		output.color = textureColor;

	}
    return output;
}