#include "Player.h"
#include "Math.h"
#include <algorithm>
#include <cassert>
#include <numbers>
using namespace KamataEngine;


void Player::Initialize(Model* player_model_, Camera* camera) {
	model_ = player_model_;
	camera_ = camera;
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