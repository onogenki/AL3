#define NOMINMAX

#include "Enemy.h"
#include"Math.h"
#include "MapChipField.h"

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


// 横移動と落下中
void Enemy::InputMove() {

	// 移動入力
	if (onGround_) {
		if (lrDirection_ == LRDirection::kRight) {
			// 移動
			velocity_.x = kWalkSpeed_;

		} else
			// 移動
			velocity_.x = -kWalkSpeed_;
	} // 空中
	else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

// 上下左右判定
void Enemy::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}


// 上方向判定
void Enemy::CheckMapCollisionUp(CollisionMapInfo& info) {

	// 上昇ありか
	if (info.move.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	// 移動後の自キャラの4つの角をfor文でまとめて計算
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	} // 右上点の設定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒットか
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight_ / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight_ / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight_ / 2.0f + kBlank_));
			// 天井に当たったことを記録する
			info.ceiling = true;
		}
	}
}

// 下方向判定
void Enemy::CheckMapCollisionDown(CollisionMapInfo& info) {

	// 上昇ありか
	if (info.move.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	// 移動後の自キャラの4つの角をfor文でまとめて計算
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	} // 右下点の設定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒットか
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight_ / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight_ / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight_ / 2.0f + kBlank_));
			// 地面に当たったことを記録する
			info.landing = true;
		}
	}
}

// 右方向判定
void Enemy::CheckMapCollisionRight(CollisionMapInfo& info) {

	// 左移動か
	if (info.move.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	// 移動後の自キャラの4つの角をfor文でまとめて計算
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	} // 右下点の設定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒットか
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth_ / 2.0f, 0, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth_ / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth_ / 2.0f + kBlank_));
			// 天井に当たったことを記録する
			info.hitWall = true;
			lrDirection_ = LRDirection::kLeft;
			// 旋回開始時の角度を記録する
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			// 旋回タイマーに時間を設定する
			turnTimer_ = kTimeTurn;
		}
	}
}

// 左方向判定
void Enemy::CheckMapCollisionLeft(CollisionMapInfo& info) {

	// 右移動か
	if (info.move.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	// 移動後の自キャラの4つの角をfor文でまとめて計算
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	} // 左下点の設定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒットか
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth_ / 2.0f, 0, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth_ / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth_ / 2.0f + kBlank_));
			// 天井に当たったことを記録する
			info.hitWall = true;
			lrDirection_ = LRDirection::kRight;
			// 旋回開始時の角度を記録する
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			// 旋回タイマーに時間を設定する
			turnTimer_ = kTimeTurn;
		}
	}
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

	// 歩行
	case Behavior::kWalk:

		InputMove();

		walkTimer_ += 1.0f / 60.0f;
		// 回転アニメーション                   傾く範囲 * 傾き時間のモーション
		worldTransform_.rotation_.x = kWalkMotionAngle_ * std::sin(std::numbers::pi_v<float> * 1.0f * walkTimer_ / kWalkMotionTime_ / 2.0f);
		break;

	// やられ
	case Behavior::kDefeated:
		// 0215　アニメーションのタイマーを加算
		counter_ += 1.0f / 60.0f;
		// y,x軸回りの回転角をイージングで変化
		worldTransform_.rotation_.y += 0.3f;
		worldTransform_.rotation_.x = EaseOut(ToRadians(kDefeatedMotionAngleStart_), ToRadians(kDefeatedMotionAngleEnd_), counter_ / kDefeatedTime_);
		// アニメーションのタイマーが一定時間に達したらデスフラグ
		if (counter_ >= kDefeatedTime_) {
			isDead_ = true;
		}
		break;
	}

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 移動
	worldTransform_.translation_ += collisionMapInfo.move;

	// 天井接触による落下開始
	if (collisionMapInfo.ceiling) {
		velocity_.y = 0;
	}

	UpdateOnWall(collisionMapInfo);

	UpdateOnGround(collisionMapInfo);

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		// 左右の自キャラ角度テーブル                                   右向き                    左向き(初期値と一緒)
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 3.5f, std::numbers::pi_v<float> * 1.3f / 2.0f};
		// 状態に応じた目標角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 旋回タイマーを使って角度補間
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	// これがないとワールド座標の関係上、enemyが動かない  行列の更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw() 
{
	model_->Draw(worldTransform_, *camera_); 
}

// 角の座標計算
Vector3 Enemy::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth_ / 2.0f, -kHeight_ / 2.0f, 0}, //  kRightBottom
	    {-kWidth_ / 2.0f, -kHeight_ / 2.0f, 0}, //  kLeftBottom
	    {+kWidth_ / 2.0f, +kHeight_ / 2.0f, 0}, //  kRightTop
	    {-kWidth_ / 2.0f, +kHeight_ / 2.0f, 0}  //  kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Enemy::UpdateOnGround(const CollisionMapInfo& info) {

	// 自キャラが接地状態か
	if (onGround_) { // 接地状態の処理

		// このelseブロック全部
		std::array<Vector3, kNumCorner> positionsNew;

		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
		}

		MapChipType mapChipType;
		// 真下の当たり判定を行う
		bool hit = false;

		// 左下点の判定
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}
		// 右下点の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}

		// 落下開始
		if (!hit) {
			// 空中状態に切り替える
			onGround_ = false;
		}
	} else { // 空中状態の処理
		// 着地フラグ
		if (info.landing) {
			// 着地状態に切り替える(落下を止める)
			onGround_ = true;
			// 着地時にx速度減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// y速度をゼロに
			velocity_.y = 0.0f;
		}
	}
}

// 壁接地中の処理
void Enemy::UpdateOnWall(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
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