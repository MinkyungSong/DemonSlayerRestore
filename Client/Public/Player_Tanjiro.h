#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer_Tanjiro final : public CPlayer
{
public:
	enum ANIM {
		ANIM_ATK_COMBO = 21,
		ANIM_ATK_AIRCOMBO = 29,
		ANIM_ATK_CHARGE = 31,
		ANIM_ATK_SKILL_GUARD = 34, ANIM_ATK_SKILL_MOVE = 38, ANIM_ATK_SKILL_NORMAL = 42,
		ANIM_ATK_THROW = 47, ANIM_ATK_THROW_CUTSCENE = 48,
		ANIM_ATK_AIRTRACK = 49,
		ANIM_BATTLE_GUARD = 63, ANIM_BATTLE_GUARD_HIT_BIG = 66, ANIM_BATTLE_GUARD_HIT_SMALL = 67, ANIM_BATTLE_GUARD_PUSH = 68,
		ANIM_BATTLE_DASH = 79,
		ANIM_BATTLE_IDLE = 82,
		ANIM_BATTLE_JUMP = 83,
		ANIM_BATTLE_RUN = 87, ANIM_BATTLE_RUN_END = 89,
		ANIM_BATTLE_STEP_AB = 91, ANIM_BATTLE_STEP_AF = 92, ANIM_BATTLE_STEP_AL = 93, ANIM_BATTLE_STEP_AR = 94,
		ANIM_BATTLE_STEP_B = 95, ANIM_BATTLE_STEP_F = 96, ANIM_BATTLE_STEP_L = 97, ANIM_BATTLE_STEP_R = 99,
		ANIM_END = 116
	};

private:
	CPlayer_Tanjiro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Tanjiro(const CPlayer_Tanjiro& rhs);
	virtual ~CPlayer_Tanjiro() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


private: //�ִϸ��̼� ����� �Լ���
	void	EventCall_Control(_double dTimeDelta);

	void	Animation_Control(_double dTimeDelta);
	void	Animation_Control_Battle_Move(_double dTimeDelta);
	void	Animation_Control_Battle_Jump(_double dTimeDelta);
	void	Animation_Control_Battle_Attack(_double dTimeDelta);
	void	Animation_Control_Battle_Charge(_double dTimeDelta);
	void	Animation_Control_Battle_Skill(_double dTimeDelta);
	void	Animation_Control_Battle_Guard(_double dTimeDelta);
	void	Animation_Control_Battle_Dash(_double dTimeDelta);

	void	Moving_Restrict();



private: //�ִϸ��̼� ����� ������
	//Move
	_float	m_fMove_Speed = { 2.f };

	_bool	m_isJump_Move = { false };


private:
	/* �ӽ� �ڵ� */
	_uint	m_iNumAnim = { 0 };
	// ���� Ȯ�ο�
	_uint	m_iMeshNum = { 0 };



private:
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;



private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CPlayer_Tanjiro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END