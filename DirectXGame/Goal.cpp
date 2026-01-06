#include "Goal.h"
#include"Math.h"
#include "Player.h"
#include "GameScene.h"

#include <cassert>
#include <numbers>

void Goal::Initialize(Model* goalBModel, Model* goalLOModel, Model* goalROModel, Camera* camera, const Vector3& position) {

	assert(goalBModel);

	modelB_ = goalBModel;
	modelLO_ = goalLOModel;
	modelRO_ = goalROModel;
	worldTransformB_.Initialize();
	worldTransformLO_.Initialize();
	worldTransformRO_.Initialize();

	worldTransformB_.translation_ = position;


	//リセット
	state_ = State::kClosed;
	openTimer_ = 0.0f;

	worldTransformLO_.translation_ = worldTransformB_.translation_;
	worldTransformRO_.translation_ = worldTransformB_.translation_;
	camera_ = camera;
	WorldTransformUpdate(worldTransformB_);
	WorldTransformUpdate(worldTransformLO_);
	WorldTransformUpdate(worldTransformRO_);
}

Vector3 Goal::GetWorldPosition() const {

	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransformB_.matWorld_.m[3][0];
	worldPos.y = worldTransformB_.matWorld_.m[3][1];
	worldPos.z = worldTransformB_.matWorld_.m[3][2];

	return worldPos;
}

AABB Goal::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth_ / 2.0f, worldPos.y - kHeight_ / 2.0f, worldPos.z - kWidth_ / 2.0f};
	aabb.max = {worldPos.x + kWidth_ / 2.0f, worldPos.y + kHeight_ / 2.0f, worldPos.z + kWidth_ / 2.0f};

	return aabb;
}

void Goal::Update() { 
	
	if (state_ == State::kOpening)
	{
		openTimer_ += 0.12f;

		worldTransformLO_.translation_.x -= 0.1f; 
		worldTransformRO_.translation_.x += 0.1f;

		if (openTimer_ >= 1.0f)
		{
			state_ = State::kOpen;
		}
	}
	
	
	WorldTransformUpdate(worldTransformB_);
	WorldTransformUpdate(worldTransformLO_);
	WorldTransformUpdate(worldTransformRO_);
}

void Goal::Draw()
{
	modelB_->Draw(worldTransformB_, *camera_);
	modelLO_->Draw(worldTransformLO_, *camera_);
	modelRO_->Draw(worldTransformRO_, *camera_);
}

//space押してドア開く
void Goal::OnCollision(const Player* player) {
	(void)player;

	//space押せるようにする
	if (state_ == State::kClosed)
	{
		state_ = State::kCanOpen;
	}

	//ドアが動く
	if (state_ == State::kCanOpen && Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		state_ = State::kOpening;
		openTimer_ = 0.0f;
	}
}