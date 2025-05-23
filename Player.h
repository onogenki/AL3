#pragma once
#include "GameScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Player {

public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	void Update();

	void Draw();


private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

	Camera* camera_ = nullptr;
};