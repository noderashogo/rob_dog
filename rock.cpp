//=============================================================================
//
// �⏈�� [rock.cpp]
// Author : GP11A132 21 �쎛�Č�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "rock.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(5)				// �e�N�X�`���̐�

#define	ROCK_WIDTH			(50.0f)			// ���_�T�C�Y
#define	ROCK_HEIGHT			(80.0f)			// ���_�T�C�Y

#define	MAX_ROCK			(256)			// �؍ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			nIdxShadow;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} ROCK;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexRock(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static ROCK					g_aRock[MAX_ROCK];	// �؃��[�N
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static int					g_TexNo;			// �e�N�X�`���ԍ�
static int					g_TexAnim;			// �e�N�X�`���A�j���p

static char *g_TextureName[] =
{
	"data/TEXTURE/tree001.png",
	"data/TEXTURE/tree002.png",
	"data/TEXTURE/tree003.png",
	"data/TEXTURE/tree004.png",
	"data/TEXTURE/tree005.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitRock(void)
{
	MakeVertexRock();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = g_TexAnim = 0;

	// �؃��[�N�̏�����
	for(int nCntRock = 0; nCntRock < MAX_ROCK; nCntRock++)
	{
		ZeroMemory(&g_aRock[nCntRock].material, sizeof(g_aRock[nCntRock].material));
		g_aRock[nCntRock].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aRock[nCntRock].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aRock[nCntRock].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aRock[nCntRock].fWidth = ROCK_WIDTH;
		g_aRock[nCntRock].fHeight = ROCK_HEIGHT;
		g_aRock[nCntRock].use = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	// �؂̐ݒ�
	SetRock(XMFLOAT3(0.0f, -10.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetRock(XMFLOAT3(200.0f, -10.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetRock(XMFLOAT3(-200.0f, -10.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetRock(XMFLOAT3(0.0f, -10.0f, 200.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetRock(XMFLOAT3(0.0f, -10.0f, -200.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitRock(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateRock(void)
{

	for(int nCntRock = 0; nCntRock < MAX_ROCK; nCntRock++)
	{
		if(g_aRock[nCntRock].use)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aRock[nCntRock].nIdxShadow, XMFLOAT3(g_aRock[nCntRock].pos.x, 0.1f, g_aRock[nCntRock].pos.z));
		}
	}


#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawRock(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_ROCK; i++)
	{
		if(g_aRock[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �֐��g������
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


			// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă��(����)
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_aRock[i].scl.x, g_aRock[i].scl.y, g_aRock[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aRock[i].pos.x, g_aRock[i].pos.y, g_aRock[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_aRock[i].material);

			// �e�N�X�`���ݒ�
			int texNo = i % TEXTURE_MAX;
			if (i == 4)	// �T�Ԃ̖؂����e�N�X�`���A�j�������Ă݂�
			{
				// �e�N�X�`���A�j���͂���Ȋ���
				g_TexAnim++;
				if ((g_TexAnim % 16) == 0)
				{
					g_TexNo = (g_TexNo + 1) % TEXTURE_MAX;
				}
				texNo = g_TexNo;
			}
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexRock(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �؂̃p�����[�^���Z�b�g
//=============================================================================
int SetRock(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxRock = -1;

	for(int nCntRock = 0; nCntRock < MAX_ROCK; nCntRock++)
	{
		if(!g_aRock[nCntRock].use)
		{
			g_aRock[nCntRock].pos = pos;
			g_aRock[nCntRock].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_aRock[nCntRock].fWidth = fWidth;
			g_aRock[nCntRock].fHeight = fHeight;
			g_aRock[nCntRock].use = TRUE;

			// �e�̐ݒ�
			g_aRock[nCntRock].nIdxShadow = CreateShadow(g_aRock[nCntRock].pos, 0.5f, 0.5f);

			nIdxRock = nCntRock;

			break;
		}
	}

	return nIdxRock;
}

