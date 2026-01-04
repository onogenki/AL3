#include "Enemy.h"

#include <cassert>
#include <numbers>

void Enemy::Initialize(Model* enemyModel, Camera* camera, const Vector3& position) {
	assert(enemyModel);

	model_ = enemyModel;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 1.3f / 2.0f;

	//速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};

	camera_ = camera;

}

void Enemy::Update() {

	// 移動
	worldTransform_.translation_ += velocity_;

	walkTimer_ += 1.0f / 60.0f;

	// ワールド行列更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw() 
{
	model_->Draw(worldTransform_, *camera_); 
}

void Enemy::UpdateTransformOnly() { WorldTransformUpdate(worldTransform_); }