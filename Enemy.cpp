#include <cassert>
#include <numbers>
#include "Enemy.h"
#include"Math.h"

void Enemy::Initialize(Model* enemyModel, Camera* camera, const Vector3& position) {
	//NULLチェック
	assert(enemyModel); 

	enemyModel_ = enemyModel;
	
	camera_ = camera;
	
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	//角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	//速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};
	
	walkTimer = 0.0f;
}

void Enemy::Update() {

	//移動
	worldTransform_.translation_ += velocity_;

	walkTimer += 1.0f / 60.0f;

	//回転アニメーション
	worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);



	//ワールド行列更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw() {

	//モデル描画
	enemyModel_->Draw(worldTransform_, *camera_);
}

AABB Enemy::GetAABB() { 
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;

}

Vector3 Enemy::GetWorldPosition() { Vector3 worldPos;

//ワールド行列の平行移動成分を取得(ワールド座標)

worldPos.x = worldTransform_.matWorld_.m[3][0];
worldPos.y = worldTransform_.matWorld_.m[3][1];
worldPos.z = worldTransform_.matWorld_.m[3][2];
return worldPos;
}

void Enemy::OnCollision(const Player *player) 
{
	(void)player;
}