#pragma once

#include "KamataEngine.h"

using namespace KamataEngine;

class Enemy {
public:
	//0209
	void Initialize(Model* enemyModel, Camera* camera, const Vector3& position);
	
	void Update();
	
	void Draw();

private:
	//ワールド
	WorldTransform worldTransform_;
	//モデル
	Model* enemyModel_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

	//歩行の速さ
	static inline const float kWalkSpeed = 0.02f;
	//速度
	Vector3 velocity_ = {};

	//最初の角度
	static inline const float kWalkMotionAngleStart = 0.0f;
	//最後の角度
	static inline const float kWalkMotionAngleEnd = 30.0f;
	//アニメーションの周期となる時間
	static inline const float kWalkMotionTime = 1.0f;
	//経過時間
	float walkTimer = 0.0f;
};
