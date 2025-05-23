#pragma once
#include "GameScene.h"
#include "KamataEngine.h"

class Skydome {

public:
	void Initialize(Model* model, Camera* camera);

	void Update();

	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

	Camera* camera_ = nullptr;
};