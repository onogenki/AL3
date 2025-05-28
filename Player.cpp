#include "Player.h"
#include "math.h"
#include <algorithm>
#include <cassert>
#include <numbers>
using namespace KamataEngine;

void Player::Initialize(Model* player_model, uint32_t textureHandle, Camera* camera) {
	model_ = player_model;
	camera_ = camera;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
}

void Player::Update() {

	// ワールド行列更新（アフィン変換～DirectXに転送）
	worldTransformUpdate(worldTransform_);
}

void Player::Draw() {

	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}