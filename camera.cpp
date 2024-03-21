//=============================================================================
//
// �J�������� [camera.cpp]
// Author : GP11A132 21 �쎛�Č�
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	POS_X_CAM			(0.0f)			// �J�����̏����ʒu(X���W)
#define	POS_Y_CAM			(10.0f)			// �J�����̏����ʒu(Y���W)
#define	POS_Z_CAM			(-140.0f)		// �J�����̏����ʒu(Z���W)

//#define	POS_X_CAM		(0.0f)			// �J�����̏����ʒu(X���W)
//#define	POS_Y_CAM		(200.0f)		// �J�����̏����ʒu(Y���W)
//#define	POS_Z_CAM		(-400.0f)		// �J�����̏����ʒu(Z���W)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// �r���[���ʂ̎���p
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z		(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(10000.0f)										// �r���[���ʂ�FarZ�l

#define	VALUE_MOVE_CAMERA	(2.0f)										// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								// �J�����̉�]��

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CAMERA			g_Camera[MAX_CAMERA];		// �J�����f�[�^

static int				g_ViewPortType = TYPE_FULL_SCREEN;

bool isFirstPersonView = false;						// 1�l�̎��_���ǂ���
extern HWND hWnd;									// �E�B���h�E�̃n���h��
float sensitivity = 0.005f;							// �J�����̊��x�i���̒l�͎��ۂ̃Q�[���ɍ��킹�Ē������Ă��������j
float pitchLimit = 89.0f;							// �J�����̃s�b�`�p�̐����l�i�x�j���J�������]�h�~


//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	g_Camera[0].pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera[0].at = { 0.0f, 0.0f, 0.0f };
	g_Camera[0].up = { 0.0f, 1.0f, 0.0f };
	g_Camera[0].rot = { 0.0f, 0.0f, 0.0f };

	// ���_�ƒ����_�̋������v�Z
	float vx, vz;
	vx = g_Camera[0].pos.x - g_Camera[0].at.x;
	vz = g_Camera[0].pos.z - g_Camera[0].at.z;
	g_Camera[0].len = sqrtf(vx * vx + vz * vz);


	// �r���[�|�[�g�^�C�v�̏�����
	SetViewPort(g_ViewPortType);
}


//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{
	// �E�B���h�E�̃N���C�A���g�̈�̃T�C�Y���擾
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	// �E�B���h�E�̃N���C�A���g�̈�̒��S���W���v�Z
	int centerX = windowWidth / 2;
	int centerY = windowHeight / 2;

	// �}�E�X�̃f���^�i�ω��ʁj���v�Z
	int mouseX = GetMousePosX();					// �}�E�X�ʒu�擾�֐�
	int mouseY = GetMousePosY();					// �}�E�X�ʒu�擾�֐�
	int deltaX = mouseX - centerX;
	int deltaY = mouseY - centerY;


	// �J��������]������i�J�����I�u�W�F�N�g�ɍ��킹�Ă��������j
	if (deltaX || deltaY)							// �}�E�X���������ꍇ�̂�
	{
		g_Camera[0].rot.y += deltaX * sensitivity;	// Y����]
		g_Camera[0].rot.x += deltaY * sensitivity;	// X����]

		// �s�b�`�p�𐧌�����
		g_Camera[0].rot.x = max(-pitchLimit * XM_PI / 180.0f, min(g_Camera[0].rot.x, pitchLimit * XM_PI / 180.0f));

	}

	// �}�E�X���E�B���h�E�̒��S�Ƀ��Z�b�g
	POINT ptCenter = { centerX, centerY };
	ClientToScreen(hWnd, &ptCenter);				// �N���C�A���g���W����X�N���[�����W�֕ϊ�
	SetCursorPos(ptCenter.x, ptCenter.y);


	if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_LEFT_R))
	{// �����_����u���v
		g_Camera[0].rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera[0].rot.y < -XM_PI)
		{
			g_Camera[0].rot.y += XM_PI * 2.0f;
		}

		g_Camera[0].at.x = g_Camera[0].pos.x + sinf(g_Camera[0].rot.y) * g_Camera[0].len;
		g_Camera[0].at.z = g_Camera[0].pos.z + cosf(g_Camera[0].rot.y) * g_Camera[0].len;
	}

	if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT_R))
	{// �����_����u�E�v
		g_Camera[0].rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera[0].rot.y > XM_PI)
		{
			g_Camera[0].rot.y -= XM_PI * 2.0f;
		}

		g_Camera[0].at.x = g_Camera[0].pos.x + sinf(g_Camera[0].rot.y) * g_Camera[0].len;
		g_Camera[0].at.z = g_Camera[0].pos.z + cosf(g_Camera[0].rot.y) * g_Camera[0].len;
	}

	if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_UP_R))
	{// �����_�ړ��u��v
		g_Camera[0].rot.x -= VALUE_ROTATE_CAMERA;
		// �s�b�`�p�𐧌�����
		g_Camera[0].rot.x = max(-pitchLimit * XM_PI / 180.0f, min(g_Camera[0].rot.x, pitchLimit * XM_PI / 180.0f));

	}

	if (GetKeyboardPress(DIK_DOWN) || IsButtonPressed(0, BUTTON_DOWN_R))
	{// �����_�ړ��u���v
		g_Camera[0].rot.x += VALUE_ROTATE_CAMERA;
		// �s�b�`�p�𐧌�����
		g_Camera[0].rot.x = max(-pitchLimit * XM_PI / 180.0f, min(g_Camera[0].rot.x, pitchLimit * XM_PI / 180.0f));

	}
#ifdef _DEBUG

	if (GetKeyboardPress(DIK_U))
	{// �߂Â�
		g_Camera[0].len -= VALUE_MOVE_CAMERA;
		g_Camera[0].pos.x = g_Camera[0].at.x - sinf(g_Camera[0].rot.y) * g_Camera[0].len;
		g_Camera[0].pos.z = g_Camera[0].at.z - cosf(g_Camera[0].rot.y) * g_Camera[0].len;
	}

	if (GetKeyboardPress(DIK_M))
	{// �����
		g_Camera[0].len += VALUE_MOVE_CAMERA;
		g_Camera[0].pos.x = g_Camera[0].at.x - sinf(g_Camera[0].rot.y) * g_Camera[0].len;
		g_Camera[0].pos.z = g_Camera[0].at.z - cosf(g_Camera[0].rot.y) * g_Camera[0].len;
	}

	// �J�����������ɖ߂�
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}

#endif



#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// �J�����̍X�V
//=============================================================================
void SetCamera(void)
{
	// �r���[�}�g���b�N�X�ݒ�
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera[0].pos), XMLoadFloat3(&g_Camera[0].at), XMLoadFloat3(&g_Camera[0].up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera[0].mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera[0].mtxInvView, mtxInvView);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera[0].mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera[0].pos);
}


//=============================================================================
// �J�����̎擾
//=============================================================================
CAMERA* GetCamera(void)
{
	return &g_Camera[0];
}

//=============================================================================
// �r���[�|�[�g�̐ݒ�
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext* g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// �r���[�|�[�g�ݒ�
	switch (g_ViewPortType)
	{
	case TYPE_FULL_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_LEFT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_RIGHT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOWN_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2;
		break;


	}
	g_ImmediateContext->RSSetViewports(1, &vp);

}


int GetViewPortType(void)
{
	return g_ViewPortType;
}



// �J�����̎��_�ƒ����_���Z�b�g
void SetCameraAT(XMFLOAT3 pos)
{
	// �J�����̒����_�������̍��W�ɂ��Ă݂�
	g_Camera[0].at = pos;

	// �J�����̎��_���J������Y����]�ɑΉ������Ă���
	g_Camera[0].pos.x = g_Camera[0].at.x - sinf(g_Camera[0].rot.y) * g_Camera[0].len;
	g_Camera[0].pos.z = g_Camera[0].at.z - cosf(g_Camera[0].rot.y) * g_Camera[0].len;

}



//�J�����͕������ς�
void SetCameraPos(int index, XMFLOAT3 pos)
{
	//�J�����ʒu���󂯎�����ʒu(���̈ʒu)
	g_Camera[index].pos = pos;
	XMMATRIX mtxWorld, mtxRot, mtxTrf, mtxTranslate;
	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(0.0f, 0.0f, 1.0f);	//�O�x�N�g��
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Camera[0].rot.x, g_Camera[0].rot.y, g_Camera[0].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	XMFLOAT3 result = XMFLOAT3(mtxWorld.r[3].m128_f32[0], mtxWorld.r[3].m128_f32[1], mtxWorld.r[3].m128_f32[2]);

	//���[���h�ɕϊ������
	g_Camera[0].at = XMFLOAT3(pos.x + result.x, pos.y + result.y, pos.z + result.z);
}