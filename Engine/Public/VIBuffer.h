#pragma once

#include "Component.h"

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	_uint	Get_NumVertices() { return m_iNumVertices; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT	Render();
	

protected:
	ID3D11Buffer*	m_pVB = { nullptr };
	ID3D11Buffer*	m_pIB = { nullptr };
protected:
	D3D11_BUFFER_DESC		m_BufferDesc;
	D3D11_SUBRESOURCE_DATA	m_SubresourceData;

protected:
	_uint					m_iNumVertexBuffers = { 0 };	//���ؽ� ������ ����
	_uint					m_iNumVertices = { 0 };			//������ ����
	_uint					m_iStride = { 0 };				//������ ũ�� (������ ����ü ũ��)
	_uint					m_iNumIndices = { 0 };			//�ε����� ���� (�ﰢ�� �ϳ� �� 3��)
	_uint					m_iIndexStride = { 0 };			//�ε����� ũ�� short/long
	DXGI_FORMAT				m_eIndexFormat;					/* �ε��� ũ�Ⱑ short�̸� R16, long�̸� R32*/
	D3D_PRIMITIVE_TOPOLOGY	m_eTopology;					/* ������ �׸��� ��� - �˻� */

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};