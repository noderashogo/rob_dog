//=============================================================================
//
// HP処理 [hp.h]
// Author : 野寺翔悟
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_HP			(5)		// HPの最大値




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitHp(void);
void UninitHp(void);
void UpdateHp(void);
void DrawHp(void);

