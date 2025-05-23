#pragma once
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

class Player {

	public:

	void Initialize(player_model_, textureHandle_, &camera_);

	void Update();

	void Draw();

	private:
	// ワールド変換データ
	    WorldTransform worldTransform_;
	// モデル
	    Model* model_ = nullptr;

		Camera* camera_ = nullptr;
};

