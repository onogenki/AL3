#include "Enemy.h"
#include"Math.h"
#include "Player.h"
#include "GameScene.h"

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
	velocity_ = {-kWalkSpeed_, 0, 0};

	camera_ = camera;

}

Vector3 Enemy::GetWorldPosition() const {

	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Enemy::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth_ / 2.0f, worldPos.y - kHeight_ / 2.0f, worldPos.z - kWidth_ / 2.0f};
	aabb.max = {worldPos.x + kWidth_ / 2.0f, worldPos.y + kHeight_ / 2.0f, worldPos.z + kWidth_ / 2.0f};

	return aabb;
}

void Enemy::Update() {

	// 変更リクエストがあったら
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kDefeated:
		default:
			counter_ = 0;
			break;
		}
		// 変更リクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {

	//歩行
	case Behavior::kWalk:

		// 移動
		worldTransform_.translation_ += velocity_;

		walkTimer_ += 1.0f / 60.0f;

		// 回転アニメーション                   傾く範囲 * 傾き時間のモーション
		worldTransform_.rotation_.x = kWalkMotionAngle_ * std::sin(std::numbers::pi_v<float> * 1.0f * walkTimer_ / kWalkMotionTime_ / 2.0f);

		// ワールド行列更新
		WorldTransformUpdate(worldTransform_);
		break;

	//やられ
	case Behavior::kDefeated:
		// 0215　アニメーションのタイマーを加算
		counter_ += 1.0f / 60.0f;
		// y,x軸回りの回転角をイージングで変化
		worldTransform_.rotation_.y += 0.3f;
		worldTransform_.rotation_.x = EaseOut(ToRadians(kDefeatedMotionAngleStart_), ToRadians(kDefeatedMotionAngleEnd_), counter_ / kDefeatedTime_);
		// ワールドトランスフォームの行列更新
		WorldTransformUpdate(worldTransform_);
		// アニメーションのタイマーが一定時間に達したらデスフラグ
		if (counter_ >= kDefeatedTime_) {
			isDead_ = true;
		}
		break;


	}
}

void Enemy::Draw() 
{
	model_->Draw(worldTransform_, *camera_); 
}

void Enemy::OnCollision(const Player* player) {

	(void)player;

	//敵がやられてるなら何もしない
	if (behavior_ == Behavior::kDefeated) {
		return;
	}

	//やられに変更
	behaviorRequest_ = Behavior::kDefeated;

	//衝突を無効化
	isCollisionDisabled_ = true;
}

void Enemy::UpdateTransformOnly() { WorldTransformUpdate(worldTransform_); }