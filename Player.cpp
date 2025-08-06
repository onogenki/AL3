#define NOMINMAX

#include "Player.h"
#include "Math.h"
#include "MapChipField.h"

#include <cassert>
#include <numbers>
#include <algorithm>

using namespace KamataEngine;

void Player ::Update() {

	// 0214
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}

		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	WorldTransformUpdate(worldTransform_);
	WorldTransformUpdate(worldTransformAttack_);
}
	// 通常行動初期化
	void Player::BehaviorRootInitialize() {}

	// 通常行動更新
	void Player::BehaviorRootUpdate() {

		// 0207 移動入力
		InputMove();

		// 0207 衝突情報を初期化
		CollisionMapInfo collisionMapInfo = {};
		collisionMapInfo.move = velocity_;
		collisionMapInfo.landing = false;
		collisionMapInfo.hitWall = false;

		// 0207マップ衝突チェック
		CheckMapCollision(collisionMapInfo);

		// 移動
		worldTransform_.translation_ += collisionMapInfo.move;

		// 天井接触による落下開始
		if (collisionMapInfo.ceiling) {
			velocity_.y = 0;
		}

		// 0208 壁接触している場合の処理
		UpdateOnWall(collisionMapInfo);

		// 0208 接地判定
		UpdateOnGround(collisionMapInfo);
		// 旋回制御
		if (turnTimer_ > 0.0f) {
			// 旋回タイマーを1/60秒だけカウントダウンする
			turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
			// 左右の自キャラ角度テーブル
			float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
			// 状態に応じた目標角度を取得する
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			// 自キャラの角度を設定する
			worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
		}

		//0214 攻撃キーを押したら
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			// 攻撃ビヘイビアをリクエスト
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
	worldTransform_.translation_ += collisionMapInfo.move;

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
}

//0207 移動入力
void Player ::InputMove() {

	// 移動入力
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration / 40.0f;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration / 40.0f;

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

		// ほぼ0の場合に0にする
		if (std::abs(velocity_.x) <= 0.0001f) {
			velocity_.x = 0.0f;
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
		}
	}
	// 空中
	else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}
//0207
	void Player::CheckMapCollision(CollisionMapInfo & info) {

		CheckMapCollisionUp(info);
		CheckMapCollisionDown(info);
		CheckMapCollisionRight(info);
		CheckMapCollisionLeft(info);
	}

	//0207 上下左右全て
    void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

	    // 0207 上昇あり?
	    if (info.move.y <= 0) {
		    return;
	    }

	    std::array<Vector3, kNumCorner> positionsNew;

	    for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		    positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	    }

	    MapChipType mapChipType;
	    // 真上の当たり判定を行う
	    bool hit = false;

	    // 左上点の判定
	    MapChipField::IndexSet indexSet;
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	    if (mapChipType == MapChipType::kBlock) {
		    hit = true;
	    }

	    // 右上点の判定
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	    if (mapChipType == MapChipType::kBlock) {
		    hit = true;
	    }

	    // 0207ブロックにヒット？
	    if (hit) {
		    // 現在座標が壁の外か判定
		    MapChipField::IndexSet indexSetNow;
		    indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		    if (indexSetNow.yIndex != indexSet.yIndex) {
			    // めり込みを排除する方向に移動量を設定する
			    indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
			    MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			    info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			    info.ceiling = true;
		    }
	    }
    }

		void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	    info;

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

	    if (mapChipType == MapChipType::kBlock) {
		    hit = true;
	    }

	    // 右下点の判定
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	    if (mapChipType == MapChipType::kBlock) {
		    hit = true;
	    }

	    // ブロックにヒット？
	    if (hit) {
		    // めり込みを排除する方向に移動量を設定する
		    indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		    // めり込み先ブロックの範囲矩形
		    MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		    info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		    // 地面に当たったことを記録する
		    info.landing = true;
	    }
    }

        //設置状態の切り替え処理
        void Player::UpdateOnGround(const CollisionMapInfo& info) {

	        info;

	        if (onGround_) {
		        //ジャンプ開始
		        if (velocity_.y > 0.0f) {
			        onGround_ = false;
		        } else {
			        //落下判定
			        //落下なら空中状態に切り替え

			        //このelseブロック全部
			        std::array<Vector3, kNumCorner> positionsNew;

			        for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				        positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			        }
					//真下の当たり判定
			        bool hit = false;

			        MapChipType mapChipType;

			        //左下点の判定
			        MapChipField::IndexSet indexSet;
			        indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			        mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			        //隣接セルがともにブロックであればヒット
			        if (mapChipType == MapChipType::kBlock) {
				        hit = true;
			        }

			        //右下点の判定
			        indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			        mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			        if (mapChipType == MapChipType::kBlock) {
				        hit = true;
			        }

			        //落下開始
			        if (!hit) {
				        //空中状態に切り替える				
				        onGround_ = false;
			        }
		        }
	        } else {
		        //地面に接触している場合の処理
		        if (info.landing) {
			        //着地状態に切り替える（落下を止める）
			        onGround_ = true;
			        //着地時にX速度を減衰
			        velocity_.x *= (1.0f - kAttenuationLanding);
			        //Y速度をゼロに
			        velocity_.y = 0.0f;
		        }
	        }
        }

        //壁接地中の処理
        void Player::UpdateOnWall(const CollisionMapInfo& info) {
			//壁接触による減速
	        if (info.hitWall) {
		        velocity_.x *= (1.0f - kAttenuationWall);
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

	        if (mapChipType == MapChipType::kBlock) {
		        hit = true;
	        }

	        //右下点の判定
	        indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	        mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	        if (mapChipType == MapChipType::kBlock) {
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
			        info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
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

	        if (mapChipType == MapChipType::kBlock) {
		        hit = true;
	        }

	        //左下点の判定
	        indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	        mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	        if (mapChipType == MapChipType::kBlock) {
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
			        info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
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

//0210 ワールド座標を取得
Vector3 Player::GetWorldPosition() {
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
	
		//0215
	if (IsAttack()) {
		return; // 攻撃中はダメージ無効
	}
	//不使用
	(void)enemy;
	//0212 書き換え
	isDead_ = true;

	//0215
	isCollisionDisabled_ = true; // 衝突無効化
}