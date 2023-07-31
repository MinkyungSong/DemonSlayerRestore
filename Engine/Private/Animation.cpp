#include "..\Public\Animation.h"

#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_AnimationDesc(rhs.m_AnimationDesc)
	/*m_dDuration(rhs.m_dDuration)
	, m_dTickPerSecond(rhs.m_dTickPerSecond)
	, m_dTimeAcc(rhs.m_dTimeAcc)
	, m_isFinish(rhs.m_isFinish)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_iCurrentKeyFrames(rhs.m_iCurrentKeyFrames)*/
{
	for (auto& pChannel : m_AnimationDesc.m_Channels)
		Safe_AddRef(pChannel);

	strcpy_s(m_AnimationDesc.m_szName, rhs.m_AnimationDesc.m_szName);
}

HRESULT CAnimation::Initialize(ANIMATIONDATA* pAnimationData, CModel* pModel)
{
	_uint iSize = { 0 };
	strcpy_s(m_AnimationDesc.m_szName, pAnimationData->szName);
	m_AnimationDesc.m_dDuration = pAnimationData->dDuration;
	m_AnimationDesc.m_dTickPerSecond = pAnimationData->dTickPerSecond;

	m_AnimationDesc.m_iNumChannels = pAnimationData->iNumChannels;

	m_AnimationDesc.m_iCurrentKeyFrames.resize(m_AnimationDesc.m_iNumChannels);
	
	for (_uint i = 0; i < m_AnimationDesc.m_iNumChannels; i++)
	{
		iSize = { 0 };
		char szName[MAX_PATH] = { "" };
		strcpy_s(szName, pAnimationData->pChannelData[i].szName);

		//�� �ִϸ��̼ǿ��� �����̴� ���� �̸��� ���� ���� ���� ã�� pBone�� ����
		CBone * pBone = pModel->Get_Bone(szName);

		CChannel* pChannel = CChannel::Create(&pAnimationData->pChannelData[i], szName, pModel->Get_BoneIndex(pBone->Get_Name()));
		if (nullptr == pChannel)
			return E_FAIL;

		m_AnimationDesc.m_Channels.emplace_back(pChannel);
	}


	return S_OK;
}

_int CAnimation::Invalidate_TransformationMatrices(CModel* pModel, _double dTimeDelta, _bool Play, _bool Combo)
{
	if (m_isFirst_EventCall)
	{
		m_isFirst_EventCall = false;

		for (auto& event : m_ControlDesc.m_vecTime_Event)
		{
			event.m_isFirst = true;
		}
	}

	m_AnimationDesc.m_isFinish = false;

	/* ���� ����Ǵ� �ִϸ��̼� */
	if(Play)
		m_AnimationDesc.m_dTimeAcc += m_AnimationDesc.m_dTickPerSecond * dTimeDelta * m_ControlDesc.m_fAnimationSpeed;


	// RootAnimation ��
	CChannel* pRoot = Get_Channel("Root");
	pRoot->Set_Root(true);

	_uint	iIndex = { 0 };
	for (auto& pChannel : m_AnimationDesc.m_Channels)
	{
		//�� �ִϸ��̼ǿ��� �����̴� ������ ���¸� �ð��� �°� �����Ѵ�.
		pChannel->Invalidate(pModel, m_AnimationDesc.m_iCurrentKeyFrames[iIndex++], m_AnimationDesc.m_dTimeAcc);
	}


	m_RootPosition = pRoot->Get_RootPosition();
	

	// ��� duration ����
	if (m_ControlDesc.m_isCombo && Combo || m_isEarlyEnd)
	{
		_double ComboDuration = m_AnimationDesc.m_dDuration - 0.35f;
		if (ComboDuration <= m_AnimationDesc.m_dTimeAcc)
		{
			// ��ü ����ð����� �����ð��� Ŀ���� == �ִϸ��̼��� ������
			m_AnimationDesc.m_isFinish = true;
			m_AnimationDesc.m_dTimeAcc = 0.0;
		}
	}
	else
	{
	if (m_AnimationDesc.m_dDuration <= m_AnimationDesc.m_dTimeAcc)
	{
		// ��ü ����ð����� �����ð��� Ŀ���� == �ִϸ��̼��� ������
		m_AnimationDesc.m_isFinish = true;
		m_AnimationDesc.m_dTimeAcc = 0.0;
	}
	}


	_uint	index = 0;
	// EventCall �ߵ� ����
	for (auto& event : m_ControlDesc.m_vecTime_Event)
	{
		if (event.m_isFirst && event.m_dTime <= m_AnimationDesc.m_dTimeAcc)
		{
			event.m_isFirst = false;

			//�Ʒ��� �̺�Ʈ �� �Լ� �ڵ带 �ۼ�
			m_ControlDesc.m_isEventCall = true;
			m_ControlDesc.m_iEventIndex = index;
		}
		index++;
	}


	// �ִϸ��̼��� ���� ��, �����ִ��� �ƴϸ�,  ���� �ִϸ��̼� �ε����� return
	if (m_AnimationDesc.m_isFinish)
	{
		// EventCall �ʱ�ȭ
		for (auto& event : m_ControlDesc.m_vecTime_Event)
		{
			event.m_isFirst = true;
		}

		if(m_ControlDesc.m_isCombo && Combo)
			return m_ControlDesc.m_iConnect_ComboAnim;
		else
			return m_ControlDesc.m_iConnect_Anim;
	}

	//�׳� ��ҿ��� -1 �� return(�ε����� �������� �ʴ�)
	return -1;
}

_bool CAnimation::Invalidate_Linear_TransformationMatrices(CModel* pModel, _double dTimeDelta, _bool Play, vector<KEYFRAME> vecLastKey )
{
	m_AnimationDesc.m_isFinish = false;

	/* ���� ����Ǵ� �ִϸ��̼� */
	if (Play)
		m_AnimationDesc.m_dTimeAcc += m_AnimationDesc.m_dTickPerSecond * dTimeDelta * m_ControlDesc.m_fAnimationSpeed;


	// RootAnimation ��
	CChannel* pRoot = Get_Channel("Root");
	pRoot->Set_Root(true);



	_uint	iIndex = { 0 };
	for (auto& pChannel : m_AnimationDesc.m_Channels)
	{
		//�� �ִϸ��̼ǿ��� �����̴� ������ ���¸� �ð��� �°� �����Ѵ�.
		pChannel->Invalidate_Linear(pModel, vecLastKey[iIndex++], m_AnimationDesc.m_dTimeAcc);
	}

	m_RootPosition = pRoot->Get_RootPosition();


	// ����duration
	if (0.09f <= m_AnimationDesc.m_dTimeAcc)
	{
		// ��ü ����ð����� �����ð��� Ŀ���� == �ִϸ��̼��� ������
		m_AnimationDesc.m_isFinish = true;
		m_AnimationDesc.m_dTimeAcc = 0.0;
	}


	// �ִϸ��̼��� ���� ��, �������� ��. false ����
	if (m_AnimationDesc.m_isFinish)
	{
		return false;
	}
	// �������� ��� ���� true ����
	return true;
}
/*
_float4 CAnimation::Get_Pos_RootAnimation(CTransform* pTransformCom)
{
	if (m_AnimationDesc.m_dTimeAcc == 0.0)
	{
		XMStoreFloat4(&m_Save_RootPos, pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

	_float4 fPos;
	XMStoreFloat4(&fPos, pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float3 RootPosition = m_RootPosition;
	_float4x4 RootWorldConvert = pTransformCom->Get_WorldFloat4x4();
	_float3 FinalRootPos = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&FinalRootPos, XMVector3TransformCoord(XMLoadFloat3(&RootPosition), XMLoadFloat4x4(&RootWorldConvert)));

	_float4 Final = { -FinalRootPos.x * 0.01f, FinalRootPos.y * 0.01f , -FinalRootPos.z * 0.01f, 1.f };
	// �÷��̾��� ���� ��ġ�� �������� Root bone�� ��ġ�� ��ȭ��Ŵ
	_float4  SubPos = { m_Save_RootPos.x + Final.x, m_Save_RootPos.y + Final.y , m_Save_RootPos.z + Final.z , 1.f };

	pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&SubPos));

	return _float4();
}*/

CChannel* CAnimation::Get_Channel(const char* pChannelName)
{
	auto iter = find_if(m_AnimationDesc.m_Channels.begin(), m_AnimationDesc.m_Channels.end(), [&](CChannel* pChannel)->_bool
		{
			if (false == strcmp(pChannel->Get_Name(), pChannelName))
				return true;

			return false;
		}
	);

	if (iter == m_AnimationDesc.m_Channels.end())
		return nullptr;

	return *iter;
}

vector<KEYFRAME> CAnimation::Get_LastKeys()
{
	vector<KEYFRAME> LastKeys;

	for (auto& pChannel : m_AnimationDesc.m_Channels)
	{
		//�� �ִϸ��̼ǿ��� �����̴� ������ ���¸� �ð��� �°� �����Ѵ�.
		//pChannel->Invalidate(pModel, m_AnimationDesc.m_iCurrentKeyFrames[iIndex++], m_AnimationDesc.m_dTimeAcc);
		KEYFRAME ChannelLastKeyFrame = pChannel->Get_LastKeyFrame();
		LastKeys.emplace_back(ChannelLastKeyFrame);
	}

	return LastKeys;
}

CAnimation* CAnimation::Create(ANIMATIONDATA* pAnimationData, class CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimationData, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_AnimationDesc.m_Channels)
		Safe_Release(pChannel);

	m_AnimationDesc.m_Channels.clear();
}
