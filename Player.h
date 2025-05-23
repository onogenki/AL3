#pragma once
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

class Player {

	public:

	void Initialize(Model* model, Camera* camera, const Vector3& position);

	void Update();

	void Draw();

	static inline const float kAcceleration = 0;

	static inline const float kAttenuation = 0;

	private:
	// ワールド変換データ
	    KamataEngine::WorldTransform worldTransform_;
	// モデル
	    Model* model_ = nullptr;

		Camera* camera_ = nullptr;
};

