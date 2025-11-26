#pragma once
#include"KamataEngine.h"

using namespace KamataEngine;

class Player {
public:

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	void Initialize(Model* playerModel, Camera* camera,const Vector3& position);

	void Update();

	void Draw();

	//geeter
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }//速度

private:
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;

	Camera* camera_ = nullptr;

	Vector3 velocity_ = {};
	// フレームごとの加速度
	static inline const float kAcceleration = 0.01f;
	// 非入力時の摩擦係数
	static inline const float kAttenuation = 0.13f;
	// 最高速度
	static inline const float kLimitRunSpeed = 0.25f;
	// 顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間 <秒>
	static inline const float kTimeTurn = 0.3f;
	// 着地フラグ
	bool onGround_ = true;
	// ジャンプ定数
	static inline const float kGravityAcceleration = 0.98f / 60.0f;// 重力加速度(下方向)
	static inline const float kLimitFallSpeed = 0.5f;              // 最大落下速度(下方向)
	static inline const float kJumpAcceleration = 20.0f / 60.0f;   // ジャンプ初速(上方向)
};
