#pragma once

#include "KamataEngine.h"
#include "math.h"

using namespace KamataEngine;

class Player;

class Enemy {
public:
	//0209
	void Initialize(Model* enemyModel, Camera* camera, const Vector3& position);
	
	void Update();
	
	void Draw();

	//0210
	AABB GetAABB();
	//ワールド座標を取得
	Vector3 GetWorldPosition();
	//衝突応答
	void OnCollision(const Player* player);

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

	//当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

};
