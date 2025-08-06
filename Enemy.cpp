#include <cassert>
#include <numbers>
#include "Enemy.h"
#include"Math.h"
#include "Player.h"
#include "GameScene.h"

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

	//0215 変更リクエストがあったら
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kDefeated:
		default:
			counter_ = 0;
			break;
		}

		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	//0215
	switch (behavior_) {
	// 歩行
	case Behavior::kWalk:
		// 移動
		worldTransform_.translation_ += velocity_;

		walkTimer += 1.0f / 60.0f;

		// 回転アニメーション
		worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

		// ワールド行列更新
		WorldTransformUpdate(worldTransform_);
		break;
	// やられ
	case Behavior::kDefeated:
		//0215　アニメーションのタイマーを加算
		counter_ += 1.0f / 60.0f;
		//y,x軸回りの回転角をイージングで変化
		worldTransform_.rotation_.y += 0.3f;
		worldTransform_.rotation_.x = EaseOut(ToRadians(kDefeatedMotionAngleStart), ToRadians(kDefeatedMotionAngleEnd), counter_ / kDefeatedTime);
		//ワールドトランスフォームの行列更新
		WorldTransformUpdate(worldTransform_);
		//アニメーションのタイマーが一定時間に達したらデスフラグ
		if (counter_ >= kDefeatedTime) {
			isDead_ = true;
		}
		break;
	}
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
	if (behavior_ == Behavior::kDefeated) {
		// 敵がやられているなら何もしない
		return;
	}

	// プレイヤーが攻撃中なら敵が死ぬ
	// player.hをインクルード
	if (player->IsAttack()) {
		// 敵の振るまいをやられに変更
		behaviorRequest_ = Behavior::kDefeated;

		//0215 衝突を無効化
		isCollisionDisabled_ = true;
	}
}