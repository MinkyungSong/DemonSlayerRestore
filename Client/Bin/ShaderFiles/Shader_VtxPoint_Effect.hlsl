#include "Shader_Defines.hpp"

/* �������� == ���(������Ʈ)���̺� */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;
texture2D		g_RampTexture;
texture2D		g_NoiseTexture1;
texture2D		g_NoiseTexture2;
texture2D		g_DistortionTexture;

float			g_fAlphaDiscard;

float			g_fTimeAcc = 0.f;
float4			g_vPanningSpeed = { 0.f, 0.f, 0.f, 0.f };
float2			g_vFlip = { 0.f, 0.f };
float			g_fDissolveAmount;
//float			g_fDissolveSoftness;
float			g_fDistortionStrength;
float			g_fDistortionSpeed;

vector			g_vCamPosition;

//--------------------------------------------------

float3			g_vSize;
vector			g_vColor;

/* ���̴��� �����ΰ�?! */

/* ���̴� : ��������� ������ ������ ����. */
/* DX9 : ������� ������ ������ ����. */

/* ID3DX11Effect pEffect;*/
/* �������� ���̴��� �׸�����. (Apply) */
/* m_pContext->DrawIndexed();*/

struct VS_IN
{
	float3	vPosition : POSITION;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	//Out.vPSize = In.vPSize;

	return Out;
}

struct GS_IN
{
	float4 vPosition : POSITION;
	//float2 vPSize : PSIZE;
};

struct GS_OUT
{
	float4	vPosition : SV_POSITION;
	float2	vTexUV : TEXCOORD0;
};

[maxvertexcount(20)]
void GS_DEFAULT(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT	Out[4];

	float3		vLook = g_vCamPosition.xyz - In[0].vPosition.xyz;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_vSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * g_vSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);


	Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.f, 0.f);

	Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.f, 0.f);

	Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.f, 1.f);

	Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(0.f, 1.f);

	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_vColor;

	return Out;
}

PS_OUT PS_DIFFUSE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_DIFFDISSOLVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vNoise = g_RampTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;

	// Dissolve ȿ�� ���
	float fDissolveFactor = vNoise.r;
	float fDissolveAmount = saturate((fDissolveFactor - g_fDissolveAmount) * 10.f);

	if (fDissolveAmount <= 0 && g_fDissolveAmount != 0)
		discard;

	// Softness - Transparency
	//float cutoff = saturate(g_fDissolveAmount + (1 - vDiffuse.a));
	//float alpha = smoothstep(cutoff, cutoff + g_fDissolveSoftness, vDiffuse.a);
	// Softness - Color
	//fixed4 rampCol = tex2D(_GradientMap, float2(col, 0)) + _BurnCol * smoothstep(orCol - cutoff, orCol - cutoff + _CutoffSoftness, _BurnSize) * smoothstep(0.001, 0.5, cutoff);
	//float4 rampCol = vDiffuse + smoothstep(0.001, 0.5, cutoff);
	//float4 finalCol = float4(rampCol.rgb * _Color.rgb * rampCol.a, 1);

	//Out.vColor = finalCol;

	//Out.vColor.a = alpha;

	return Out;
}

PS_OUT PS_MASKRAMP(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	vector vRamp;

	vRamp = g_RampTexture.Sample(LinearSampler, float2(vMask.r, 0.f));
	Out.vColor = vRamp;
	Out.vColor.a = vMask.r;
	
	if (Out.vColor.a < 0.05f)
		discard;

	return Out;
}

PS_OUT PS_DIFFDISTORTION(PS_IN In)
{
	PS_OUT   Out = (PS_OUT)0;

	vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexUV);
	float2 fWeight;
	fWeight.x = cos(vDistortion.r * g_fTimeAcc *g_fDistortionSpeed) * g_fDistortionStrength;
	fWeight.y = sin(vDistortion.r * g_fTimeAcc *g_fDistortionSpeed) * g_fDistortionStrength;

	vector vMtrlEffect = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + fWeight);
	Out.vColor = vMtrlEffect;

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MASKDISTORTION_NONEZ(PS_IN In)
{
	PS_OUT   Out = (PS_OUT)0;

	Out.vColor = float4(0.f, 0.f, 0.f, 0.f);

	vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexUV);
	vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	float2 fWeight;
	fWeight.x = cos(vDistortion.r * g_fTimeAcc *g_fDistortionSpeed) * vMask.r * g_fDistortionStrength;
	fWeight.y = sin(vDistortion.r * g_fTimeAcc *g_fDistortionSpeed) * vMask.r * g_fDistortionStrength;

	vector vMtrlEffect = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + fWeight);
	Out.vColor = vMtrlEffect;

	return Out;
}


/* ������ �ϵ��� �´� ���̴��������� ���̴��� �������ֱ� ����. */
technique11 DefaultTechnique {
	pass Default // 0
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default , float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT();
	}

	pass Diffuse // 1
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSE();
	}

	pass DiffuseDissolve	// 2
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFDISSOLVE();
	}

	pass MaskRamp	// 3
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKRAMP();
	}

	pass DiffuseDistortion		// 4
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFDISTORTION();
	}

	pass MaskDistortionNoneZ		// 5
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DS_None_ZEnable_None_ZWrite, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_DEFAULT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASKDISTORTION_NONEZ();
	}
};