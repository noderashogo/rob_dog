//=============================================================================
//
// 弾発射処理 [bullet.cpp]
// Author : GP11A132 21 野寺翔悟
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "bullet.h"
#include "camera.h"
#include "model.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BULLET0		"data/MODEL/bullet.obj"	// 読み込むモデル名

#define	BULLET_SPEED		(5.0f)					// 弾の移動スピード


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//HRESULT MakeVertexBullet(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BULLET		g_Bullet[MAX_BULLET];
static BULLET		g_BulletModel[MAX_BULLET_MODEL];


static char		g_ModelName[MAX_BULLET_MODEL][256] = {
	MODEL_BULLET0,
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	//MakeVertexBullet();

	// バレットの初期化
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
		g_Bullet[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };

		g_Bullet[nCntBullet].modelIdx = 0;

		g_Bullet[nCntBullet].spd = BULLET_SPEED;
		g_Bullet[nCntBullet].size = BULLET_SIZE;

		g_Bullet[nCntBullet].use = FALSE;

	}

	// モデルの読込
	for (int i = 0; i < MAX_BULLET_MODEL; i++)
	{
		LoadModel(g_ModelName[i], &g_BulletModel[i].model);
		g_BulletModel[i].load = TRUE;

		SetModelDiffuse(&g_BulletModel[0].model, i, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	for (int i = 0; i < MAX_BULLET_MODEL; i++)
	{
		UnloadModel(&g_Bullet[i].model);
		ReleaseShadow(g_Bullet[i].shadowIdx);
		g_BulletModel[i].load = FALSE;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
            // カメラの回転角に基づいた弾の移動ベクトルを計算
            XMFLOAT3 direction;
            direction.x = sinf(g_Bullet[i].rot.y) * cosf(g_Bullet[i].rot.x);
            direction.y = -sinf(g_Bullet[i].rot.x);
            direction.z = cosf(g_Bullet[i].rot.y) * cosf(g_Bullet[i].rot.x);

            // 弾の移動処理
            g_Bullet[i].pos.x += direction.x * BULLET_SPEED;
            g_Bullet[i].pos.y += direction.y * BULLET_SPEED;
            g_Bullet[i].pos.z += direction.z * BULLET_SPEED;

			// 弾の色をランダムに設定
			float time = GetTickCount64() / 1000.0f; // 現在の時間を秒単位で取得

			float r = sinf(time * 1.0f) * 0.5f + 0.5f; // 0.0f ～ 1.0f の間で変化
			float g = sinf(time * 1.4f) * 0.5f + 0.5f; // 同上
			float b = sinf(time * 1.8f) * 0.5f + 0.5f; // 同上
			SetModelDiffuse(&g_BulletModel[0].model, 0, XMFLOAT4(r, g, b, 1.0f));

			// 影の位置を更新
			SetPositionShadow(g_Bullet[i].shadowIdx, XMFLOAT3(g_Bullet[i].pos.x, 0.1f, g_Bullet[i].pos.z));

			// フィールドの外に出たら弾を消す処理
			if (g_Bullet[i].pos.x < MAP_LEFT || g_Bullet[i].pos.x > MAP_RIGHT ||
				g_Bullet[i].pos.y < MAP_DOWN || g_Bullet[i].pos.y > MAP_TOP ||
				g_Bullet[i].pos.z < MAP_DOWN || g_Bullet[i].pos.z > MAP_TOP)
			{
				g_Bullet[i].use = FALSE;
				ReleaseShadow(g_Bullet[i].shadowIdx);
			}
		}
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
{
	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, g_Bullet[i].rot.y + XM_PI, g_Bullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);


			// マテリアル設定
			SetMaterial(g_Bullet[i].material);

			// モデル描画
			DrawModel(&g_BulletModel[g_Bullet[i].modelIdx].model);
		}
	}

	// カリング無効
	SetCullingMode(CULL_MODE_BACK);

	// ライティングを有効に
	SetLightEnable(TRUE);

}


//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot, int modelIdx)
{
	CAMERA* cam = GetCamera(); // カメラ情報を取得
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			g_Bullet[nCntBullet].pos = pos;
			g_Bullet[nCntBullet].rot = cam->rot; // カメラの回転情報を使用
			g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_Bullet[nCntBullet].use = TRUE;
			g_Bullet[nCntBullet].modelIdx = modelIdx;

			// 影の設定
			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

			PlaySound(SOUND_LABEL_SE_8bit_shoot1);

			break;
		}
	}

	return nIdxBullet;
}


//=============================================================================
// 弾の取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

