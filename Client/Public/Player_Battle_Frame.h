#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPlayer_Battle_Frame final : public CUI
{

	// ź���� ������ ������ �ó�� �����

public:
	typedef struct tagUIDesc
	{
		_bool    m_Is_Reverse = { false };
		_uint    m_Type = {};
		_uint    m_C_Num = {};
	}UIDESC;


private:
	CPlayer_Battle_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Battle_Frame(const CPlayer_Battle_Frame& rhs);
	virtual ~CPlayer_Battle_Frame() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private: /* ���� ����ϰ��ϴ� ������Ʈ�� ����� �˻����� �ٷ� ����� �� �ִ�. */	
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

public:
	void Set_UI();
	void Get_Mouse_Pos();
	void Change_Info(_double TimeDelta);
	void Tool_Funtion(_double TimeDelta);

private:
	UIDESC                  m_UI_Desc = {};

	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;	

	_bool                   m_Is_Reverse = { false };

	_float                  m_Alpha = { 1.f };

	_tchar					m_szX[MAX_PATH] = TEXT("");
	_tchar					m_szY[MAX_PATH] = TEXT("");

	_uint m_Level_Index = { 0 };

	_bool m_Is_CutScene = false;

	_bool m_Pt_In = { false };

	_float m_Size_Param = {};

	_int m_Origin_X = {};
	_int m_Origin_Y = {};

	_bool m_Is_Fix = { true };


private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();




public:
	static CPlayer_Battle_Frame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END