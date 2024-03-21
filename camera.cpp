//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : GP11A132 21 野寺翔悟
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	POS_X_CAM			(0.0f)			// カメラの初期位置(X座標)
#define	POS_Y_CAM			(10.0f)			// カメラの初期位置(Y座標)
#define	POS_Z_CAM			(-140.0f)		// カメラの初期位置(Z座標)

//#define	POS_X_CAM		(0.0f)			// カメラの初期位置(X座標)
//#define	POS_Y_CAM		(200.0f)		// カメラの初期位置(Y座標)
//#define	POS_Z_CAM		(-400.0f)		// カメラの初期位置(Z座標)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(10000.0f)										// ビュー平面のFarZ値

#define	VALUE_MOVE_CAMERA	(2.0f)										// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								// カメラの回転量

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAMERA			g_Camera[MAX_CAMERA];		// カメラデータ

static int				g_ViewPortType = TYPE_FULL_SCREEN;

bool isFirstPersonView = false;						// 1人称視点かどうか
extern HWND hWnd;									// ウィンドウのハンドル
float sensitivity = 0.005f;							// カメラの感度（この値は実際のゲームに合わせて調整してください）
float pitchLimit = 89.0f;							// カメラのピッチ角の制限値（度）←カメラ反転防止


//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	g_Camera[0].pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera[0].at = { 0.0f, 0.0f, 0.0f };
	g_Camera[0].up = { 0.0f, 1.0f, 0.0f };
	g_Camera[0].rot = { 0.0f, 0.0f, 0.0f };

	// 視点と注視点の距離を計算
	float vx, vz;
	vx = g_Camera[0].pos.x - g_Camera[0].at.x;
	vz = g_Camera[0].pos.z - g_Camera[0].at.z;
	g_Camera[0].len = sqrtf(vx * vx + vz * vz);


	// ビューポートタイプの初期化
	SetViewPort(g_ViewPortType);
}


//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	// ウィンドウのクライアント領域のサイズを取得
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	// ウィンドウのクライアント領域の中心座標を計算
	int centerX = windowWidth / 2;
	int centerY = windowHeight / 2;

	// マウスのデルタ（変化量）を計算
	int mouseX = GetMousePosX();					// マウス位置取得関数
	int mouseY = GetMousePosY();					// マウス位置取得関数
	int deltaX = mouseX - centerX;
	int deltaY = mouseY - centerY;


	// カメラを回転させる（カメラオブジェクトに合わせてください）
	if (deltaX || deltaY)							// マウスが動いた場合のみ
	{
		g_Camera[0].rot.y += deltaX * sensitivity;	// Y軸回転
		g_Camera[0].rot.x += deltaY * sensitivity;	// X軸回転

		// ピッチ角を制限する
		g_Camera[0].rot.x = max(-pitchLimit * XM_PI / 180.0f, min(g_Camera[0].rot.x, pitchLimit * XM_PI / 180.0f));

	}

	// マウスをウィンドウの中心にリセット
	POINT ptCenter = { centerX, centerY };
	ClientToScreen(hWnd, &ptCenter);				// クライアント座標からスクリーン座標へ変換
	SetCursorPos(ptCenter.x, ptCenter.y);


	if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_LEFT_R))
	{// 注視点旋回「左」
		g_Camera[0].rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera[0].rot.y < -XM_PI)
		{
			g_Camera[0].rot.y += XM_PI * 2.0f;
		}

		g_Camera[0].at.x = g_Camera[0].pos.x + sinf(g_Camera[0].rot.y) * g_Camera[0].len;
		g_Camera[0].at.z = g_Camera[0].pos.z + cosf(g_Camera[0].rot.y) * g_Camera[0].len;
	}

	if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT_R))
	{// 注視点旋回「右」
		g_Camera[0].rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera[0].rot.y > XM_PI)
		{
			g_Camera[0].rot.y -= XM_PI * 2.0f;
		}

		g_Camera[0].at.x = g_Camera[0].pos.x + sinf(g_Camera[0].rot.y) * g_Camera[0].len;
		g_Camera[0].at.z = g_Camera[0].pos.z + cosf(g_Camera[0].rot.y) * g_Camera[0].len;
	}

	if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_UP_R))
	{// 注視点移動「上」
		g_Camera[0].rot.x -= VALUE_ROTATE_CAMERA;
		// ピッチ角を制限する
		g_Camera[0].rot.x = max(-pitchLimit * XM_PI / 180.0f, min(g_Camera[0].rot.x, pitchLimit * XM_PI / 180.0f));

	}

	if (GetKeyboardPress(DIK_DOWN) || IsButtonPressed(0, BUTTON_DOWN_R))
	{// 注視点移動「下」
		g_Camera[0].rot.x += VALUE_ROTATE_CAMERA;
		// ピッチ角を制限する
		g_Camera[0].rot.x = max(-pitchLimit * XM_PI / 180.0f, min(g_Camera[0].rot.x, pitchLimit * XM_PI / 180.0f));

	}
#ifdef _DEBUG

	if (GetKeyboardPress(DIK_U))
	{// 近づく
		g_Camera[0].len -= VALUE_MOVE_CAMERA;
		g_Camera[0].pos.x = g_Camera[0].at.x - sinf(g_Camera[0].rot.y) * g_Camera[0].len;
		g_Camera[0].pos.z = g_Camera[0].at.z - cosf(g_Camera[0].rot.y) * g_Camera[0].len;
	}

	if (GetKeyboardPress(DIK_M))
	{// 離れる
		g_Camera[0].len += VALUE_MOVE_CAMERA;
		g_Camera[0].pos.x = g_Camera[0].at.x - sinf(g_Camera[0].rot.y) * g_Camera[0].len;
		g_Camera[0].pos.z = g_Camera[0].at.z - cosf(g_Camera[0].rot.y) * g_Camera[0].len;
	}

	// カメラを初期に戻す
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}

#endif



#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(void)
{
	// ビューマトリックス設定
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera[0].pos), XMLoadFloat3(&g_Camera[0].at), XMLoadFloat3(&g_Camera[0].up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera[0].mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera[0].mtxInvView, mtxInvView);


	// プロジェクションマトリックス設定
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera[0].mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera[0].pos);
}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA* GetCamera(void)
{
	return &g_Camera[0];
}

//=============================================================================
// ビューポートの設定
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext* g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// ビューポート設定
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



// カメラの視点と注視点をセット
void SetCameraAT(XMFLOAT3 pos)
{
	// カメラの注視点を引数の座標にしてみる
	g_Camera[0].at = pos;

	// カメラの視点をカメラのY軸回転に対応させている
	g_Camera[0].pos.x = g_Camera[0].at.x - sinf(g_Camera[0].rot.y) * g_Camera[0].len;
	g_Camera[0].pos.z = g_Camera[0].at.z - cosf(g_Camera[0].rot.y) * g_Camera[0].len;

}



//カメラは複数化済み
void SetCameraPos(int index, XMFLOAT3 pos)
{
	//カメラ位置を受け取った位置(頭の位置)
	g_Camera[index].pos = pos;
	XMMATRIX mtxWorld, mtxRot, mtxTrf, mtxTranslate;
	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(0.0f, 0.0f, 1.0f);	//前ベクトル
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Camera[0].rot.x, g_Camera[0].rot.y, g_Camera[0].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	XMFLOAT3 result = XMFLOAT3(mtxWorld.r[3].m128_f32[0], mtxWorld.r[3].m128_f32[1], mtxWorld.r[3].m128_f32[2]);

	//ワールドに変換し代入
	g_Camera[0].at = XMFLOAT3(pos.x + result.x, pos.y + result.y, pos.z + result.z);
}