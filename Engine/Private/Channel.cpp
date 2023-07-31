#include "..\Public\Channel.h"

#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CHANNELDATA* pChannelData, const char* pName, _uint iBoneIndex)
{
	strcpy_s(m_szName, pName);

	m_iBoneIndex = iBoneIndex;

	m_iNumKeyFrames = pChannelData->iNumKeyFrames;

	_float3		vScale = { 0.f, 0.f, 0.f };
	_float4		vRotation = { 0.f, 0.f, 0.f, 0.f };
	_float3		vPosition = { 0.f, 0.f, 0.f };

	for (_uint i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME KeyFrame;
		ZeroMemory(&KeyFrame, sizeof KeyFrame);
	
		_uint iKey = { 0 };
		iKey = pChannelData->pKeyFrameData[i].iScaleKey;
		if (i < iKey)
		{
			vScale = pChannelData->pKeyFrameData[i].vScale;
			KeyFrame.dTime = pChannelData->pKeyFrameData[i].dScaleTime;
		}

		iKey = pChannelData->pKeyFrameData[i].iRotationKey;
		if (i < iKey)
		{
			vRotation = pChannelData->pKeyFrameData[i].vRotation;
			KeyFrame.dTime = pChannelData->pKeyFrameData[i].dRotationTime;
		}

		iKey = { 0 };
		iKey = pChannelData->pKeyFrameData[i].iPositionKey;
		if (i < iKey)
		{
			vPosition = pChannelData->pKeyFrameData[i].vPosition;
			KeyFrame.dTime = pChannelData->pKeyFrameData[i].dPositionTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Invalidate(CModel* pModel, _uint& pCurrentKeyFrame, _double TrackPosition)
{
	if (0.0 == TrackPosition)
		pCurrentKeyFrame = 0;

	//Ư�� �ִϸ��̼��� �ð��� ���� ���� ���¸� �����Ѵ�.
	_float3		vScale;
	_float4		vRotation;
	_float3		vPosition;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();
	//�� �ִϸ��̼��� ������ Ű ������

	if (TrackPosition >= LastKeyFrame.dTime)
	{
		/*��Ȥ ��ü ����ð� ������ Ű �������� ������ ��� ���� ����ð� ���� 
		* ������ ���¸� ���������ֱ� ���� ����ó��
		*/
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vPosition = LastKeyFrame.vPosition;
	}
	else
	{
		if (m_dSave_TrackPosition == TrackPosition)
			m_isReverse = false;
		if (m_dSave_TrackPosition < TrackPosition)
			m_isReverse = false;
		else if (TrackPosition < m_dSave_TrackPosition)
			m_isReverse = true;

		
		//�Ϲ����
		if (m_isReverse == false)
		{
			while (TrackPosition >= m_KeyFrames[pCurrentKeyFrame + 1].dTime)
				++pCurrentKeyFrame;
			//��� �ð��� ���� Ű���������� �Ѿ�� ���� Ű �������� ������Ų��
		}
		//�����
		else
		{
			while (TrackPosition < m_KeyFrames[pCurrentKeyFrame].dTime)
				--pCurrentKeyFrame;
		}

		_double dRatio = (TrackPosition - m_KeyFrames[pCurrentKeyFrame].dTime)
			/ (m_KeyFrames[pCurrentKeyFrame + 1].dTime - m_KeyFrames[pCurrentKeyFrame].dTime);
		/*�� Ű ������ �������� ���� �󸶳� ����ƴ��� 0 ~ 1
		* 1���� Ŀ���� ���� Ű���������� �Ѿ�� - ���� if���� ����
		*/

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[pCurrentKeyFrame].vScale;
		vDestScale = m_KeyFrames[pCurrentKeyFrame + 1].vScale;

		vSourRotation = m_KeyFrames[pCurrentKeyFrame].vRotation;
		vDestRotation = m_KeyFrames[pCurrentKeyFrame + 1].vRotation;

		vSourPosition = m_KeyFrames[pCurrentKeyFrame].vPosition;
		vDestPosition = m_KeyFrames[pCurrentKeyFrame + 1].vPosition;
		
		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), (_float)dRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), (_float)dRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), (_float)dRatio));


		/*�� ������ ������ ���¸� ���������ϴ� �۾� - Lerp, Slerp
		* Ű������ �������� - Sour
		* Ű������ ������ - Dest
		*/
	}

	_float4 vTranslation = _float4(vPosition.x, vPosition.y, vPosition.z, 1.f);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(
		XMLoadFloat3(&vScale),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation),
		XMLoadFloat4(&vTranslation));

	// ��Ʈ ä���� ���.
	if (m_isRoot)
	{
		m_RootPosition = vPosition;
	}
	else
	{
		pModel->Get_Bone(m_iBoneIndex)->Set_TransformationMatrix(TransformationMatrix);
		//������ ������ ���·� ����� �����, �ش� ��ķ� ���� �����Ѵ�
	}
	
	//����
	m_dSave_TrackPosition = TrackPosition;
}

void CChannel::Invalidate_Linear(CModel* pModel, KEYFRAME LastKeyFrame_Prev, _double TrackPosition )
{
	// LastKeyFrame_Prev : ���� Ű�������� �������� ó���� �ȴ�.

	//if (0.0 == TrackPosition)
	//	pCurrentKeyFrame = 0;

	//Ư�� �ִϸ��̼��� �ð��� ���� ���� ���¸� �����Ѵ�.
	_float3		vScale;
	_float4		vRotation;
	_float3		vPosition;

	KEYFRAME	LastKeyFrame = m_KeyFrames.front();
	//�� �ִϸ��̼��� ������ Ű ������

	// ����duration
	if (TrackPosition >= 0.09f)
	{
		/*��Ȥ ��ü ����ð� ������ Ű �������� ������ ��� ���� ����ð� ����
		* ������ ���¸� ���������ֱ� ���� ����ó��
		*/
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vPosition = LastKeyFrame.vPosition;
	}
	else
	{
		// ����duration
		_double dRatio = TrackPosition / 0.09f;
		/*�� Ű ������ �������� ���� �󸶳� ����ƴ��� 0 ~ 1
		* 1���� Ŀ���� ���� Ű���������� �Ѿ�� - ���� if���� ����
		*/

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = LastKeyFrame_Prev.vScale;
		vDestScale = m_KeyFrames.front().vScale;

		vSourRotation = LastKeyFrame_Prev.vRotation;
		vDestRotation = m_KeyFrames.front().vRotation;

		
		vSourPosition = LastKeyFrame_Prev.vPosition;
		vDestPosition = m_KeyFrames.front().vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), (_float)dRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), (_float)dRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), (_float)dRatio));

		//��Ʈä���� ��ġ ����
		if (m_isRoot)
		{
			vPosition = { 0.0f, 0.0f, 0.0f };
		}

		/*�� ������ ������ ���¸� ���������ϴ� �۾� - Lerp, Slerp
		* Ű������ �������� - Sour
		* Ű������ ������ - Dest
		*/
	}

	_float4 vTranslation = _float4(vPosition.x, vPosition.y, vPosition.z, 1.f);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(
		XMLoadFloat3(&vScale),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation),
		XMLoadFloat4(&vTranslation));
	
	// ��Ʈ ä���� ���.
	if (m_isRoot)
	{
		m_RootPosition = vPosition;
	}
	else
	{
		pModel->Get_Bone(m_iBoneIndex)->Set_TransformationMatrix(TransformationMatrix);
		//������ ������ ���·� ����� �����, �ش� ��ķ� ���� �����Ѵ�
	}
	
	
}

KEYFRAME CChannel::Get_LastKeyFrame()
{
	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	return LastKeyFrame;
}

CChannel* CChannel::Create(CHANNELDATA* pChannelData, const char* pName, _uint iBoneIndex)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pChannelData, pName, iBoneIndex)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}
