//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(6)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(480)			// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/121.png",
	"data/TEXTURE/yajirusi.png",
	"data/TEXTURE/start.png",
	"data/TEXTURE/tutorial.png",
	"data/TEXTURE/exit.png",
	"data/TEXTURE/tutorial_1.png",

};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

struct MenuNode
{
	float posY;
	MenuNode* Prev;
	MenuNode* Next;

	void (*OnEnter)();
};

#define MAX_MENU_NODE 3
static MenuNode g_MenuNodes[MAX_MENU_NODE];

static MenuNode* g_CurrentMenuNode = nullptr;

enum MenuState
{
	IDEL, TUTORIAL,
};
static MenuState g_CurrentMenuState;

void StartGame()
{
	/*g_CurrentMenuState = MenuState::IDEL;
	
	
	/*SetFade(FADE_IN, MODE_TUTORIAL);*/
	g_CurrentMenuState = MenuState::IDEL;
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}
}

void StartTutorial()
{
	SetFade(FADE_IN, MODE_TUTORIAL);
	g_CurrentMenuState = MenuState::TUTORIAL;
	if (GetKeyboardTrigger(DIK_RIGHTARROW))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}
}

void Back()
{
	g_CurrentMenuState = MenuState::IDEL;
}

void Exit()
{
	exit(0);
}


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;

	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 200.0f;
	effect_dy = 100.0f;

	g_Load = TRUE;

	{
		g_CurrentMenuNode = &g_MenuNodes[0];

		g_MenuNodes[0] = { 300.0f,&g_MenuNodes[2],&g_MenuNodes[1],StartGame };
		g_MenuNodes[1] = { 380.0f,&g_MenuNodes[0],&g_MenuNodes[2],StartTutorial };
		g_MenuNodes[2] = { 460.0f,&g_MenuNodes[1],&g_MenuNodes[0],Exit };

		g_CurrentMenuState = MenuState::IDEL;
	}
	// BGM再生
	PlaySound(SOUND_LABEL_BGM_MusMus146);


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{

	//if (GetKeyboardTrigger(DIK_RETURN))
	//{// Enter押したら、ステージを切り替える
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	//// ゲームパッドで入力処理
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	//else if (IsButtonTriggered(0, BUTTON_B))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}



	if (GetKeyboardTrigger(DIK_BACK))
	{
		Back();
	}

	if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_CurrentMenuNode = g_CurrentMenuNode->Next;
	}
	else if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_UP))
	{
		g_CurrentMenuNode = g_CurrentMenuNode->Prev;
	}

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		g_CurrentMenuNode->OnEnter();
	}



	// テストでエフェクトの発生場所を移動させる
	//float speed = 4.0f;

	//if (GetKeyboardPress(DIK_DOWN))
	//{
	//	effect_dy += speed;
	//}
	//else if (GetKeyboardPress(DIK_UP))
	//{
	//	effect_dy -= speed;
	//}

	//if (GetKeyboardPress(DIK_RIGHT))
	//{
	//	effect_dx += speed;
	//}
	//else if (GetKeyboardPress(DIK_LEFT))
	//{
	//	effect_dx -= speed;
	//}


	#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

	#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	switch (g_CurrentMenuState)
	{
	case IDEL:
	{
		// タイトルの背景を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);


			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}


		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, 300.0f, g_CurrentMenuNode->posY, 70.0f, 70.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMConvertToRadians(0.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		if (g_CurrentMenuNode->posY == 300)
		{
			if (flag_alpha == TRUE)
			{
				alpha -= 0.02f;
				if (alpha <= 0.0f)
				{
					alpha = 0.0f;
					flag_alpha = FALSE;
				}
			}
			else
			{
				alpha += 0.02f;
				if (alpha >= 1.0f)
				{
					alpha = 1.0f;
					flag_alpha = TRUE;
				}
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, 550.0f, 300.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.0f, 1.0f, 0.5f, alpha));
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, 550.0f, 380.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));


				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, 550.0f, 460.0f, 350, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}



		}
		if (g_CurrentMenuNode->posY == 380)
		{
			if (flag_alpha == TRUE)
			{
				alpha -= 0.02f;
				if (alpha <= 0.0f)
				{
					alpha = 0.0f;
					flag_alpha = FALSE;
				}
			}
			else
			{
				alpha += 0.02f;
				if (alpha >= 1.0f)
				{
					alpha = 1.0f;
					flag_alpha = TRUE;
				}
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, 550.0f, 300.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, 550.0f, 380.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, alpha));


				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, 550.0f, 460.0f, 350, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}


		}

		if (g_CurrentMenuNode->posY == 460)
		{
			if (flag_alpha == TRUE)
			{
				alpha -= 0.02f;
				if (alpha <= 0.0f)
				{
					alpha = 0.0f;
					flag_alpha = FALSE;
				}
			}
			else
			{
				alpha += 0.02f;
				if (alpha >= 1.0f)
				{
					alpha = 1.0f;
					flag_alpha = TRUE;
				}
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, 550.0f, 300.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, 550.0f, 380.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));


				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, 550.0f, 460.0f, 350, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, alpha));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}


		}


		break;
	}
	case TUTORIAL:
	{
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}
		break;

	}



	// 加減算のテスト
	//SetBlendState(BLEND_MODE_ADD);		// 加算合成
	//SetBlendState(BLEND_MODE_SUBTRACT);	// 減算合成

	// //テクスチャ設定
	//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	//for (int i = 0; i < 30; i++)
	//{
	//	// １枚のポリゴンの頂点とテクスチャ座標を設定
	//	float dx = effect_dx;
	//	float dy = effect_dy;
	//	float sx = (float)(rand() % 100);
	//	float sy = (float)(rand() % 100);

	//	SetSpriteColor(g_VertexBuffer, dx + sx, dy + sy, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f,
	//		XMFLOAT4(1.0f, 0.3f, 1.0f, 0.5f));

	//	// ポリゴン描画
	//	GetDeviceContext()->Draw(4, 0);
	//}
	//SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す


	}
}





