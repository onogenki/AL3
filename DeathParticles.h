#pragma once
#include <array>
#include <numbers>
#include <algorithm>
#include "KamataEngine.h"
#include "Math.h"

class DeathParticles {
public:
	void Initialize(Model* deathParticleModel, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	//0212
	bool IsFinished() { return isFinished_; }

	private:
		//パーティクルの個数
	    static inline const uint32_t kNumParticles = 8;

		//パーティクル座標配列
	    std::array<WorldTransform, kNumParticles> worldTransforms_;

		//モデル
	    Model* deathParticleModel_ = nullptr;

		//カメラ
	    Camera* camera_ = nullptr;

		//存続時間(消滅までの時間)<秒>
	    static inline const float kDuration_ = 2.0f;

		//移動の速さ
	    static inline const float kSpeed_ = 0.05f;

		//分割した1個分の角度
	    static inline const float kAngleUint_ = 2.0f * std::numbers::pi_v<float> / kNumParticles;

		//終了フラグ
	    bool isFinished_ = false;

		//経過時間カウント
	    float counter_ = 0.0f;

		//色変更オブジェクト
	    ObjectColor objectColor_;

		//色の数値
	    Vector4 color_;
};
