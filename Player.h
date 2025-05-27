#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player {

	public:

	void Initialize(Model* player_model_, Camera* camera_);

	void Update();

	void Draw();

	private:
	// ワールド変換データ
	    WorldTransform worldTransform_;
	// モデル
	    Model* model_ = nullptr;

		Camera* camera_ = nullptr;

		uint32_t textureHandle_ = 0u;
};

