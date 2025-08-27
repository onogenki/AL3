#define NOMINMAX
#include <cassert>
#include <numbers>
#include <algorithm>
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

	respawnPosition_ = position;

	isDead_ = false;

	behavior_ = Behavior::kWalk;

	respawnTimer_ = 0.0f;

}

void Enemy::Update() {

	//0215 変更リクエストがあったら
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kDefeated:
			counter_ = 0;
			break;
		default://どのcaseにも一致しない
			//通常初期化
			BehaviorRootInitialize();
			break;
		}

		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	//死んだら
	if (isDead_) {
		respawnTimer_ += 1.0f / 60.0f;
		if (respawnTimer_ >= respawnCountTime_) {//復活
			isDead_ = false;
			isCollisionDisabled_ = false;
			behavior_ = Behavior::kWalk;
			worldTransform_.translation_ = respawnPosition_;
			worldTransform_.rotation_ = {0.0f, std::numbers::pi_v<float> * 3.0f / 2.0f, 0.0f}; 
		}
		return;
	}

	//0215
	switch (behavior_) {
	// 歩行
	case Behavior::kWalk:

		BehaviorRootUpdate(); // 通常

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
			respawnTimer_=0.0f;

			//復活する場所に移動
			  worldTransform_.translation_ = respawnPosition_;
		}
		break;
	}
}


// 通常行動初期化
void Enemy::BehaviorRootInitialize() {}

// 通常行動更新
void Enemy::BehaviorRootUpdate() {

	UpdateMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;
	collisionMapInfo.hitWall = false;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 進めなかった場合のみ方向転換
	if (collisionMapInfo.hitWall) {

		// 候補を調べる
		Vector3 directions[4];
		int n = 0;
		if (std::abs(velocity_.x) > 0.0f) {        // 横に動いて詰まった
			directions[n++] = {0, kWalkSpeed, 0};  // 上
			directions[n++] = {0, -kWalkSpeed, 0}; // 下
			directions[n++] = {kWalkSpeed, 0, 0};  // 右
			directions[n++] = {-kWalkSpeed, 0, 0}; // 左
		} else {                                   // 縦に動いて詰まった
			directions[n++] = {kWalkSpeed, 0, 0};  // 右
			directions[n++] = {-kWalkSpeed, 0, 0}; // 左
			directions[n++] = {0, kWalkSpeed, 0};  // 上
			directions[n++] = {0, -kWalkSpeed, 0}; // 下
		}

		for (int i = 0; i < 4; ++i) {
			const Vector3& dir = directions[i];

			// 逆方向除外
			if (dir.x == -velocity_.x && dir.y == -velocity_.y)
				continue;

			if (CanMove(dir)) {
				velocity_ = dir;

				// 向きを変えた“このフレームで”移動量を計算し直す
				collisionMapInfo = {};
				collisionMapInfo.move = velocity_;
				CheckMapCollision(collisionMapInfo);
				break;
			}
		}
	}

	// 実際に移動
	worldTransform_.translation_ += collisionMapInfo.move;

	walkTimer += 1.0f / 60.0f;
	// 回転アニメーション
	worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

	// 旋回タイマー
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	// ワールド行列更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::UpdateMove() {

	//進路方向に進む位置
	Vector3 nextPos = worldTransform_.translation_ + velocity_; 

	// 移動先の角4点の座標を計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + velocity_, static_cast<Corner>(i));
	}

}

 // 移動判定(壁に沿って移動できる処理)
void Enemy::CheckMapCollision(Enemy::CollisionMapInfo& info) {

	const Vector3 saved = worldTransform_.translation_;

	// y方向の移動判定
	CollisionMapInfo collisionY{};
	collisionY.move = {0, info.move.y, 0}; // 動くy方向のみ取り出す
	CheckMapCollisionUp(collisionY);
	CheckMapCollisionDown(collisionY);

	//x判定はyを反映した位置から行う
	worldTransform_.translation_ = saved + collisionY.move;

	// x方向の移動判定
	CollisionMapInfo collisionX{};
	collisionX.move = {info.move.x, 0, 0}; // 動くx方向のみ取り出す
	CheckMapCollisionRight(collisionX);    // yを動かした後の位置から壁に当たるか
	CheckMapCollisionLeft(collisionX);

	 // 位置を元に戻す
	worldTransform_.translation_ = saved;

	// 出力まとめ
	info.move = {collisionX.move.x, collisionY.move.y, 0};

	info.landing = collisionY.landing;
	info.ceiling = collisionY.ceiling;
	info.hitWall = collisionX.hitWall || collisionY.hitWall || collisionY.ceiling || collisionY.landing;
}


//進めるか判定
bool Enemy::CanMove(const Vector3& dir) {
	CollisionMapInfo test = {};
	test.move = dir;
	// 移動テスト
	CheckMapCollision(test);
	// もし衝突しなければ進める
	return !test.hitWall;
}



void Enemy::CheckMapCollisionUp(CollisionMapInfo& info) {
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

void Enemy::CheckMapCollisionDown(CollisionMapInfo& info) {

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

// 0208
void Enemy::CheckMapCollisionRight(CollisionMapInfo& info) {
	// 右移動あり?
	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kNone) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kNone) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kNone));
			info.hitWall = true;
		}
	}
}

// 0208
void Enemy::CheckMapCollisionLeft(CollisionMapInfo& info) {

	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kNone) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kNone) {
		hit = true;
	}

	// ブロックにヒット？
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

// 0207
Vector3 Enemy::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Enemy::Draw() {
	// 復活の直前で点滅させる
	if (isDead_ && respawnTimer_ >= 3.0f && respawnTimer_ < respawnCountTime_) {
		// タイマーの整数部分が偶数なら描画する
		if (static_cast<int>(respawnTimer_ * 10.0f) % 2 == 0) {
			enemyModel_->Draw(worldTransform_, *camera_);
		}
	}
	// 2. 死亡中（点滅時間以外）
	else if (isDead_) {
		// 何も描画しない
		return;
	}
	// 3. 生きている状態
	else {
		// 通常通り描画する
		enemyModel_->Draw(worldTransform_, *camera_);
	}
}

const Vector3 Enemy::GetWorldPosition() const{ 

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

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

void Enemy::OnCollision(const Player *player) 
{

	if (isDead_)return;
	worldTransform_.translation_ = respawnPosition_;
	// プレイヤーが攻撃中なら敵が死ぬ
	// player.hをインクルード
	if (player->IsAttack()) {
		behaviorRequest_ = Behavior::kDefeated;
		isCollisionDisabled_ = true;

		// 敵の振るまいをやられに変更
		behaviorRequest_ = Behavior::kDefeated;
		// 0215 衝突を無効化
		isCollisionDisabled_ = true;

		//0216
		if (gameScene_) {

			const Vector3 enemyPos = GetWorldPosition();
			Vector3 playerPos = player->GetWorldPosition();

			// 敵と自キャラの中間位置にエフェクトを生成
			Vector3 effectPos;

			// 敵とplayerの中間位置にエフェクトを生成
			effectPos.x = (enemyPos.x + playerPos.x) / 2.0f;
			effectPos.y = (enemyPos.y + playerPos.y) / 2.0f;
			effectPos.z = (enemyPos.z + playerPos.z) / 2.0f;
			gameScene_->CreateEffect(effectPos);
		}
	}
}