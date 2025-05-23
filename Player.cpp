#include "Player.h"
#include "Math.h"
#include <algorithm>
#include <cassert>
#include <numbers>
using namespace KamataEngine;


void Player::Initialize(player_model_, textureHandle_, &camera_) {

	camera_ = camera;
}

void Player::Update() {

	// ワールド行列更新（アフィン変換～DirectXに転送）
	worldTransformUpdate(worldTransform_);
}

void Player::Draw() {

// モデル描画
	model_->Draw(worldTransform_, *camera_);

}