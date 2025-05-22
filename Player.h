#pragma once
#include "KamataEngine.h"
class Player {

	public:
	void Initialize();

	void Update();

	void Draw();

	private:
	// ワールド変換データ
	    KamataEngine::WorldTransform worldTransform_;
	// モデル
	    Model* model_ = nullptr;

	uint32_t textureHandle_ = 0u;

};

