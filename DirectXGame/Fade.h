#pragma once
#include <KamataEngine.h>

using namespace KamataEngine;

class Fade {
public:
	// フェードの更新
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};

	void Initialize();

	void Update();

	void Draw();

	// フェード開始
	void Start(Status status, float duration);
	// フェード終了
	void Stop();

	// フェード終了判定関数
	bool IsFinished() const;

private:
	// 暗転フェード
	uint32_t BlackTextureHandle_ = 0;
	Sprite* BlackSprite_ = nullptr;

	// 現在のフェードの更新
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;
};