#include "pch.h"
#include "..\Public\Level_Tool.h"

#include "GameInstance.h"

#include "Camera_Tool.h"
#include "Player_Tool.h"
#include "MapObject.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Tool::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);
    SetWindowText(g_hWnd, TEXT("Tool"));

}

HRESULT CLevel_Tool::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Lights()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    LIGHTDESC LightDesc;
    ZeroMemory(&LightDesc, sizeof LightDesc);

    LightDesc.eType = LIGHTDESC::TYPE_DIRECTION;
    LightDesc.vLightDir         = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vLightDiffuse     = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vLightAmbient     = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vLightSpecular    = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : CDirection_Light");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CCamera::CAMERADESC CameraDesc;
    ZeroMemory(&CameraDesc, sizeof(CameraDesc));

    CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
    CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

    CameraDesc.fFovY = XMConvertToRadians(60.f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNearZ = 0.3f;
    CameraDesc.fFarZ = 1000.f;

    CameraDesc.TransformDesc.dSpeedPerSec = 10.0;
    CameraDesc.TransformDesc.dRadianRotationPerSec = XMConvertToRadians(90.f);
    CameraDesc.dSensitivity = 0.1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, pLayerTag,
        TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : CCamera_Tool");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Player(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CCharacter_Tool::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

    CharacterDesc.WorldInfo.vScale = _float3(1.f, 1.f, 1.f);
    CharacterDesc.WorldInfo.fDegree = 0.f;
    CharacterDesc.WorldInfo.vPosition = _float4(0.f, 0.f, 0.f, 1.f);

    CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
    CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);

    CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
    CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_AABB].vPosition = _float3(0.f, CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_AABB].vSize.y * 0.5f, 0.f);

    CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
    CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
    CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_OBB].vPosition = _float3(0.f, CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_OBB].vSize.y * 0.5f, 0.f);

    CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_SPHERE].vSize = _float3(1.f, 1.f, 1.f);
    CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_SPHERE].vPosition = _float3(0.f, CharacterDesc.ColliderDesc[CCharacter_Tool::COLL_SPHERE].vSize.x, 0.f);

    CharacterDesc.NaviDesc.iCurrentIndex = 0;
    CharacterDesc.NaviDesc.vStartPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    
    // AnimTool¿ë
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, pLayerTag,
        TEXT("Prototype_GameObject_AnimCharacter_Tool"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : AnimCharacter_Tool");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_MapObject(const _tchar* pLayerTag)
{
    Load_MapObject_Info(TEXT("../../Data/Object/Acaza_Battle/Acaza_Test.dat"), pLayerTag);

    return S_OK;
}

HRESULT CLevel_Tool::Load_MapObject_Info(const _tchar* pPath, const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    HANDLE hFile = CreateFile(pPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    _ulong			dwByte = 0;
    _ulong			dwStrByte = 0;
    _uint			iSize = 0;

    CGameObject* pGameObject = { nullptr };

    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iSize; ++i)
    {
        CMapObject::MAPOBJECT_INFO tMapObject_Info;
        ZeroMemory(&tMapObject_Info, sizeof tMapObject_Info);

        ReadFile(hFile, &tMapObject_Info.vPos, sizeof(_float4), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.vRotAngle, sizeof(_float3), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.vScale, sizeof(_float3), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.iMapObjectType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iNumInstance, sizeof(_uint), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fRange, sizeof(_float), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fMinSize, sizeof(_float), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fMaxSize, sizeof(_float), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iInstanceType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.bRandomRatationY, sizeof(_bool), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.iArrangementType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iSceneType, sizeof(_uint), &dwByte, nullptr);


        ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.szMeshName, dwStrByte, &dwByte, nullptr);

        const _tchar* pMapObjectTypeTag = TEXT("");

        switch (tMapObject_Info.iMapObjectType)
        {
        case CMapObject::MAPOBJECT_STATIC:
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_StaticMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_TERRAIN:
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_TerrainMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_ROTATION:
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_RotationMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_INSTANCE:
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_InstanceMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        default:
            break;
        }
    }

    CloseHandle(hFile);

    Safe_Release(pGameInstance);

    return S_OK;
}


CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Tool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Tool::Free()
{
    __super::Free();
}