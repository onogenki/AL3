#pragma once
#include"KamataEngine.h"

using namespace KamataEngine;

class Player {
public:
	void Initialize(Model* playerModel, uint32_t textureHandle, Camera* camera);

	void Update();

	void Draw();

private:
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	Camera* camera_ = nullptr;
};
