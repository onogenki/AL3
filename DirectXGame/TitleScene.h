#pragma once
#include "KamataEngine.h"
#include"Fade.h"

using namespace KamataEngine;


class TitleScene
{
public:

	enum class Phase
	{
		kFadeIn, //フェードイン
		kMain,   //メイン部
		kFadeOut,//フェードアウト
	};

	void Initialize();

	void Update();

	void Draw();

	~TitleScene();
	



	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

	private:

	// テクスチャハンドル(リソース読み込むために必要なもの)
	    uint32_t textureHandleTitle_ = 0;
	    uint32_t textureHandleSpace_ = 0;
	// スプライト(2D表示物)
	    Sprite* spriteTitle_ = nullptr;
	    Sprite* spriteSpace_ = nullptr;

	    //暗転フェード
	    Fade* fade_ = nullptr;

		//現在のフェーズ
	    Phase phase_ = Phase::kFadeIn;

	// サウンドデータハンドル(音データそのもの)
	uint32_t soundDataHandle_ = 0;
	// 音声再生ハンドル(実際に再生している音)
	uint32_t voiceHandle_ = 0;

	//終了フラグ
	bool finished_ = false;

};
