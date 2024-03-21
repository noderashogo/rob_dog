//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : GP11A132 21 野寺翔悟
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "shadow.h"
#include "particle.h"
#include "player.h"
#include "bullet.h"
#include "score.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/goldenryuu_fixed/goldenryuu.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(1.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(13.5f)						// エネミーの足元をあわせる

#define ENEMY_HP			(2)							// エネミーの足元をあわせる
#define	MIN_DISTANCE		(10.0f)						// エネミー同士の距離感
#define AVOID_SPEED			(1.1f)						// 衝突時の回避スピード
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー
static ENEMY			g_EnemyModel[MAX_ENEMY_MODEL];

int g_Enemy_load = 0;
static int g_CurrentMaxEnemies;					 // 現在のエネミーの最大出現数
static int g_EnemyDefeatedCount;				// エネミー撃破カウンタ

static char		g_ModelName[MAX_ENEMY_MODEL][256] = {
	MODEL_ENEMY,
};


static INTERPOLATION_DATA g_MoveTbl0[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(MAP_W,   ENEMY_OFFSET_Y + 450.0f, MAP_W),	XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(50.0f, 50.0f, 50.0f), 60 * 2 },
	{ XMFLOAT3(-MAP_W,  ENEMY_OFFSET_Y + 450.0f, MAP_W),	XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(50.0f, 50.0f, 50.0f), 1 },
	{ XMFLOAT3(-MAP_W,  ENEMY_OFFSET_Y + 450.0f, MAP_W),	XMFLOAT3(0.0f, -XM_PI / 2, 0.0f),	XMFLOAT3(50.0f, 50.0f, 50.0f), 60 * 2 },
	{ XMFLOAT3(-MAP_W,  ENEMY_OFFSET_Y + 450.0f, -MAP_W),	XMFLOAT3(0.0f, -XM_PI / 2, 0.0f),	XMFLOAT3(50.0f, 50.0f, 50.0f), 1 },
	{ XMFLOAT3(-MAP_W,  ENEMY_OFFSET_Y + 450.0f, -MAP_W),	XMFLOAT3(0.0f, XM_PI, 0.0f),		XMFLOAT3(50.0f, 50.0f, 50.0f), 60 * 2 },
	{ XMFLOAT3(MAP_W,   ENEMY_OFFSET_Y + 450.0f, -MAP_W),	XMFLOAT3(0.0f, XM_PI, 0.0f),		XMFLOAT3(50.0f, 50.0f, 50.0f), 1 },
	{ XMFLOAT3(MAP_W,   ENEMY_OFFSET_Y + 450.0f, -MAP_W),	XMFLOAT3(0.0f, XM_PI / 2, 0.0f),	XMFLOAT3(50.0f, 50.0f, 50.0f), 60 * 2 },
	{ XMFLOAT3(MAP_W,   ENEMY_OFFSET_Y + 450.0f, MAP_W),	XMFLOAT3(0.0f, XM_PI / 2, 0.0f),	XMFLOAT3(50.0f, 50.0f, 50.0f), 1 },
	{ XMFLOAT3(MAP_W,   ENEMY_OFFSET_Y + 450.0f, MAP_W),	XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(50.0f, 50.0f, 50.0f), 1 },

};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,

};

PLAYER* player = GetPlayer();


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	g_CurrentMaxEnemies = 5;
	g_EnemyDefeatedCount = 0;


	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		//g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd  = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ
		g_Enemy[i].hp = ENEMY_HP;		// エネミーHP

		g_Enemy[i].modelIdx = 0;		// 共通モデルのインデックス

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		
		g_Enemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット


		g_Enemy[i].use = FALSE;			// TRUE:生きてる

	}

	// モデルの読込
	for (int i = 0; i < MAX_ENEMY_MODEL; i++)
	{
		LoadModel(g_ModelName[i], &g_EnemyModel[i].model);
		g_EnemyModel[i].load = TRUE;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_EnemyModel[i].model, &g_EnemyModel[i].diffuse[0]);

	}


	// 必要な数のエネミーをアクティブ化
	for (int i = 0; i < g_CurrentMaxEnemies; i++) 
	{
		ActivateEnemy(i,SPAWN_RANGE);
	}


	// 0番だけ線形補間で動かしてみる
	g_Enemy[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
		if (g_Enemy[i].use)
		{
			// 影を削除する
			if (g_Enemy[i].shadowIdx >= 0)
			{
				ReleaseShadow(g_Enemy[i].shadowIdx);
				g_Enemy[i].shadowIdx = -1; // 影のインデックスをリセット
			}
			g_Enemy[i].use = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	// エネミーを動かく場合は、影も合わせて動かす事を忘れない
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes

				// 他のエネミーとの距離を確認
			for (int j = 0; j < MAX_ENEMY; j++)
			{
				if (i != j && g_Enemy[j].use == TRUE)
				{
					XMFLOAT3 diff;
					diff.x = g_Enemy[j].pos.x - g_Enemy[i].pos.x;
					diff.y = g_Enemy[j].pos.y - g_Enemy[i].pos.y;
					diff.z = g_Enemy[j].pos.z - g_Enemy[i].pos.z;

					float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
					float minDistanceSq = MIN_DISTANCE * MIN_DISTANCE; // MIN_DISTANCEはエネミー間の最小距離

					// 衝突回避
					if (distanceSq < minDistanceSq)
					{
						// diffを逆方向に設定
						XMVECTOR avoidDir = -XMLoadFloat3(&diff);
						avoidDir = XMVector3Normalize(avoidDir);
						XMStoreFloat3(&diff, avoidDir);

						g_Enemy[i].pos.x += diff.x * AVOID_SPEED;
						g_Enemy[i].pos.y += diff.y * AVOID_SPEED;
						g_Enemy[i].pos.z += diff.z * AVOID_SPEED;
					}
				}
			}



			// エネミーからプレイヤーへの方向ベクトルを計算
			XMFLOAT3 direction;
			direction.x = player->pos.x - g_Enemy[i].pos.x;
			direction.y = 0;								 // Y軸は変更しない（水平移動のみ）
			direction.z = player->pos.z - g_Enemy[i].pos.z;

			// 方向ベクトルを正規化
			XMVECTOR dirVector = XMLoadFloat3(&direction);
			dirVector = XMVector3Normalize(dirVector);
			XMStoreFloat3(&direction, dirVector);

			// エネミーの向きをプレイヤーに向ける
			g_Enemy[i].rot.y = atan2f(direction.x, direction.z) + XM_PI;

			// エネミーを前進させる
			float moveSpeed = VALUE_MOVE;					 // 移動速度
			g_Enemy[i].pos.x += direction.x * moveSpeed;
			g_Enemy[i].pos.z += direction.z * moveSpeed;

			// エネミーが倒された
			if (g_Enemy[i].hp <= 0)
			{
				// パーティクルのパラメータ
				const int particleCount = 150;				// 生成するパーティクルの数
				const float particleSize = 10.0f;			// パーティクルのサイズ
				const int particleLife = 60;				// パーティクルの寿命（フレーム数）

				// パーティクルの生成
				for (int j = 0; j < particleCount; j++)
				{
					// パーティクルの位置（エネミーの現在位置）
					XMFLOAT3 pos = g_Enemy[i].pos;

					// パーティクルの移動方向と速度（ランダム）
					XMFLOAT3 move = XMFLOAT3(
						((float)rand() / RAND_MAX - 0.5f) * 2.0f, // X方向
						((float)rand() / RAND_MAX - 0.5f) * 2.0f, // Y方向
						((float)rand() / RAND_MAX - 0.5f) * 2.0f  // Z方向
					);

					// パーティクルの色（赤色、半透明）
					XMFLOAT4 color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f);

					// パーティクルの生成
					SetParticle(pos, move, color, particleSize, particleSize, particleLife);

				}


				g_Enemy[i].use = FALSE;
				ReleaseShadow(g_Enemy[i].shadowIdx);
				// プレイヤーの近くに新しいエネミーを3体生成
				SpawnNewEnemy(player, SPAWN_RANGE);

				g_EnemyDefeatedCount++;
				if (g_EnemyDefeatedCount % 2 == 0 && g_CurrentMaxEnemies < MAX_ENEMY) // 2体倒すごとに最大数を増やす
				{
					g_CurrentMaxEnemies++;
					SpawnAdditionalEnemies(); // 新しいエネミーを生成
				}



			}


			// 移動処理
			if (g_Enemy[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);

				// frameを使て時間経過処理をする
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}

			}


			// 影もエネミーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	}




#ifdef _DEBUG

	if (GetKeyboardTrigger(DIK_P))
	{
		// モデルの色を変更できるよ！半透明にもできるよ。
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
		}
	}

	if (GetKeyboardTrigger(DIK_L))
	{
		// モデルの色を元に戻している
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
		}
	}
#endif



}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// 縁取りの設定
	SetFuchi(1);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

			// マテリアル設定
			SetMaterial(g_Enemy[i].material);

			// モデル描画
			DrawModel(&g_EnemyModel[g_Enemy[i].modelIdx].model);

		}

	}

	SetFuchi(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤーの周囲にランダムな位置を生成する関数
//=============================================================================

XMFLOAT3 GenerateRandomPositionInField(float excludeRange) {
	float randX, randZ;

	do {
		randX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * MAP_W/2.5;
		randZ = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * MAP_H/2.5;
	} while (sqrt(randX * randX + randZ * randZ) < excludeRange); // プレイヤーの近くの範囲を除外

	return XMFLOAT3(randX, ENEMY_OFFSET_Y, randZ);
}
//=============================================================================
// 新しいエネミーを生成する関数
//=============================================================================

void SpawnNewEnemy(PLAYER* player, float excludeRange) {
	int currentEnemyCount = 0; // 現在のエネミー数を数える

	// 現在のエネミーの数をカウント
	for (int i = 0; i < MAX_ENEMY; ++i) 
	{
		if (g_Enemy[i].use) {
			currentEnemyCount++;
		}
	}

	// 現在のエネミーの数が最大数より少ない場合、新しいエネミーを生成
	if (currentEnemyCount < g_CurrentMaxEnemies) 
	{
		for (int i = 0; i < MAX_ENEMY; ++i) {
			if (!g_Enemy[i].use) {
				ActivateEnemy(i, excludeRange);
				break;
			}
		}
	}
}


//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY* GetEnemy()
{
	return &g_Enemy[0];
}


//=============================================================================
// エネミーのアクティブ化
//=============================================================================
void ActivateEnemy(int index, float excludeRange) 
{
	if (index >= 0 && index < MAX_ENEMY) {
		ENEMY* enemy = &g_Enemy[index];

		// エネミーの位置、回転、スケール、HPなどの属性を設定
		enemy->pos = GenerateRandomPositionInField(excludeRange);
		enemy->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		enemy->scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		enemy->hp = ENEMY_HP;

		// エネミーをアクティブに設定
		enemy->use = TRUE;

		// その他必要な初期化処理
		XMFLOAT3 shadowPos = enemy->pos;
		shadowPos.y -= ENEMY_OFFSET_Y; // 影の位置調整

		enemy->shadowIdx = CreateShadow(shadowPos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

	}
}


//=============================================================================
// エネミー上限変更
//=============================================================================
void SpawnAdditionalEnemies() 
{
	int currentEnemyCount = 0;
	for (int i = 0; i < MAX_ENEMY; ++i) {
		if (g_Enemy[i].use) {
			currentEnemyCount++;
		}
	}

	while (currentEnemyCount < g_CurrentMaxEnemies) {
		for (int i = 0; i < MAX_ENEMY; ++i) {
			if (!g_Enemy[i].use) {
				ActivateEnemy(i, SPAWN_RANGE);
				currentEnemyCount++;
				break;
			}
		}
	}
}
