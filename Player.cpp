#define NOMINMAX

#include "Player.h"
#include "Math.h"
#include "MapChipField.h"

#include <cassert>
#include <numbers>
#include <algorithm>

using namespace KamataEngine;

void Player ::Update() {

	if (isDead_) {
		respawnTimer_ += 1.00f / 60.0f;

		if (respawnTimer_ >= respawnCountTime_) {
			isDead_ = false;
			respawnTimer_ = 0.0f;
			worldTransform_.translation_ = respawnPosition_;

			velocity_ = {0.0f, 0.0f, 0.0f};
		}
		return;
	}

	//behaviorRequestにリクエストがきていたらbehaviorを変更し、初期化関数を1度呼び出す
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();//通常へ
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();//攻撃へ
			break;
		}

		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}
	//現在の行動に応じて更新処理
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();//通常
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();//攻撃
		break;
	}
	//ワールド行列更新
	WorldTransformUpdate(worldTransform_);
	WorldTransformUpdate(worldTransformAttack_);
}
	// 通常行動初期化
	void Player::BehaviorRootInitialize() {}

	// 通常行動更新
    void Player::BehaviorRootUpdate() {

	   // 0207 移動入力
	    InputMove();

	    // 衝突情報を初期化
	    CollisionMapInfo collisionMapInfo = {};
	    collisionMapInfo.move = velocity_;
	    collisionMapInfo.landing = false;
	    collisionMapInfo.hitWall = false;

	    // マップ衝突チェック
	    CheckMapCollision(collisionMapInfo);

	    // 修正された移動量で実際に移動
	    //worldTransform_.translation_ += collisionMapInfo.move;

		// 旋回タイマー
	    if (turnTimer_ > 0.0f) {
		    turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		    float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		    float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		    worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	    }

	    // 攻撃入力
	    if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		    behaviorRequest_ = Behavior::kAttack;
	    }

    }
    //0214 攻撃行動初期化
void Player::BehaviorAttackInitialize() {

	//0214 カウンター初期化
	attackParameter_ = 0;

	velocity_ = {};

	// 溜めフェーズから始める
	attackPhase_ = AttackPhase::kAnticipation;
}

//0214 攻撃行動更新
void Player::BehaviorAttackUpdate() {

	const Vector3 attackVelocity = {0.8f, 0.0f, 0.0f};

	//0214 攻撃動作用の速度
	Vector3 velocity{};

	//0214 予備動作
	attackParameter_++;
	//攻撃フェーズごとの更新処理
	switch (attackPhase_) {
	case AttackPhase::kAnticipation: // 溜め動作
	default: {
		velocity = {};
		float t = static_cast<float>(attackParameter_) / kAnticipationTime;
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);

		//トランスフォームの値をコピー
		worldTransformAttack_.translation_ = worldTransform_.translation_;
		worldTransformAttack_.rotation_ = worldTransform_.rotation_;

		// 前進動作へ移行
		if (attackParameter_ >= kAnticipationTime) {
			attackPhase_ = AttackPhase::kAction;
			attackParameter_ = 0; // カウンターをリセット
		}
		break;
	}
	case AttackPhase::kAction: { // 突進動作
		if (lrDirection_ == LRDirection::kRight) {
			velocity = +attackVelocity;
		} else {
			velocity = -attackVelocity;
		}

		float t = static_cast<float>(attackParameter_) / kActionTime;
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);

		// 余韻動作へ移行
		if (attackParameter_ >= kActionTime) {
			attackPhase_ = AttackPhase::kRecovery;
			attackParameter_ = 0; // パラメータをリセット
		}
	} break;

	case AttackPhase::kRecovery: { // 余韻動作
		velocity = {};
		float t = static_cast<float>(attackParameter_) / kRecoveryTime;
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);

		// 既定の時間経過で攻撃終了して通常行動に戻る
		if (attackParameter_ >= kRecoveryTime) {
			behaviorRequest_ = Behavior::kRoot;
		}
		break;
	}
	}

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity;
	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 移動
	//worldTransform_.translation_ += collisionMapInfo.move;

	if (turnTimer_ > 0.0f) {
		// タイマーを進める
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
}

void Player::Initialize(Model* player_model, Model* modelAttack, Camera* camera, const Vector3& position) {

	assert(player_model);
	// モデル
	model_ = player_model;

	//攻撃用モデル
	modelAttack_ = modelAttack;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;

	camera_ = camera;

	respawnPosition_ = position;

	isDead_ = false;

	respawnTimer_ = 0.0f;
}

//0207 移動入力
void Player ::InputMove() {

	// 左右加速
	Vector3 acceleration = {};
	if (Input::GetInstance()->PushKey(DIK_D)) {

		// 左移動中の右入力
		if (velocity_.x < 0.0f) {
			// 速度と逆方向に入力中は急ブレーキ
			velocity_.x *= (1.0f - kAttenuation);
		}

		acceleration.x += kAcceleration / 55.0f;

		if (lrDirection_ != LRDirection::kRight) {
			lrDirection_ = LRDirection::kRight;
			// 旋回開始時の角度を記録する
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			// 旋回タイマーに時間を設定する
			turnTimer_ = kTimeTurn;
		}
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {

		// 右移動中の左入力
		if (velocity_.x > 0.0f) {
			// 速度と逆方向に入力中は急ブレーキ
			velocity_.x *= (1.0f - kAttenuation);
		}

		acceleration.x -= kAcceleration / 55.0f;

		if (lrDirection_ != LRDirection::kLeft) {
			lrDirection_ = LRDirection::kLeft;
			// 旋回開始時の角度を記録する
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			// 旋回タイマーに時間を設定する
			turnTimer_ = kTimeTurn;
		}
	}
	if (Input::GetInstance()->PushKey(DIK_W)) {

		// 上移動中の左入力
		if (velocity_.y < 0.0f) {
			// 速度と逆方向に入力中は急ブレーキ
			velocity_.y *= (1.0f - kAttenuation);
		}

		acceleration.y += kAcceleration / 55.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {

		// 下移動中の左入力
		if (velocity_.y > 0.0f) {
			// 速度と逆方向に入力中は急ブレーキ
			velocity_.y *= (1.0f - kAttenuation);
		}

		acceleration.y -= kAcceleration / 55.0f;
	}

	// 加速/減速
	velocity_ += acceleration;
	// 最大速度制限
	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	velocity_.y = std::clamp(velocity_.y, -kLimitRunSpeed, kLimitRunSpeed);

	velocity_ *= (1.04f - kAttenuation);
	// 入力がない軸だけ減衰
	if (!Input::GetInstance()->PushKey(DIK_D) && !Input::GetInstance()->PushKey(DIK_A)) {
		velocity_.x *= (1.0f - kAttenuation);
	}
	if (!Input::GetInstance()->PushKey(DIK_W) && !Input::GetInstance()->PushKey(DIK_S)) {
		velocity_.y *= (1.0f - kAttenuation);
	}

	// ほぼ0の場合に0にする
	if (std::abs(velocity_.x) <= 0.0001f) {
		velocity_.x = 0.0f;
	}

	if (std::abs(velocity_.y) <= 0.0001f) {
		velocity_.y = 0.0f;
	}

	// 移動先の角4点の座標を計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + velocity_, static_cast<Corner>(i));
	}
}
    //移動判定(壁に沿って移動できる処理)
	void Player::CheckMapCollision(CollisionMapInfo & info) {
		
		//y方向の移動判定
		CollisionMapInfo collisionY = {};
	    collisionY.move = {0, info.move.y, 0};//動くy方向のみ取り出す
	    CheckMapCollisionUp(collisionY);
	    CheckMapCollisionDown(collisionY);
	    worldTransform_.translation_ += collisionY.move;//y更新
		//x方向の移動判定
	    CollisionMapInfo collisionX = {};
	    collisionX.move = {info.move.x, 0, 0};//動くx方向のみ取り出す
	    CheckMapCollisionRight(collisionX);//yを動かした後の位置から壁に当たるか
	    CheckMapCollisionLeft(collisionX);
	    worldTransform_.translation_ += collisionX.move;//x更新

		info.move.x = collisionX.move.x;
	    info.move.y = collisionY.move.y;
	}


	//0207 上下左右全て
	void Player::CheckMapCollisionUp(CollisionMapInfo & info) {
		// 上昇がないなら処理しない
		if (info.move.y <= 0) {
			return;
		}

		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
		}

		MapChipType mapChipType;
		bool hit = false;

		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kNone) {
			hit = true;
		}

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kNone) {
			hit = true;
		}

		if (hit) {
			MapChipField::IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
			if (indexSetNow.yIndex != indexSet.yIndex) {
				indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kNone));
				info.ceiling = true;
			}
		}
	}

		void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	    // 0208下降あり？
	    if (info.move.y >= 0) {
		    return;
	    }

	    // 移動後の4つの角の座標の計算

	    std::array<Vector3, kNumCorner> positionsNew;

	    for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		    positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	    }

	    //(右下、左下の判定まで)
	    MapChipType mapChipType;
	    // 真上の当たり判定を行うフラグ初期化
	    bool hit = false;

	    // 左下の判定
	    MapChipField::IndexSet indexSet;
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	    if (mapChipType == MapChipType::kNone) {
		    hit = true;
	    }

	    // 右下点の判定
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	    if (mapChipType == MapChipType::kNone) {
		    hit = true;
	    }

	    // ブロックにヒット？
	    if (hit) {
		    // めり込みを排除する方向に移動量を設定する
		    indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		    // めり込み先ブロックの範囲矩形
		    MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		    info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kNone));
		    // 地面に当たったことを記録する
		    info.landing = true;
	    }
    }

		//0208
        void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
			//右移動あり?
	        if (info.move.x <= 0) {
		        return;
	        }

	        std::array<Vector3, kNumCorner> positionsNew;

	        for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		        positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	        }

	        MapChipType mapChipType;
	        //右側の当たり判定
	        bool hit = false;

	        //右上点の判定
	        MapChipField::IndexSet indexSet;
	        indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	        mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	        if (mapChipType == MapChipType::kNone) {
		        hit = true;
	        }

	        //右下点の判定
	        indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	        mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	        if (mapChipType == MapChipType::kNone) {
		        hit = true;
	        }

	        //ブロックにヒット？
	        if (hit) {
		        //現在座標が壁の外か判定
		        MapChipField::IndexSet indexSetNow;
		        indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		        if (indexSetNow.xIndex != indexSet.xIndex) {
			        //めり込みを排除する方向に移動量を設定する
			        indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
			        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			        info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kNone));
			        info.hitWall = true;
		        }
	        }
        }

        //0208
        void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {

	        if (info.move.x >= 0) {
		        return;
	        }

	        std::array<Vector3, kNumCorner> positionsNew;

	        for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		        positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	        }

	        MapChipType mapChipType;
	        //右側の当たり判定
	        bool hit = false;

	        //左上点の判定
	        MapChipField::IndexSet indexSet;
	        indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	        mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	        if (mapChipType == MapChipType::kNone) {
		        hit = true;
	        }

	        //左下点の判定
	        indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	        mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	        if (mapChipType == MapChipType::kNone) {
		        hit = true;
	        }

	        //ブロックにヒット？
	        if (hit) {
		        // 現在座標が壁の外か判定
		        MapChipField::IndexSet indexSetNow;
		        indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));

		        if (indexSetNow.xIndex != indexSet.xIndex) {
			        // めり込みを排除する方向に移動量を設定する
			        indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
			        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			        info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kNone));
			        info.hitWall = true;
		        }
	        }
        }


		//0207
		Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

			Vector3 offsetTable[] = {
			    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
			    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
			    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
			    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
			};

			return center + offsetTable[static_cast<uint32_t>(corner)];
		}
		
		

void Player::Draw() {

	// モデル描画
	model_->Draw(worldTransform_, *camera_);
	if (behavior_ == Behavior::kAttack) {
		switch (attackPhase_) {
		case AttackPhase::kAnticipation:
		default:
			// 予備動作中は攻撃モデルを描画しない
			break;
		case AttackPhase::kAction:
		case AttackPhase::kRecovery:
			if (modelAttack_) {
				modelAttack_->Draw(worldTransformAttack_, *camera_);
			}
			break;
		}
	}
}

//0210 ワールド座標を取得 constを付けてオブジェクト変更しない
Vector3 Player::GetWorldPosition()const {
	//ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Player::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	//最小値と最大値
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {

	if (isDead_) {
		return;
	}
	// 0215
	if (IsAttack()) {
		return; // 攻撃中はダメージ無効
	}
		respawnTimer_ = 0.0f;
		// 不使用
		(void)enemy;
		// 0212 書き換え
		isDead_ = true;

		// 0215
		isCollisionDisabled_ = true; // 衝突無効化
}