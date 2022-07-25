#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void GamePlayUpdate(); //ゲームプレイ更新
	void GamePlayDraw3D(); //ゲームプレイ3D描画
	void GamePlayDraw2DBack(); //ゲームプレイ背景2D描画
	void GamePlayDraw2DNear(); //ゲームプレイ近景2D描画
	void GamePlayStart();

	void PlayerUpdate();
	void BeamUpdate();
	void BeamMove();
	void BeamBorn();
	void EnemyUpdate();
	void EnemyMove();
	void EnemyBorn();
	void EnemyJump();
	void StageUpdate();

	void DrawScore();

	void Collision();
	void CollisionPlayerEnemy();
	void CollisionBeamEnemy();

	void TitleUpdate();
	void TitleDraw2DNear();

	void GameoverUpdate();
	void GameoverDraw2Dnear();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	const int ENEMY_MAX = 10;
	const int BEAM_MAX = 10;
	const int STAGE_CUTS = 20;
	
	int sceneMode_ = 1;
	int gameTime_ = 0;
	int beamtimer_ = 0;
	int playerLife_ = 3;
	int playerTimer_ = 0;
	int gameScore_ = 0;
	int beamFlag[10] = {};
	int enemyFlag[10] = {};
	float enemySpeed[10] = {};
	float enemyJumpSpeed[10] = {};

	ViewProjection viewProjection_;

	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	uint32_t textureHandleScore_ = 0;
	Sprite* spriteScore_ = nullptr;
	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumBer_[5] = {};

	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;
	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;
	uint32_t textureHandleGameover_ = 0;
	Sprite* spriteGameover_ = nullptr;

	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_[20];

	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;
	Sprite* spriteLife_[3] = {};

	uint32_t textureHnadleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_[10];

	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_[10];

	uint32_t soundDataHandleTitleBGM_ = 0;
	uint32_t soundDataHandleGamePlayBGM_ = 0;
	uint32_t soundDataHandleGameOverBGM_ = 0;
	uint32_t soundDataHandleEnemyHitSE_ = 0;
	uint32_t soundDataHandlePlayerHitSE_ = 0;
	uint32_t voiceHandleBGM_ = 0;
};
