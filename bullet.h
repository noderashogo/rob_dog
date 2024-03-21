//=============================================================================
//
// 弾発射処理 [bullet.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_BULLET			(256)	// 弾最大数
#define	MAX_BULLET_MODEL	(1)		// model最大数
#define	BULLET_SIZE			(5.0f)	// 当たり判定の大きさ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	MATERIAL			material;						// マテリアル
	XMFLOAT4X4			mtxWorld;						// ワールドマトリックス
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	DX11_MODEL			model;

	XMFLOAT3			pos;							// 位置
	XMFLOAT3			rot;							// 角度
	XMFLOAT3			scl;							// スケール

	float				timer;							// 生存時間
	float				spd;							// 移動量
	float				size;							// 当たり判定の大きさ
	int					shadowIdx;						// 影ID
	int					modelIdx;						// モデルインデックス
	BOOL				use;							// 使用しているかどうか
	BOOL				load;


} BULLET;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot, int modelIdx );

BULLET *GetBullet(void);

