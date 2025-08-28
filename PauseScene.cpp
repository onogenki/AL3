#include "Math.h"
#include "PauseScene.h"
#include <numbers>

void PauseScene::Initialize() {

	modelPause_ = Model::CreateFromOBJ("pauseFont", true);
	modelSpace_ = Model::CreateFromOBJ("spaceGameFont", true);
	modelReturn_ = Model::CreateFromOBJ("EnterFont", true);

	// カメラ初期化
	camera_.Initialize();

	const float kPlayerTitle = 2.0f;

	worldTransformPause_.Initialize();

	worldTransformPause_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};

	worldTransformSpace_.Initialize();

	worldTransformSpace_.translation_.x = -2.0f;

	worldTransformSpace_.translation_.y = -1.0f;

	worldTransformSpace_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};

	const float kPlayerScale = 20.0f;

	worldTransformReturn_.Initialize();

	worldTransformReturn_.scale_ = {kPlayerScale, kPlayerScale, kPlayerScale};

	worldTransformReturn_.translation_.y = -10.0f;

	// 0203天球
	// skydome生成
	skydome_ = new Skydome();
	// 初期化
	modelSkydome_ = Model::CreateFromOBJ("sky_sphere", true);
	skydome_->Initialize(modelSkydome_, &camera_);
}

void PauseScene::Update() {

	
	// Enterキーでタイトルに戻る
	if (Input::GetInstance()->PushKey(DIK_RETURN)) {
		returnToTitle_ = true;
	}
	// Spaceキーでゲームに戻る
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		returnToGame_ = true;
	}


	camera_.TransferMatrix();

	// アフィン変換～DirectXに転送
	WorldTransformUpdate(worldTransformPause_);
	WorldTransformUpdate(worldTransformSpace_);
	WorldTransformUpdate(worldTransformReturn_);
}

void PauseScene::Draw() {

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	// 天球描画
	skydome_->Draw();

	modelPause_->Draw(worldTransformPause_, camera_);
	modelSpace_->Draw(worldTransformSpace_, camera_);
	modelReturn_->Draw(worldTransformReturn_, camera_);

	Model::PostDraw();
}

PauseScene::~PauseScene() {
	delete modelPause_;
	delete modelReturn_;
	delete modelSpace_;
	delete skydome_;//newSkydomeの解放
	delete modelSkydome_;
}