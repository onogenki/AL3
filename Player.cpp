#include "Player.h"
using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera) {
	assert(model);

	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Player::Update() { Matrix4x4 affin_mat = MakeAffineMatrix(
	worldTransform_.scale_,
	worldTransform_.rotation_,
	worldTransform_.translation_
);

worldTransform.matWorld_ = affin_mat;


}

void Player::Draw() {
	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}