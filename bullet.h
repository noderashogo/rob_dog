//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BULLET			(256)	// �e�ő吔
#define	MAX_BULLET_MODEL	(1)		// model�ő吔
#define	BULLET_SIZE			(5.0f)	// �����蔻��̑傫��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	MATERIAL			material;						// �}�e���A��
	XMFLOAT4X4			mtxWorld;						// ���[���h�}�g���b�N�X
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	DX11_MODEL			model;

	XMFLOAT3			pos;							// �ʒu
	XMFLOAT3			rot;							// �p�x
	XMFLOAT3			scl;							// �X�P�[��

	float				timer;							// ��������
	float				spd;							// �ړ���
	float				size;							// �����蔻��̑傫��
	int					shadowIdx;						// �eID
	int					modelIdx;						// ���f���C���f�b�N�X
	BOOL				use;							// �g�p���Ă��邩�ǂ���
	BOOL				load;


} BULLET;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot, int modelIdx );

BULLET *GetBullet(void);

