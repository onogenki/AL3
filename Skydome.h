#pragma once
#include "KamataEngine.h"
#include "math.h"

using namespace KamataEngine;

class Skydome {

public:
	void Initialize(Model* modelSkydome_, Camera* camera_);

	void Update();

	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

	Camera* camera_ = nullptr;
};