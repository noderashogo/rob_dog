//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : 
//
//=============================================================================
#pragma once

#include "player.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ENEMY			(50)					// エネミーの数
#define MAX_ENEMY_MODEL		(1)						// エネミーのモデル数

#define	ENEMY_SIZE		(5.0f)						// 当たり判定の大きさ

#define SPAWN_RANGE		(100.0f)					// プレイヤーの周囲ユニットの範囲にエネミーが生成される

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	MATERIAL			material;			// マテリアル
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				shootTimer;			// 次の発射までのタイマー
	float				shootInterval;		// 発射間隔


	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号
	int					hp;					// エネミーのHP
	int					modelIdx;			// モデルインデックス

	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);


XMFLOAT3 GenerateRandomPositionInField(float excludeRange);
void SpawnNewEnemy(PLAYER* player, float excludeRange);
void ActivateEnemy(int index, float excludeRange);
void SpawnAdditionalEnemies();


ENEMY *GetEnemy(void);
