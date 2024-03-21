//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : GP11A132 21 �쎛�Č�
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
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/goldenryuu_fixed/goldenryuu.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(1.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(13.5f)						// �G�l�~�[�̑��������킹��

#define ENEMY_HP			(2)							// �G�l�~�[�̑��������킹��
#define	MIN_DISTANCE		(10.0f)						// �G�l�~�[���m�̋�����
#define AVOID_SPEED			(1.1f)						// �Փˎ��̉���X�s�[�h
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[
static ENEMY			g_EnemyModel[MAX_ENEMY_MODEL];

int g_Enemy_load = 0;
static int g_CurrentMaxEnemies;					 // ���݂̃G�l�~�[�̍ő�o����
static int g_EnemyDefeatedCount;				// �G�l�~�[���j�J�E���^

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
// ����������
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

		g_Enemy[i].spd  = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��
		g_Enemy[i].hp = ENEMY_HP;		// �G�l�~�[HP

		g_Enemy[i].modelIdx = 0;		// ���ʃ��f���̃C���f�b�N�X

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		
		g_Enemy[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Enemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g


		g_Enemy[i].use = FALSE;			// TRUE:�����Ă�

	}

	// ���f���̓Ǎ�
	for (int i = 0; i < MAX_ENEMY_MODEL; i++)
	{
		LoadModel(g_ModelName[i], &g_EnemyModel[i].model);
		g_EnemyModel[i].load = TRUE;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_EnemyModel[i].model, &g_EnemyModel[i].diffuse[0]);

	}


	// �K�v�Ȑ��̃G�l�~�[���A�N�e�B�u��
	for (int i = 0; i < g_CurrentMaxEnemies; i++) 
	{
		ActivateEnemy(i,SPAWN_RANGE);
	}


	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[0].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	return S_OK;
}

//=============================================================================
// �I������
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
			// �e���폜����
			if (g_Enemy[i].shadowIdx >= 0)
			{
				ReleaseShadow(g_Enemy[i].shadowIdx);
				g_Enemy[i].shadowIdx = -1; // �e�̃C���f�b�N�X�����Z�b�g
			}
			g_Enemy[i].use = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ�
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes

				// ���̃G�l�~�[�Ƃ̋������m�F
			for (int j = 0; j < MAX_ENEMY; j++)
			{
				if (i != j && g_Enemy[j].use == TRUE)
				{
					XMFLOAT3 diff;
					diff.x = g_Enemy[j].pos.x - g_Enemy[i].pos.x;
					diff.y = g_Enemy[j].pos.y - g_Enemy[i].pos.y;
					diff.z = g_Enemy[j].pos.z - g_Enemy[i].pos.z;

					float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
					float minDistanceSq = MIN_DISTANCE * MIN_DISTANCE; // MIN_DISTANCE�̓G�l�~�[�Ԃ̍ŏ�����

					// �Փˉ��
					if (distanceSq < minDistanceSq)
					{
						// diff���t�����ɐݒ�
						XMVECTOR avoidDir = -XMLoadFloat3(&diff);
						avoidDir = XMVector3Normalize(avoidDir);
						XMStoreFloat3(&diff, avoidDir);

						g_Enemy[i].pos.x += diff.x * AVOID_SPEED;
						g_Enemy[i].pos.y += diff.y * AVOID_SPEED;
						g_Enemy[i].pos.z += diff.z * AVOID_SPEED;
					}
				}
			}



			// �G�l�~�[����v���C���[�ւ̕����x�N�g�����v�Z
			XMFLOAT3 direction;
			direction.x = player->pos.x - g_Enemy[i].pos.x;
			direction.y = 0;								 // Y���͕ύX���Ȃ��i�����ړ��̂݁j
			direction.z = player->pos.z - g_Enemy[i].pos.z;

			// �����x�N�g���𐳋K��
			XMVECTOR dirVector = XMLoadFloat3(&direction);
			dirVector = XMVector3Normalize(dirVector);
			XMStoreFloat3(&direction, dirVector);

			// �G�l�~�[�̌������v���C���[�Ɍ�����
			g_Enemy[i].rot.y = atan2f(direction.x, direction.z) + XM_PI;

			// �G�l�~�[��O�i������
			float moveSpeed = VALUE_MOVE;					 // �ړ����x
			g_Enemy[i].pos.x += direction.x * moveSpeed;
			g_Enemy[i].pos.z += direction.z * moveSpeed;

			// �G�l�~�[���|���ꂽ
			if (g_Enemy[i].hp <= 0)
			{
				// �p�[�e�B�N���̃p�����[�^
				const int particleCount = 150;				// ��������p�[�e�B�N���̐�
				const float particleSize = 10.0f;			// �p�[�e�B�N���̃T�C�Y
				const int particleLife = 60;				// �p�[�e�B�N���̎����i�t���[�����j

				// �p�[�e�B�N���̐���
				for (int j = 0; j < particleCount; j++)
				{
					// �p�[�e�B�N���̈ʒu�i�G�l�~�[�̌��݈ʒu�j
					XMFLOAT3 pos = g_Enemy[i].pos;

					// �p�[�e�B�N���̈ړ������Ƒ��x�i�����_���j
					XMFLOAT3 move = XMFLOAT3(
						((float)rand() / RAND_MAX - 0.5f) * 2.0f, // X����
						((float)rand() / RAND_MAX - 0.5f) * 2.0f, // Y����
						((float)rand() / RAND_MAX - 0.5f) * 2.0f  // Z����
					);

					// �p�[�e�B�N���̐F�i�ԐF�A�������j
					XMFLOAT4 color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.7f);

					// �p�[�e�B�N���̐���
					SetParticle(pos, move, color, particleSize, particleSize, particleLife);

				}


				g_Enemy[i].use = FALSE;
				ReleaseShadow(g_Enemy[i].shadowIdx);
				// �v���C���[�̋߂��ɐV�����G�l�~�[��3�̐���
				SpawnNewEnemy(player, SPAWN_RANGE);

				g_EnemyDefeatedCount++;
				if (g_EnemyDefeatedCount % 2 == 0 && g_CurrentMaxEnemies < MAX_ENEMY) // 2�̓|�����Ƃɍő吔�𑝂₷
				{
					g_CurrentMaxEnemies++;
					SpawnAdditionalEnemies(); // �V�����G�l�~�[�𐶐�
				}



			}


			// �ړ�����
			if (g_Enemy[i].tblMax > 0)	// ���`��Ԃ����s����H
			{	// ���`��Ԃ̏���
				int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Enemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);

				// frame���g�Ď��Ԍo�ߏ���������
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Enemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Enemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

			}


			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	}




#ifdef _DEBUG

	if (GetKeyboardTrigger(DIK_P))
	{
		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
		}
	}

	if (GetKeyboardTrigger(DIK_L))
	{
		// ���f���̐F�����ɖ߂��Ă���
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
		}
	}
#endif



}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// �����̐ݒ�
	SetFuchi(1);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_Enemy[i].material);

			// ���f���`��
			DrawModel(&g_EnemyModel[g_Enemy[i].modelIdx].model);

		}

	}

	SetFuchi(0);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�̎��͂Ƀ����_���Ȉʒu�𐶐�����֐�
//=============================================================================

XMFLOAT3 GenerateRandomPositionInField(float excludeRange) {
	float randX, randZ;

	do {
		randX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * MAP_W/2.5;
		randZ = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * MAP_H/2.5;
	} while (sqrt(randX * randX + randZ * randZ) < excludeRange); // �v���C���[�̋߂��͈̔͂����O

	return XMFLOAT3(randX, ENEMY_OFFSET_Y, randZ);
}
//=============================================================================
// �V�����G�l�~�[�𐶐�����֐�
//=============================================================================

void SpawnNewEnemy(PLAYER* player, float excludeRange) {
	int currentEnemyCount = 0; // ���݂̃G�l�~�[���𐔂���

	// ���݂̃G�l�~�[�̐����J�E���g
	for (int i = 0; i < MAX_ENEMY; ++i) 
	{
		if (g_Enemy[i].use) {
			currentEnemyCount++;
		}
	}

	// ���݂̃G�l�~�[�̐����ő吔��菭�Ȃ��ꍇ�A�V�����G�l�~�[�𐶐�
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
// �G�l�~�[�̎擾
//=============================================================================
ENEMY* GetEnemy()
{
	return &g_Enemy[0];
}


//=============================================================================
// �G�l�~�[�̃A�N�e�B�u��
//=============================================================================
void ActivateEnemy(int index, float excludeRange) 
{
	if (index >= 0 && index < MAX_ENEMY) {
		ENEMY* enemy = &g_Enemy[index];

		// �G�l�~�[�̈ʒu�A��]�A�X�P�[���AHP�Ȃǂ̑�����ݒ�
		enemy->pos = GenerateRandomPositionInField(excludeRange);
		enemy->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		enemy->scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		enemy->hp = ENEMY_HP;

		// �G�l�~�[���A�N�e�B�u�ɐݒ�
		enemy->use = TRUE;

		// ���̑��K�v�ȏ���������
		XMFLOAT3 shadowPos = enemy->pos;
		shadowPos.y -= ENEMY_OFFSET_Y; // �e�̈ʒu����

		enemy->shadowIdx = CreateShadow(shadowPos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

	}
}


//=============================================================================
// �G�l�~�[����ύX
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
