//=============================================================================
//
// モデル処理 [player.cpp]
// Author : GP11A132 21 野寺翔悟
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "bullet.h"
#include "debugproc.h"
#include "meshfield.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/dog/dog_body.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_HEAD	"data/MODEL/dog/dog_head.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LARM	"data/MODEL/dog/dog_lleg.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LLEG	"data/MODEL/dog/dog_lleg_2.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_RARM	"data/MODEL/dog/dog_rleg.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_RLEG	"data/MODEL/dog/dog_rleg_2.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_TAIL	"data/MODEL/dog/dog_tail.obj"	// 読み込むモデル名


enum {
	PATTERN_MOVE = 0,
	PATTERN_STOP,
	PATTERN_MAX,
};

#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(0.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(6)								// プレイヤーのパーツの数



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static float		roty = 0.0f;

static LIGHT		g_Light;

// プレイヤーの階層アニメーションデータ
// プレイヤーの左右パーツを動かしているアニメデータ
static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-15.0f, 0.0f, 9.0f),		XMFLOAT3(0.0f,  XM_PI / 3, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(15.0f, 0.0f, 9.0f),		XMFLOAT3(0.0f, -XM_PI / 3, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_larm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 16.0f),		XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 11.0f, -12.0f),	XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },


};

static INTERPOLATION_DATA move_tbl_rarm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 11.0f, -12.0f),	XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 16.0f),		XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_lleg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 10.0f, 12.0f),		XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f,   -16.0f),	XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_rleg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f,   -16.0f),	XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 10.0f, 12.0f),		XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_tail[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(10.0f, 0.0f, -10.0f),	XMFLOAT3(0.0f,  XM_PI / 2, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-10.0f, 0.0f, -10.0f),	XMFLOAT3(0.0f, -XM_PI / 2, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },


};

static INTERPOLATION_DATA move_tbl_stop[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f,  0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};




static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_head,
	move_tbl_larm,
	move_tbl_rarm,
	move_tbl_lleg,
	move_tbl_rleg,
	move_tbl_tail,
	move_tbl_stop,
};






//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player.load = TRUE;
	LoadModel(MODEL_PLAYER, &g_Player.model);

	g_Player.pos = XMFLOAT3(-10.0f, PLAYER_OFFSET_Y+70.0f, -50.0f);
	g_Player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.scl = XMFLOAT3(0.8f, 0.8f, 0.8f);

	g_Player.spd = 0.0f;			// 移動スピードクリア

	g_Player.use = TRUE;			// TRUE:生きてる
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	g_Player.hp = 5;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号

	// キーを押した時のプレイヤーの向き
	roty = 0.0f;

	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる


	// 階層アニメーションの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(0.8f, 0.8f, 0.8f);

		// 親子関係
		g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
	//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
	//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Parts[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Parts[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = FALSE;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent = &g_Player;	// 親をセット
	g_Parts[0].tblNo = 0;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[0].load = 1;
	LoadModel(MODEL_PLAYER_HEAD, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent = &g_Player;	// 親をセット
	g_Parts[1].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[1].tblMax = sizeof(move_tbl_stop) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[1].load = 1;
	LoadModel(MODEL_PLAYER_LARM, &g_Parts[1].model);

	g_Parts[2].use = TRUE;
	g_Parts[2].parent = &g_Player;	// 親をセット
	g_Parts[2].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[2].tblMax = sizeof(move_tbl_stop) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[2].load = 1;
	LoadModel(MODEL_PLAYER_RARM, &g_Parts[2].model);

	g_Parts[3].use = TRUE;
	g_Parts[3].parent = &g_Player;	// 親をセット
	g_Parts[3].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[3].tblMax = sizeof(move_tbl_stop) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[3].load = 1;
	LoadModel(MODEL_PLAYER_LLEG, &g_Parts[3].model);

	g_Parts[4].use = TRUE;
	g_Parts[4].parent = &g_Player;	// 親をセット
	g_Parts[4].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[4].tblMax = sizeof(move_tbl_stop) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[4].load = 1;
	LoadModel(MODEL_PLAYER_RLEG, &g_Parts[4].model);

	g_Parts[5].use = TRUE;
	g_Parts[5].parent = &g_Player;	// 親をセット
	g_Parts[5].tblNo = 5;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[5].tblMax = sizeof(move_tbl_tail) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[5].load = 1;
	LoadModel(MODEL_PLAYER_TAIL, &g_Parts[5].model);



	// クォータニオンの初期化
	XMStoreFloat4(&g_Player.Quaternion, XMQuaternionIdentity());



	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load == TRUE)
	{
		UnloadModel(&g_Player.model);
		ReleaseShadow(g_Player.shadowIdx);
		g_Player.load = FALSE;
	}

	// パーツの解放処理
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}



}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	g_Player.spd *= 0.7f;

	// プレイヤーの回転をカメラのY軸回転に設定
	// プレイヤーがカメラの反対方向を向くように回転させる
	g_Player.rot.y = cam->rot.y + XM_PI;

	// 移動処理
	if (GetKeyboardPress(DIK_W) || IsButtonPressed(0,BUTTON_UP))  // 前進
	{
		g_Player.pos.x -= sinf(g_Player.rot.y) * VALUE_MOVE;
		g_Player.pos.z -= cosf(g_Player.rot.y) * VALUE_MOVE;

		if (g_Player.pattern != PATTERN_MOVE)
		{
			SetPattern(PATTERN_MOVE);
		}

	}
	else if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, BUTTON_DOWN))  // 後退
	{
		g_Player.pos.x += sinf(g_Player.rot.y) * VALUE_MOVE;
		g_Player.pos.z += cosf(g_Player.rot.y) * VALUE_MOVE;
		if (g_Player.pattern != PATTERN_MOVE)
		{
			SetPattern(PATTERN_MOVE);
		}

	}
	else if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_LEFT))  // 左移動
	{
		g_Player.pos.x += sinf(g_Player.rot.y + XM_PI / 2) * VALUE_MOVE;
		g_Player.pos.z += cosf(g_Player.rot.y + XM_PI / 2) * VALUE_MOVE;

		if (g_Player.pattern != PATTERN_MOVE)
		{
			SetPattern(PATTERN_MOVE);
		}

	}
	else if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, BUTTON_RIGHT))  // 右移動
	{
		g_Player.pos.x += sinf(g_Player.rot.y - XM_PI / 2) * VALUE_MOVE;
		g_Player.pos.z += cosf(g_Player.rot.y - XM_PI / 2) * VALUE_MOVE;

		if (g_Player.pattern != PATTERN_MOVE)
		{
			SetPattern(PATTERN_MOVE);
		}

	}

	else
	{
			SetPattern(PATTERN_STOP);
	}



#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.spd = 0.0f;
		roty = 0.0f;
	}
#endif


	// レイキャストして足元の高さを求める
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	BOOL ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
	if (ans)
	{
		g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		g_Player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}


	// 弾発射処理
	if (IsMouseLeftTriggered() || GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_R2))
	{
		g_Player.pos.y += 5.0f;
		SetBullet(g_Player.pos, g_Player.rot, 0);
	}


	// プレイヤーの位置がマップの境界を超えていないかチェック
	if (g_Player.pos.x < -MAP_W / 2 + PLAYER_SIZE) 
	{
		g_Player.pos.x = -MAP_W / 2 + PLAYER_SIZE;  // 左の境界を超えた場合
	}
	if (g_Player.pos.x > MAP_W / 2 - PLAYER_SIZE) 
	{
		g_Player.pos.x = MAP_W / 2 - PLAYER_SIZE;   // 右の境界を超えた場合
	}
	if (g_Player.pos.z < -MAP_H / 2 + PLAYER_SIZE) 
	{
		g_Player.pos.z = -MAP_H / 2 + PLAYER_SIZE;  // 下の境界を超えた場合
	}
	if (g_Player.pos.z > MAP_H / 2 - PLAYER_SIZE) 
	{
		g_Player.pos.z = MAP_H / 2 - PLAYER_SIZE;   // 上の境界を超えた場合
	}

	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);


	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if ((g_Parts[i].use == TRUE) && (g_Parts[i].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[i].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[i].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[i].tblNo];	// 行動テーブルのアドレスを取得

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[i].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Parts[i].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_Parts[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
			}

		}

	}


	// ポイントライトのテスト
	{
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}



	//////////////////////////////////////////////////////////////////////
	// 姿勢制御
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;


	g_Player.UpVector = Normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.UpVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	//nvx = vx / len;
	angle = asinf(len);

	//quat = XMQuaternionIdentity();

//	quat = XMQuaternionRotationAxis(nvx, angle);
	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.Quaternion), quat, 0.05f);
	XMStoreFloat4(&g_Player.Quaternion, quat);




#ifdef _DEBUG
	 //デバッグ表示
	PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, Normal.y);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// クォータニオンを反映
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_Player.model);



	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
																			// ↑
																			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する
		if (g_Parts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// モデル描画
		DrawModel(&g_Parts[i].model);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

//=============================================================================
// プレイヤー行動パターン
//=============================================================================
void SetPattern(int pattern)
{
	g_Player.pattern = pattern;

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].time = 0.0f;

	}

	switch (g_Player.pattern)
	{
	case PATTERN_MOVE:

		g_Parts[0].tblNo = 0;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		g_Parts[1].tblNo = 1;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[1].tblMax = sizeof(move_tbl_larm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		g_Parts[2].tblNo = 2;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[2].tblMax = sizeof(move_tbl_rarm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		g_Parts[3].tblNo = 3;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[3].tblMax = sizeof(move_tbl_lleg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		g_Parts[4].tblNo = 4;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[4].tblMax = sizeof(move_tbl_rleg) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		g_Parts[5].tblNo = 5;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[5].tblMax = sizeof(move_tbl_tail) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		break;

	case PATTERN_STOP:

		g_Parts[0].tblNo = 0;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット


		g_Parts[1].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[1].tblMax = sizeof(move_tbl_stop) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		g_Parts[2].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[2].tblMax = sizeof(move_tbl_stop) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		g_Parts[3].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[3].tblMax = sizeof(move_tbl_stop) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		g_Parts[4].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[4].tblMax = sizeof(move_tbl_stop) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		g_Parts[5].tblNo = 5;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[5].tblMax = sizeof(move_tbl_tail) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		break;
	}

}

