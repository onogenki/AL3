#include "Player.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	assert(model);
	//モデル
	model_ = model;
	//テクスチャハンドル
	textureHandle_ = textureHandle;
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//カメラ
	camera_ = camera;
}

void Player::Update() {
	worldTransform_.TransferMatrix();
}

void Player::Draw() { 
	
	model_->Draw(worldTransform_, *camera_,textureHandle_);
};

