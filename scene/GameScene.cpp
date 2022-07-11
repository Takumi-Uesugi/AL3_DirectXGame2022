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
	worldTransformBeam_.scale_ = {0.33f, 0.33f, 0.33f};
	worldTransformBeam_.Initialize();

	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformEnemy_.Initialize();
}

void GameScene::Update()
{
	PlayerUpdate();
	BeamUpdate();
	EnemyUpdate();
}

void GameScene::PlayerUpdate()
{
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

void GameScene::BeamUpdate() 
{
	if (beamFlag == 1) {
		BeamMove();
	}
	BeamBorn();
	
	worldTransformBeam_.UpdateMatrix();
}

void GameScene::BeamMove() 
{
	worldTransformBeam_.translation_.z += 2.0f;
	worldTransformBeam_.rotation_.x += 0.1f;
	if (worldTransformBeam_.translation_.z >= 40) {
		beamFlag = 0;
	}
}

void GameScene::BeamBorn() {
	if (input_->PushKey(DIK_SPACE)) {
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		worldTransformBeam_.translation_.z = 0;
		beamFlag = 1;
	}
}

void GameScene::EnemyUpdate() {
	if (enemyFlag == 1) {
		EnemyMove();
	}
	EnemyBorn();
	worldTransformEnemy_.UpdateMatrix();
}

void GameScene::EnemyMove() {
	worldTransformEnemy_.translation_.z -= 0.5f;
	worldTransformEnemy_.rotation_.x -= 0.1f;
	if (worldTransformEnemy_.translation_.z  < -5) {
		enemyFlag = 0;
	}
}

void GameScene::EnemyBorn() {
	if (enemyFlag == 0) {
		int x = rand() % 80;
		float x2 = (float)x / 10 - 4;
		worldTransformEnemy_.translation_.x = x2;
		worldTransformEnemy_.translation_.z = 40;
		worldTransformEnemy_.rotation_.x -= 0;
		enemyFlag = 1;
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
	spriteBG_->Draw();

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
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	if (beamFlag == 1) 
	{
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHnadleBeam_);
	}
	modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
