// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo

Texture2D textureMap;
SamplerState textureSampler;

// Global variables
cbuffer cbPerObject {
	float4x4 worldInverseTranspose;
	float4x4 worldViewProj;
	//Material gMaterial;
};

cbuffer cbDirectionalLights {
	float3 lightDir;
};


// Shaders
struct VertexIn {
    float3 pos : POSITION; 
    float4 color : COLOR; 
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};



struct VertexOut { 
    float4 posH : SV_POSITION; 
    float4 color : COLOR; 
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

RasterizerState mainState {
	FillMode = SOILD;
	CullMode = None;
};

VertexOut RenderSceneVS(VertexIn vin) {
    VertexOut vout; 
	vout.color = vin.color;
    vout.posH = mul(float4(vin.pos, 1.0f), worldViewProj);
    vout.normal = normalize(mul(float4(vin.normal, 1.0f), worldInverseTranspose).xyz);
    vout.texCoord = vin.texCoord;
    return vout;
}

float4 RenderScenePS(VertexOut pin) : SV_TARGET {
    return pin.color; 
}

float4 RenderAmbientPS(VertexOut pin) : SV_TARGET0 {
    return 0.3*textureMap.Sample(textureSampler, pin.texCoord);
}

float4 RenderDirectionalLightPS(VertexOut pin) : SV_TARGET0 {
	float4 tex = textureMap.Sample(textureSampler, pin.texCoord);
	float intensity = dot(normalize(pin.normal), normalize(lightDir));
    return intensity * tex;
}

// Techniques
technique10 RenderScene {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
        SetPixelShader( CompileShader( ps_4_0, RenderAmbientPS() ) );
    
		SetRasterizerState( mainState ); 

	}

	pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
        SetPixelShader( CompileShader( ps_4_0, RenderDirectionalLightPS() ) );
    
		SetRasterizerState( mainState );  
	}

}

