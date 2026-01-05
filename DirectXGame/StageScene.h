#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "Player.h"

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

};
