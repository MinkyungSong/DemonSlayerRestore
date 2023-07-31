
/*Address Mode
 WRAP -> �ؽ�ó�� �ݺ� ���������� ��Ÿ�� 0.0, 1.0 �ؽ�ó�� �ѹ��� ���Ƽ� ������ ������ ����Ŵ
 MIRROR -> �ؽ�ó�� �ݺ������� ,�ݺ��Ǵ� �������� ��ǥ�� ���� ������ �����ư��鼭 ������
 CLAMP -> �ؽ�ó ��ǥ�� ��迡 �����ϸ� ��� ������ ����
 BORDER  -> �ؽ�ó ��ǥ�� �ؽ�ó�� ��迡 �����ϸ� ����ڰ� ������ ��� �������� �ؽ�ó�� ä��
 MIRROR_ONCE -> �ؽ�ó �ݺ������� ��ǥ 0,1 ������ ����� MIRRO�� �����ϰ� ������  -> �� �ݺ��� �ѹ��� �Ͼ
*/
sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;

	AddressU = WRAP;
	AddressV = WRAP;
};

sampler LinearClampSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

sampler PointClampSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};


/* For.RasterizerState */
RasterizerState		RS_Default
{
	FillMode = Solid;
	CullMode = BACK;
	FrontCounterClockwise = false;
};
RasterizerState		RS_Default2
{
	FillMode = Solid;
	CullMode = BACK;
	FrontCounterClockwise = true;
};

RasterizerState		RS_Wireframe
{
	FillMode = Wireframe;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState		RS_None
{
	FillMode = Solid;
	CullMode = NONE;
	FrontCounterClockwise = false;
};


RasterizerState		RS_CULL_CW
{
	FillMode = Solid;
	CullMode = FRONT;
	FrontCounterClockwise = false;
};

RasterizerState		RS_CULL_NONE
{
	FillMode = Solid;
	CullMode = NONE;
	FrontCounterClockwise = false;
};

BlendState	 BS_Default
{
	/* [0] : 0��° ����Ÿ�ٿ� �׸��� ������ �Ҳ��� ������.*/
	BlendEnable[0] = false;
};

BlendState	 BS_AlphaBlending
{
	BlendEnable[0] = true;

	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = Add;
};

BlendState	 BS_OneByOne
{
	BlendEnable[0] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = Add;
};


BlendState    BS_OneByOne_Engine
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = Add;
};

DepthStencilState DS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = LESS_EQUAL;
};

DepthStencilState DS_None_ZEnable
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = LESS_EQUAL;
};

DepthStencilState DS_None_ZEnable_None_ZWrite
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

DepthStencilState DS_None_Test
{
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = LESS_EQUAL;
};


