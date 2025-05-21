#pragma once
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

class Player {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	/// <summary>
	/// 初期化
	/// </summary>
	void Update();

	/// <summary>
	/// 初期化
	/// </summary>
	void Draw();

	vector3 velocity_{};

	static inline const float kAcceleration = 0;

	static inline const float kAttenuation = 0;

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};
