//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : 
//
//=============================================================================
#pragma once

#include "player.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_ENEMY			(50)					// �G�l�~�[�̐�
#define MAX_ENEMY_MODEL		(1)						// �G�l�~�[�̃��f����

#define	ENEMY_SIZE		(5.0f)						// �����蔻��̑傫��

#define SPAWN_RANGE		(100.0f)					// �v���C���[�̎��̓��j�b�g�͈̔͂ɃG�l�~�[�����������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	MATERIAL			material;			// �}�e���A��
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				shootTimer;			// ���̔��˂܂ł̃^�C�}�[
	float				shootInterval;		// ���ˊԊu


	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	int					hp;					// �G�l�~�[��HP
	int					modelIdx;			// ���f���C���f�b�N�X

	float				time;				// ���`��ԗp
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��

};

//*****************************************************************************
// �v���g�^�C�v�錾
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
