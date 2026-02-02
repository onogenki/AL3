#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"

using namespace KamataEngine;

class StageScene 
{
public:

	enum class Phase { 
		kFadeIn, // フェードイン
		kMain,   // メイン部
		kFadeOut,// フェードアウト
	};


	void Initialize();

	void Update();

	void Draw();

	~StageScene();

	bool IsFinished() const { return isFinished_; }
	private:


		//フェード
		Fade* fade_ = nullptr;
		//現在のフェーズ
		Phase phase_ = Phase::kFadeIn;

		//終了フラグ
		bool isFinished_ = false;

		// ビュープロジェクション
	    Camera camera_;
	    WorldTransform worldTransformStage1_;
	    WorldTransform worldTransformPlayer_;
		//  3Dモデル
	    Model* stage1Model_ = nullptr;
	    Model* playerModel_ = nullptr;

		// テクスチャハンドル(リソース読み込むために必要なもの)
	    uint32_t textureHandleSpace_ = 0;
	    // スプライト(2D表示物)
	    Sprite* spriteSpace_ = nullptr;

		// 天球
	    Skydome* skydome_ = nullptr;
	    Model* skyDomeModel_ = nullptr;

		//// サウンドデータハンドル(音データそのもの)
	    uint32_t selectBGM_ = 0;
	    bool isSelectBGMPlaying_ = false;
	    // 音声再生ハンドル(実際に再生している音)
		uint32_t selectHandle_ = 0;
};
