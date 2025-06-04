#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	/// 初期化
	void Initialize(Model* player_model, Camera* camera, const Vector3& position);

	/// 更新
	void Update();

	/// 描画
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	Camera* camera_ = nullptr;
	// 02_05 移動量
	Vector3 velocity_ = {};
	//フレームごとの加速度
	static inline const float kAcceleration = 0.01f;
	//非入力時の摩擦係数
	static inline const float kAttenuation = 0.05f;
	//最高速度
	static inline const float kLimitRunSpeed = 0.3f;
	//顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//旋回時間 <秒>
	static inline const float kTimeTurn = 0.3f;
	//着地フラグ
	bool onGround_ = true;
	//ジャンプ定数
	static inline const float kJumpAcceleration = 20.0f;     //重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.98f;  //最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.5f;        //ジャンプ初速(上方向)
};