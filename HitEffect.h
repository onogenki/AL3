#pragma once
#include <KamataEngine.h>
#include <cstdint>

using namespace KamataEngine;

//0216
class HitEffect {
public:
	enum class State {
		kSpread, // 拡大中
		kFade,   // フェードアウト中
		kDead    // 死亡
	};

	//0216
	static void SetModel(Model* model) { hitEffectModel_ = model; }

	static void SetCamera(Camera* camera) { camera_ = camera; }
	//インスタンス生成         
	static HitEffect* Create(const Vector3& position, Camera* camera);

	void Update();

	void Draw();

	bool IsDead() const { return state_ == State::kDead; }

private:
	HitEffect() = default;

	void Initialize(const Vector3& position);

	// 拡大アニメーションの時間
	static inline const uint32_t kSpreadTime = 10;

	// フェードアウトアニメーションの時間
	static inline const uint32_t kFadeTime = 20;

	// エフェクトの寿命
	static inline const uint32_t kLifetime = kSpreadTime + kFadeTime;

	//0216 モデル借りてくる用
	static Model* hitEffectModel_;//共有したい変数には"static"
	static Camera* camera_;

	// 楕円エフェクトの数
	static const inline uint32_t kellipseEffectNum = 2;

	// 楕円のワールドトランスフォーム
	std::array<WorldTransform, kellipseEffectNum> ellipseWorldTransforms_;

	// 円のワールドトランスフォーム
	WorldTransform circleWorldTransform_;

	State state_ = State::kSpread;

	// カウンター
	uint32_t counter_ = 0;

	ObjectColor objectColor_;
};