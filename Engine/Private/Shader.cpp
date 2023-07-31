#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_iNumPasses(rhs.m_iNumPasses)
	, m_pEffect(rhs.m_pEffect)
	, m_pTechnique(rhs.m_pTechnique)
	, m_InputLayouts(rhs.m_InputLayouts)
{
	for (_uint i = 0; i < m_iNumPasses; i++)
		Safe_AddRef(m_InputLayouts[i]);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElemets)
{
	_uint iHlslFlag = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

	//��ο� �ִ� ���̴� ������ �ڵ忡�� ����� �� �ֵ��� pEffect ��ü�� ����
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	//���� pEffect���� 0��° technique11��ü�� �޾ƿ�
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == m_pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);

	//Technique ��ü�� ���� ������ �޾ƿ� (passes�� ����ϱ� ����)
	m_pTechnique->GetDesc(&TechniqueDesc);
	
	m_iNumPasses = TechniqueDesc.Passes;

	//Technique ��ü�� pass �� ��ŭ �ݺ�
	//�� ���̴� ���Ͼ��� pass �� ��ŭ �ݺ��Ѵ�
	for (_uint i = 0; i < m_iNumPasses; i++)
	{
		//���� InputLayout�� ���� ����
		ID3D11InputLayout* pInputLayout = { nullptr };
		//i�� pass��ü�� �޾ƿ�
		ID3DX11EffectPass* pPass = m_pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);

		//pass��ü�� ���� ������ �޾ƿ� (InputLayout�� ���� �� �ʿ�)
		pPass->GetDesc(&PassDesc);

		//InputLayout�� ���� �� 3, 4�� ���ڸ� �־��� PassDesc�� ������, �� ������ ����� ���� ���� ������ ��������
		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElemets, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		//���� ��ü�� ���Ϳ� �߰�
		m_InputLayouts.emplace_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::SetUp_RawValue(const char* pConstantName, const void* pValue, _uint iByteSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pValue, 0, iByteSize);
}

HRESULT CShader::SetUp_Vector(const char* pConstantName, const _float4* pVector)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectVectorVariable* pVectorVariable = pVariable->AsVector();
	if (nullptr == pVectorVariable)
		return E_FAIL;

	//�Ű������� ���� ����� ���� ���̴��� ���������� ����
	return pVectorVariable->SetFloatVector((const _float*)pVector);
}

//���̴��� ���� �������� �Լ�
HRESULT CShader::SetUp_Matrix(const char* pConstantName, const _float4x4* pMatrix)
{
	//��� ���ڿ��� ���� �̸��� ���������� ��ü�� ���� - ���� ���� ��ü
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	//��ü�� ��ķ� ĳ���� - ���̴��� ���������� ����� �ƴ� ��� ���� �߻�
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	//�Ű������� ���� ����� ���� ���̴��� ���������� ����
	return pMatrixVariable->SetMatrix((const _float*)pMatrix);
}

HRESULT CShader::SetUp_Matrix_Array(const char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((const _float*)pMatrix, 0, iNumMatrices);
}

HRESULT CShader::SetUp_SRV(const char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	//��� ���ڿ��� ���� �̸��� ���������� ��ü�� ���� - ���� ���� ��ü
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	//��ü�� ���̴� ���ҽ��� ĳ����
	ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVVariable)
		return E_FAIL;

	//�Ű������� ���� �ؽ�ó(���ҽ��� ��ȯ��)�� ���� ���̴��� �ؽ�ó ���� ������ ����
	return pSRVVariable->SetResource(pSRV);
}

HRESULT CShader::SetUp_SRV_Array(const char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTextures)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVVariable)
		return E_FAIL;

	return pSRVVariable->SetResourceArray(ppSRVs, 0, iNumTextures);
}

//Begin�� ȣ��Ǳ� ����(�� ���� Apply�� ȣ��Ǳ� ����)
//�ݵ�� ���̴��� ����� ��������(���̴��� ���޹޾ƾ� �� ��)��
//��� �� ������ ������ �Ѵ�.
HRESULT CShader::Begin(_uint iPassIndex)
{
	if (m_iNumPasses <= iPassIndex)
		return E_FAIL;

	//DX9���� SetFVF()�� ����� �������� ��
	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	ID3DX11EffectPass* pEffectPass = m_pTechnique->GetPassByIndex(iPassIndex);
	if (nullptr == pEffectPass)
		return E_FAIL;

	pEffectPass->Apply(0, m_pContext);

	return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElemets)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElemets)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);

	for (_uint i = 0; i < m_iNumPasses; i++)
		Safe_Release(m_InputLayouts[i]);
}
