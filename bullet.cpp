//=============================================================================
//
// �e���ˏ��� [bullet.cpp]
// Author : GP11A132 21 �쎛�Č�
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
// �}�N����`
//*****************************************************************************
#define	MODEL_BULLET0		"data/MODEL/bullet.obj"	// �ǂݍ��ރ��f����

#define	BULLET_SPEED		(5.0f)					// �e�̈ړ��X�s�[�h


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
//HRESULT MakeVertexBullet(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BULLET		g_Bullet[MAX_BULLET];
static BULLET		g_BulletModel[MAX_BULLET_MODEL];


static char		g_ModelName[MAX_BULLET_MODEL][256] = {
	MODEL_BULLET0,
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
{
	//MakeVertexBullet();

	// �o���b�g�̏�����
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

	// ���f���̓Ǎ�
	for (int i = 0; i < MAX_BULLET_MODEL; i++)
	{
		LoadModel(g_ModelName[i], &g_BulletModel[i].model);
		g_BulletModel[i].load = TRUE;

		SetModelDiffuse(&g_BulletModel[0].model, i, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	}

	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateBullet(void)
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
            // �J�����̉�]�p�Ɋ�Â����e�̈ړ��x�N�g�����v�Z
            XMFLOAT3 direction;
            direction.x = sinf(g_Bullet[i].rot.y) * cosf(g_Bullet[i].rot.x);
            direction.y = -sinf(g_Bullet[i].rot.x);
            direction.z = cosf(g_Bullet[i].rot.y) * cosf(g_Bullet[i].rot.x);

            // �e�̈ړ�����
            g_Bullet[i].pos.x += direction.x * BULLET_SPEED;
            g_Bullet[i].pos.y += direction.y * BULLET_SPEED;
            g_Bullet[i].pos.z += direction.z * BULLET_SPEED;

			// �e�̐F�������_���ɐݒ�
			float time = GetTickCount64() / 1000.0f; // ���݂̎��Ԃ�b�P�ʂŎ擾

			float r = sinf(time * 1.0f) * 0.5f + 0.5f; // 0.0f �` 1.0f �̊Ԃŕω�
			float g = sinf(time * 1.4f) * 0.5f + 0.5f; // ����
			float b = sinf(time * 1.8f) * 0.5f + 0.5f; // ����
			SetModelDiffuse(&g_BulletModel[0].model, 0, XMFLOAT4(r, g, b, 1.0f));

			// �e�̈ʒu���X�V
			SetPositionShadow(g_Bullet[i].shadowIdx, XMFLOAT3(g_Bullet[i].pos.x, 0.1f, g_Bullet[i].pos.z));

			// �t�B�[���h�̊O�ɏo����e����������
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
// �`�揈��
//=============================================================================
void DrawBullet(void)
{
	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, g_Bullet[i].rot.y + XM_PI, g_Bullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Bullet[i].material);

			// ���f���`��
			DrawModel(&g_BulletModel[g_Bullet[i].modelIdx].model);
		}
	}

	// �J�����O����
	SetCullingMode(CULL_MODE_BACK);

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

}


//=============================================================================
// �e�̃p�����[�^���Z�b�g
//=============================================================================
int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot, int modelIdx)
{
	CAMERA* cam = GetCamera(); // �J���������擾
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			g_Bullet[nCntBullet].pos = pos;
			g_Bullet[nCntBullet].rot = cam->rot; // �J�����̉�]�����g�p
			g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_Bullet[nCntBullet].use = TRUE;
			g_Bullet[nCntBullet].modelIdx = modelIdx;

			// �e�̐ݒ�
			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

			PlaySound(SOUND_LABEL_SE_8bit_shoot1);

			break;
		}
	}

	return nIdxBullet;
}


//=============================================================================
// �e�̎擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

