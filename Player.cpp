#include "Player.h"
#include "Math.h"
#include <algorithm>
#include <cassert>
#include <numbers>

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	// モデル
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	camera_ = camera;
}

void Player::Update() {

	// ワールド行列更新（アフィン変換～DirectXに転送）
	WorldTransformUpdate(worldTransform_);
}

void Player::Draw() {

	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}