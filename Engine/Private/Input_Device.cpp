#include "..\public\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
	: m_pInputSDK(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}


CInput_Device::~CInput_Device()
{
	Free();
}

_bool CInput_Device::Get_DIKeyDown(_ubyte ubyKeyID)
{
	if (!m_isKeyDown[ubyKeyID] && (m_byKeyState[ubyKeyID] & 0x80))
		return true;
	
	if (m_isKeyDown[ubyKeyID] && !(m_byKeyState[ubyKeyID] & 0x80))
		return false;

	return false;
}

_bool CInput_Device::Get_DIKeyUp(_ubyte ubyKeyID)
{
	if (m_isKeyDown[ubyKeyID] && !(m_byKeyState[ubyKeyID] & 0x80))
		return true;

	if (!m_isKeyDown[ubyKeyID] && (m_byKeyState[ubyKeyID] & 0x80))
		return false;

	return false;
}

_bool CInput_Device::Get_DIMouseDown(MOUSEKEYSTATE eMouseID)
{
	if (!m_isMouseDown[eMouseID] && (m_MouseState.rgbButtons[eMouseID] & 0x80))
		return true;

	if (m_isMouseDown[eMouseID] && !(m_MouseState.rgbButtons[eMouseID] & 0x80))
		return false;

	return false;
}

_bool CInput_Device::Get_DIMouseUp(MOUSEKEYSTATE eMouseID)
{
	if (m_isMouseDown[eMouseID] && !(m_MouseState.rgbButtons[eMouseID] & 0x80))
		return true;

	if (!m_isMouseDown[eMouseID] && (m_MouseState.rgbButtons[eMouseID] & 0x80))
		return false;

	return false;
}

_bool CInput_Device::Get_AnyKeyPressing()
{
	for (_uint i = 0; i < 256; i++)
	{
		if (m_byKeyState[i])
			return true;
	}

	for (_uint i = 0; i < 4; i++)
	{
		if (m_MouseState.rgbButtons[i])
			return true;
	}

	return false;
}

HRESULT CInput_Device::Ready_DInput(HINSTANCE hInst, HWND hWnd)
{
	// �θ� �İ�ü ����
	DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr);

	// �� ��ǻ�Ϳ� ����Ǿ� �ִ� Ű���� ��ü ����
	m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr);

	// ������ Ű���� ��ü�� ���� ������ �İ�ü���� ����
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ������ Ű���� ��ü�� ���� ������ ����
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ������ Ű���� �İ�ü�� Ȱ��ȭ
	m_pKeyBoard->Acquire();

	// �� ��ǻ�Ϳ� ����Ǿ� �ִ� ���콺 ��ü ����
	m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr);

	// ������ ���콺 ��ü�� ���� ������ �İ�ü���� ����
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ������ ���콺 ��ü�� ���� ������ ����
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ������ ���콺 �İ�ü�� Ȱ��ȭ
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Update_DInput()
{
	for (_uint i = 0; i < 256; i++)
	{
		if (m_byKeyState[i] & 0x80)
			m_isKeyDown[i] = true;
		else
			m_isKeyDown[i] = false;
	}

	for (_uint i = 0; i < DIM_END; i++)
	{
		if (m_MouseState.rgbButtons[i] & 0x80)
			m_isMouseDown[i] = true;
		else
			m_isMouseDown[i] = false;
	}

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

void Engine::CInput_Device::Free()
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
