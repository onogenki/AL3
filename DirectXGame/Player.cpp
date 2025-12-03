#define NOMINMAX

#include "Player.h"
#include "Math.h"
#include "MapChipField.h"

#include <algorithm>
#include <cassert>
#include <numbers>
using namespace KamataEngine;

void Player::Initialize(Model* playerModel, Camera* camera,const Vector3& position) { 
	
	assert(playerModel);
	//モデル
	model_ = playerModel;
	//ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;//引数で受け取った初期座標を代入

	//Y軸周りに回転する(左右)
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 1.5f;
	//引数の内容をメンバ変数に記録
	camera_ = camera;
}

//移動入力
void Player ::InputMove() {

	// 移動入力
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_D)) {

				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				// 右加速
				acceleration.x += kAcceleration;

				// 右に向く
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_A)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				// 左加速
				acceleration.x -= kAcceleration;

				// 左に向く
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}

			// 加速/減速
			velocity_ += acceleration;
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_W)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}
	}
	// 空中
	else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}


//上下左右判定
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}


//上方向判定
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

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
	}
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒットか
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			//天井に当たったことを記録する
			info.ceiling = true;
		}
	}
}

void Player::Update() {

	//衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	//移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;

	//マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	//移動
	worldTransform_.translation_ += collisionMapInfo.move;

	//worldTransform_.translation_ += velocity_;

	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定 下降あり？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			//空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			worldTransform_.translation_.y = 1.0f; // めり込み排斥
			velocity_.x *= (1.0f - kAttenuation);  // 摩擦で横方向速度が減衰する
			velocity_.y = 0.0f;                    // 下方向速度をリセット
			onGround_ = true;                      // 接地状態に移行
		}
	}

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 1.5f, std::numbers::pi_v<float> * 3.0f / 2.32f};
		// 状態に応じた目標角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 旋回タイマーを使って角度補間
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	//これがないとワールド座標の関係上、playerが動かない  行列の更新
	WorldTransformUpdate(worldTransform_);
}


void Player::Draw()
{
	model_->Draw(worldTransform_, *camera_);
}

//角の座標計算
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},//kRightBottom
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},//kLeftBottom
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},//kRightTop
        {-kWidth / 2.0f, +kHeight / 2.0f, 0} //kLeftTop
    };
	return center + offsetTable[static_cast<uint32_t>(corner)];
}