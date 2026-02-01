#pragma once
#include"KamataEngine.h"
#include"Math.h"
#include <algorithm>
#include<array>
#include <numbers>
using namespace KamataEngine;

class DeathParticles 
{
public:

	void Initialize(Model* deathParticlesModel, Camera* camera, const Vector3& position);

	void Update();

	void Draw();

	//終了判定関数
	bool IsFinished() { return isFinished_; }

	const Vector3& GetVelocity() const { return velocity_; } // 速度

	//使い回し関数
	void Spawn(const Vector3& position);
	private:

		WorldTransform worldTransform_;

	    // モデル
	    Model* model_ = nullptr;

	    Camera* camera_ = nullptr;

	    Vector3 velocity_ = {};

		//パーティクルの個数
		static inline const uint32_t kNumParticles = 8;

		std::array<WorldTransform, kNumParticles> worldTransforms_;

		//消滅までの時間
	    static inline const float kDuration_ = 2.0f;

	    // 移動の速さ
	    static inline const float kSpeed_ = 0.05f;

	    // 分割した1個分の角度
	    static inline const float kAngleUint_ = 2.0f * std::numbers::pi_v<float> / kNumParticles;

		// 終了フラグ
	    bool isFinished_ = false;

	    // 経過時間カウント
	    float counter_ = 0.0f;

		// 色変更オブジェクト(色を薄くしていく)
	    ObjectColor objectColor_;

	    // 色の数値
	    Vector4 color_;
};
