
struct Mtrl
{
	float3 ambient;
	float3 diffuse;
	float3 spec;
	float  specPower;
};

struct DirLight
{
	float3 ambient;
	float3 diffuse;
	float3 spec;
	float3 dirW;  
};

uniform extern float4x4   Model;
uniform extern float4x4   MVP;
uniform extern float4x4 Bones[100];
uniform extern Mtrl       Material;
uniform extern DirLight   Light;
uniform extern float3     EyePos;
uniform extern texture    Tex;

sampler TexS = sampler_state
{
	Texture = <Tex>;
	// MinFilter = LINEAR;
	// MagFilter = LINEAR;
	// MipFilter = LINEAR;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	
	AddressU  = WRAP;
    AddressV  = WRAP;
	
};
 
struct OutputVS {
    float4 posH    : POSITION0;
    float3 normalW : TEXCOORD0;
    float3 toEyeW  : TEXCOORD1;
    float2 tex0    : TEXCOORD2;
};

OutputVS animVS( float3 posL    : POSITION0 , float3 normalL : NORMAL0 , float2 tex0    : TEXCOORD0, 
                 float4 bweight  : BLENDWEIGHT0,  int4 boneIndex : BLENDINDICES0){
	OutputVS outVS = (OutputVS)0;
    float4 p = bweight[0] * mul(float4(posL, 1.0f), Bones[boneIndex[0]]);
    p       += bweight[1] * mul(float4(posL, 1.0f), Bones[boneIndex[1]]);
    p       += bweight[2] * mul(float4(posL, 1.0f), Bones[boneIndex[2]]);
    p       += bweight[3] * mul(float4(posL, 1.0f), Bones[boneIndex[3]]);
	p.w=1.0f;
    float4 n = bweight[0] * mul(float4(normalL, 0.0f), Bones[boneIndex[0]]);
    n       += bweight[1] * mul(float4(normalL, 0.0f), Bones[boneIndex[1]]);
    n       += bweight[2] * mul(float4(normalL, 0.0f), Bones[boneIndex[2]]);
    n       += bweight[3] * mul(float4(normalL, 0.0f), Bones[boneIndex[3]]);
    n.w = 0.0f;
	outVS.normalW = mul( n , Model).xyz;
	float3 posW  = mul( p , Model).xyz;
	outVS.toEyeW = EyePos - posW;
	outVS.posH = mul( p , MVP);
	outVS.tex0 = tex0;
    return outVS;
}

OutputVS anim_sVS( float3 posL    : POSITION0 , float3 normalL : NORMAL0 , float2 tex0    : TEXCOORD0, 
                   float bweight  : BLENDWEIGHT0,  int boneIndex : BLENDINDICES0){
	OutputVS outVS = (OutputVS)0;
    float4 p = bweight * mul(float4(posL, 1.0f), Bones[boneIndex]);
	p.w=1.0f;
    float4 n = bweight * mul(float4(normalL, 0.0f), Bones[boneIndex]);
    n.w = 0.0f;
	outVS.normalW = mul( n , Model).xyz;
	float3 posW  = mul( p , Model).xyz;
	outVS.toEyeW = EyePos - posW;
	outVS.posH = mul( p , MVP);
	outVS.tex0 = tex0;
    return outVS;
}
OutputVS VS( float3 posL    : POSITION0 , float3 normalL : NORMAL0 , float2 tex0    : TEXCOORD0 ){
	OutputVS outVS = (OutputVS)0;
	outVS.normalW = mul( float4(normalL,0.0f) , Model).xyz;
		float3 posW  = mul( float4(posL,1.0f) , Model).xyz;
	outVS.toEyeW = EyePos - posW;
	outVS.posH = mul(float4(posL,1.0f) , MVP);
	outVS.tex0 = tex0;
    return outVS;
}

float4 PS(float3 normalW : TEXCOORD0, float3 toEyeW  : TEXCOORD1, float2 tex0 : TEXCOORD2) : COLOR {
	normalW = normalize(normalW);
	toEyeW  = normalize(toEyeW);
	// float3 lightVecW = -Light.dirW;
	// float3 r = reflect(Light.dirW, normalW);
	// float t  = pow(max(dot(r, toEyeW), 0.0f), Material.specPower);
	float s = max(dot(-Light.dirW, normalW), 0.0f);
	
	//float3 spec = t*(Material.spec*Light.spec).rgb;
	float3 diffuse = s*(Material.diffuse*Light.diffuse).rgb;
	float3 ambient = Material.ambient*Light.ambient;
	float4 texColor = tex2D(TexS, tex0);
	float3 color = (ambient + diffuse)*(texColor.rgb+float3(0.1,0.1,0.1));// + spec;
	
    return float4(color,1);
}

technique Tech {
    pass P0 {
        vertexShader = compile vs_2_0 VS();
        pixelShader  = compile ps_2_0 PS();
    }
}
technique AnimTech {
    pass P0 {
        vertexShader = compile vs_2_0 animVS();
        pixelShader  = compile ps_2_0 PS();
    }
}
technique Anim_sTech {
    pass P0 {
        vertexShader = compile vs_2_0 anim_sVS();
        pixelShader  = compile ps_2_0 PS();
    }
}

struct OutLineVS {
    float4 posH  : POSITION0;
    float3 color : COLOR0;
};
OutLineVS LineVS( float3 posL : POSITION0, float3 c : COLOR0) {
	OutLineVS outVS = (OutLineVS)0;
	outVS.posH = mul(float4(posL, 1.0f), MVP);
	outVS.color = c;
    return outVS;
}
float4 LinePS(float3 c : COLOR0) : COLOR {  return float4(c,1); }
technique LineTech {
    pass P0 {
        vertexShader = compile vs_2_0 LineVS();
        pixelShader  = compile ps_2_0 LinePS();
    }
}