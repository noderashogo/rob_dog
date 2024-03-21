//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(6)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(480)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/121.png",
	"data/TEXTURE/yajirusi.png",
	"data/TEXTURE/start.png",
	"data/TEXTURE/tutorial.png",
	"data/TEXTURE/exit.png",
	"data/TEXTURE/tutorial_1.png",

};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

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
// ����������
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
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
	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_MusMus146);


	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateTitle(void)
{

	//if (GetKeyboardTrigger(DIK_RETURN))
	//{// Enter��������A�X�e�[�W��؂�ւ���
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	//// �Q�[���p�b�h�œ��͏���
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



	// �e�X�g�ŃG�t�F�N�g�̔����ꏊ���ړ�������
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


	#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

	#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	switch (g_CurrentMenuState)
	{
	case IDEL:
	{
		// �^�C�g���̔w�i��`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);


			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}


		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, 300.0f, g_CurrentMenuNode->posY, 70.0f, 70.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMConvertToRadians(0.0f));

			// �|���S���`��
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

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, 550.0f, 300.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.0f, 1.0f, 0.5f, alpha));
			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, 550.0f, 380.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));


				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, 550.0f, 460.0f, 350, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));

				// �|���S���`��
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

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, 550.0f, 300.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));
			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, 550.0f, 380.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, alpha));


				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, 550.0f, 460.0f, 350, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));

				// �|���S���`��
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

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, 550.0f, 300.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));
			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, 550.0f, 380.0f, 381, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f));


				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, 550.0f, 460.0f, 350, 71, 0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(0.0f, 1.0f, 0.5f, alpha));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}


		}


		break;
	}
	case TUTORIAL:
	{
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
		break;

	}



	// �����Z�̃e�X�g
	//SetBlendState(BLEND_MODE_ADD);		// ���Z����
	//SetBlendState(BLEND_MODE_SUBTRACT);	// ���Z����

	// //�e�N�X�`���ݒ�
	//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	//for (int i = 0; i < 30; i++)
	//{
	//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//	float dx = effect_dx;
	//	float dy = effect_dy;
	//	float sx = (float)(rand() % 100);
	//	float sy = (float)(rand() % 100);

	//	SetSpriteColor(g_VertexBuffer, dx + sx, dy + sy, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f,
	//		XMFLOAT4(1.0f, 0.3f, 1.0f, 0.5f));

	//	// �|���S���`��
	//	GetDeviceContext()->Draw(4, 0);
	//}
	//SetBlendState(BLEND_MODE_ALPHABLEND);	// ���������������ɖ߂�


	}
}





