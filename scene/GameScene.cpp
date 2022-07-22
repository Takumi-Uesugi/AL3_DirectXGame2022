#include "GameScene.h"
#include "TextureManager.h"
#include <time.h>
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete spriteBG_;
	delete modelStage_;
	delete modelPlayer_;
	delete modelBeam_;
	delete modelEnemy_; 
	delete spriteTitle_;
}

void GameScene::Initialize() {
	srand((unsigned int)time(NULL));

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	viewProjection_.eye = {0, 1, -6};
	viewProjection_.target = {0, 1, 0};
	viewProjection_.Initialize();

	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	worldTransformStage_.Initialize();

	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	textureHnadleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.33f, 0.33f, 0.33f};
		worldTransformBeam_[i].Initialize();
	}

	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();
	}

	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 400});

	textureHandleGameover_ = TextureManager::Load("gameover.png");
	spriteGameover_ = Sprite::Create(textureHandleGameover_, {0, 100});
}

void GameScene::Update() {
	gameTime_++;
	switch (sceneMode_) {
	case 0:
		GamePlayUpdate();
		break;
	case 1:
		TitleUpdate();
		break;
	case 2:
		GameoverUpdate();
		break;
	default:
		break;
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_) {
	case 0:
	case 2:
		GamePlayDraw2DBack();
		break;
	default:
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_) {
	case 0:
	case 2:
		GamePlayDraw3D();
		break;
	default:
		break;
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DNear();
		break;
	case 1:
		TitleDraw2DNear();
		break;
	case 2:
		GamePlayDraw2DNear();
		GameoverDraw2Dnear();
		break;
	default:
		break;
	}

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GamePlayUpdate() {
	PlayerUpdate();
	EnemyUpdate();
	BeamUpdate();
	Collision();
}
void GameScene::GamePlayDraw3D() {
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	for (int i = 0; i < 10; i++) {
		if (enemyFlag[i] == 1) {
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
	}
	for (int i = 0; i < 10; i++) {
		if (beamFlag[i] == 1) {
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHnadleBeam_);
		}
	}
}
void GameScene::GamePlayDraw2DBack() { spriteBG_->Draw(); }
void GameScene::GamePlayDraw2DNear() {
	char str[100];
	sprintf_s(str, "SCORE %d", gameScore);
	debugText_->Print(str, 200, 10, 2);
	sprintf_s(str, "LIFE %d", playerLife);
	debugText_->Print(str, 800, 10, 2);
}
void GameScene::GamePlayStart() {
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.UpdateMatrix();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.33f, 0.33f, 0.33f};
		worldTransformBeam_[i].UpdateMatrix();
	}
	for (int i = 0; i < 10; i++) {
		enemyFlag[i] = 0;
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].UpdateMatrix();
	}
	playerLife = 3;
	gameScore = 0;
}

void GameScene::PlayerUpdate() {
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}
	if (worldTransformPlayer_.translation_.x > 4.0f) {
		worldTransformPlayer_.translation_.x = 4.0f;
	}
	if (worldTransformPlayer_.translation_.x < -4.0f) {
		worldTransformPlayer_.translation_.x = -4.0f;
	}

	worldTransformPlayer_.UpdateMatrix();
}

void GameScene::BeamUpdate() {
	
	BeamMove();
	BeamBorn();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].UpdateMatrix();
	}
}

void GameScene::BeamMove() {
	for (int i = 0; i < 10; i++) {
		if (beamFlag[i] == 1) {
			worldTransformBeam_[i].translation_.z += 1.0f;
			worldTransformBeam_[i].rotation_.x += 0.1f;
			if (worldTransformBeam_[i].translation_.z >= 40) {
				beamFlag[i] = 0;
			}
		}
	}
}

void GameScene::BeamBorn() {
	if (input_->PushKey(DIK_SPACE) && beamtimer_ == 0) {
		for (int i = 0; i < 10; i++) {
			if (beamFlag[i] == 0) {
				worldTransformBeam_[i].translation_.x = worldTransformPlayer_.translation_.x;
				worldTransformBeam_[i].translation_.z = 0;
				beamFlag[i] = 1;
				break;
			}
		}
		beamtimer_ = 1;
	} else if (beamtimer_ >= 1) {
		if (++beamtimer_ >= 10) {
			beamtimer_ = 0;
		}
	}

}

void GameScene::EnemyUpdate() {
	
	EnemyMove();
	EnemyBorn();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].UpdateMatrix();
	}
}

void GameScene::EnemyMove() {
	for (int i = 0; i < 10; i++) {
		if (enemyFlag[i] == 1) {
			worldTransformEnemy_[i].translation_.x += enemySpeed[i];
			if (abs(worldTransformEnemy_[i].translation_.x) > 5) {
				enemySpeed[i] *= -1;
			}
			worldTransformEnemy_[i].translation_.z -= 0.5f;
			worldTransformEnemy_[i].rotation_.x -= 0.1f;
			if (worldTransformEnemy_[i].translation_.z < -5) {
				enemyFlag[i] = 0;
			}
		}
	}
}

void GameScene::EnemyBorn() {
	if (rand() % 10 == 0) {
		for (int i = 0; i < 10; i++) {
			if (enemyFlag[i] == 0) {
				int x = rand() % 80;
				float x2 = (float)x / 10 - 4;
				if (rand() % 2 == 0) {
					enemySpeed[i] = -0.2f;
				} else {
					enemySpeed[i] = 0.2f;
				}
				worldTransformEnemy_[i].translation_.x = x2;
				worldTransformEnemy_[i].translation_.z = 40;
				worldTransformEnemy_[i].rotation_.x -= 0;
				enemyFlag[i] = 1;
				break;
			}
		}
	}
}

void GameScene::Collision() {
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
	if (playerLife <= 0) {
		sceneMode_ = 2;
	}
}

void GameScene::CollisionPlayerEnemy() {
	for (int i = 0; i < 10; i++) {
		if (enemyFlag[i] == 1) {
			float dx =
			  abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			  abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);
			if (dx < 1 && dz < 1) {
				enemyFlag[i] = 0;
				playerLife--;
			}
		}
	}
}

void GameScene::CollisionBeamEnemy() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (beamFlag[j] == 1 && enemyFlag[i] == 1) {
				float dx = abs(
				  worldTransformBeam_[j].translation_.x - worldTransformEnemy_[i].translation_.x);
				float dz = abs(
				  worldTransformBeam_[j].translation_.z - worldTransformEnemy_[i].translation_.z);
				if (dx < 1 && dz < 1) {
					beamFlag[j] = 0;
					enemyFlag[i] = 0;
					gameScore++;
				}
			}
		}
	}
}

void GameScene::TitleUpdate() {
	if (input_->TriggerKey(DIK_RETURN)) {
		GamePlayStart();
		sceneMode_ = 0;
	}
}

void GameScene::TitleDraw2DNear() {
	spriteTitle_->Draw();
	if (gameTime_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

void GameScene::GameoverUpdate() {
	if (input_->PushKey(DIK_RETURN)) {
		sceneMode_ = 1;
	}
}

void GameScene::GameoverDraw2Dnear() {
	spriteGameover_->Draw();
	if (gameTime_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}