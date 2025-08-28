#pragma once
#include "KamataEngine.h"
#include "skydome.h"
#include "Fade.h"

using namespace KamataEngine;

//タイトルシーン
class TitleScene {
public:

	enum class Phase {
		kFadeIn, //フェードイン
		kMain,   //メイン部
		kFadeOut,//フェードアウト
	};

	~TitleScene();
	
	void Initialize();

	void Update();

	void Draw();

	//デスフラグのgetter
	bool IsFinished() const { return finished_; }

private:
	static inline const float kTimeTitleMove = 2.0f;

	//ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformTitle_;
	WorldTransform worldTransformSpace_;
	WorldTransform worldTransformPlayer_;

	// 0203天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	Model* modelPlayer_ = nullptr;
	Model* modelTitle_ = nullptr;
	Model* modelSpace_ = nullptr;

	float counter_ = 0.0f;
	//0212 終了フラグ
	bool finished_ = false;
	//0213
	Fade* fade_ = nullptr;
	//現在のフェーズ
	Phase phase_ = Phase::kFadeIn;
};