#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

//0213
class Fade {
public:
	//0213 フェードの状態
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};

	void Initialize();
	void Update();
	void Draw();

	//0213 フェード開始
	void Start(Status status, float duration);

	//0213 フェード停止
	void Stop();

	//0213 フェード終了判定
	bool IsFinished() const;

private:
	Sprite* sprite_ = nullptr;

	//0213 現在のフェードの状態
	Status status_ = Status::None;

	//0213 フェードの持続時間
	float duration_ = 0.0f;
	//0213 経過時間カウンター
	float counter_ = 0.0f;
};