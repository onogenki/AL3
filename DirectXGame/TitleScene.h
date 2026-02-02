#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "Skydome.h"

using namespace KamataEngine;

class TitleScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	void Initialize();

	void Update();

	void Draw();

	~TitleScene();

	// デスフラグのgetter
	bool IsFinished() const { return isFinished_; }

private:
	// テクスチャハンドル(リソース読み込むために必要なもの)
	uint32_t textureHandleTitle_ = 0;
	uint32_t textureHandleSpace_ = 0;
	// スプライト(2D表示物)
	Sprite* spriteTitle_ = nullptr;
	Sprite* spriteSpace_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;
	Model* skyDomeModel_ = nullptr;
	// カメラ
	Camera camera_;
	// 暗転フェード
	Fade* fade_ = nullptr;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// サウンドデータハンドル(音データそのもの)
	uint32_t titleBGM_ = 0;
	bool isTitleBGMPlaying_ = false;
	// 音声再生ハンドル(実際に再生している音)
	uint32_t titleHandle_ = 0;

	// 終了フラグ
	bool isFinished_ = false;
};