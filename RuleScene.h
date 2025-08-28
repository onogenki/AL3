#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "skydome.h"

using namespace KamataEngine;

class RuleScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	~RuleScene();

	void Initialize();

	void Update();

	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

private:
	static inline const float kTimeSpaceMove = 2.0f;

	// ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformRule_;
	WorldTransform worldTransformRule1_;
	WorldTransform worldTransformRule2_;
	WorldTransform worldTransformRule3_;
	WorldTransform worldTransformRule4_;

	WorldTransform worldTransformSpace_;

	// 0203天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	Model* modelRule_ = nullptr;
	Model* modelRule1_ = nullptr;
	Model* modelRule2_ = nullptr;
	Model* modelRule3_ = nullptr;
	Model* modelRule4_ = nullptr;

	Model* modelSpace_ = nullptr;

	float counter_ = 0.0f;
	// 0212 終了フラグ
	bool finished_ = false;
	// 0213
	Fade* fade_ = nullptr;
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;
};