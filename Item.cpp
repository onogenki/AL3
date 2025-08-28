#include "Item.h"

void Item::Initialize(Model* startItem_model,Model* playerItem_model, Model* enemiesItem_model, Camera* camera, const Vector3& position) {
	startItem_model_ = startItem_model;//所属しない陣
	playerItem_model_ = playerItem_model;//player陣
	enemiesItem_model_ = enemiesItem_model;//敵陣

	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 初期は誰にも属してない陣(startItem)
	isStartColor_ = true;
	isPlayerColor_ = false;
	isEnemiesColor_ = false;

	owner_ = Owner::kNone;

	WorldTransformUpdate(worldTransform_);
}

void Item::Update(Player* player, const std::list<Enemy*>& enemies) {
	if (IsCollision(GetAABB(), player->GetAABB())) {
		// owner_メンバ変数を更新
		SetOwner(Owner::kPlayer);
		isPlayerColor_ = true;
		isEnemiesColor_ = false;
		isStartColor_ = false;
		return; // playerに触れたら敵の判定は行わない
	}
	for (Enemy* enemy : enemies) {
		if (IsCollision(GetAABB(), enemy->GetAABB())) {
			// owner_メンバ変数を更新
			SetOwner(Owner::kEnemy);
			isEnemiesColor_ = true;
			isPlayerColor_ = false;
			isStartColor_ = false;
			break; // 敵に触れたらいちいちチェックするのを防ぐ
		}
	}
}

void Item::Draw() {
	if (isStartColor_) {
		startItem_model_->Draw(worldTransform_, *camera_);
	}
	else if (isPlayerColor_) {
		playerItem_model_->Draw(worldTransform_, *camera_);
	} else if (isEnemiesColor_) {
		enemiesItem_model_->Draw(worldTransform_, *camera_);
	}
}

Vector3 Item::GetWorldPosition() const{
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Item::GetAABB()
{
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	// 最小値と最大値
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}