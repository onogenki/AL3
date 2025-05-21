#include "Player.h"
#include<numbers>
#include<algorithm>

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera) {
	assert(model);

	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() { Matrix4x4 affin_mat = MakeAffineMatrix(
	worldTransform_.scale_,
	worldTransform_.rotation_,
	worldTransform_.translation_
);

worldTransform.matWorld_ = affin_mat;

//移動入力
//左右移動操作
if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
	// 左右加速
	Vector3 acceleration = {};
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		//左移動中の右入力
		if (velocity_.x < 0.0f)
		{
			velocity_.x *= (1.0f - kAttenuation);
		}
		acceleration.x = kAcceleration;
	} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		if (velocity_.x > 0.0f) {
			// 速度と逆方向に入力中は急ブレーキ
			velocity_.x *= (1.0f - kAttenuation);
		}
		acceleration.x -= kAcceleration;
	} else {
		velocity_.x *= (1.0f - kAttenuation);
	}
	velocity_ += acceleration;
}
	//移動
worldTransform_.translation_ += velocity_;

//行列更新


}

void Player::Draw() {
	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}