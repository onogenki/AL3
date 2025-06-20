#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Skydome {

public:
	void Initialize(Model* modelSkydome_, Camera*);

	void Update();

	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

	Camera* camera_ = nullptr;
};