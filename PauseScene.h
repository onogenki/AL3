#pragma once
#include "KamataEngine.h"
#include "skydome.h"

using namespace KamataEngine;

// タイトルシーン
class PauseScene {
public:
	~PauseScene();

	void Initialize();

	void Update();

	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

	// ゲームに戻るかのgetter
	bool IsReturnToGame() const { return returnToGame_; }
	// タイトルに戻るかのgetter
	bool IsReturnToTitle() const { return returnToTitle_; }


private:

	// ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformPause_;
	WorldTransform worldTransformSpace_;
	WorldTransform worldTransformReturn_;

	// 0203天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	Model* modelPause_ = nullptr;
	Model* modelSpace_ = nullptr;
	Model* modelReturn_ = nullptr;//Enter

	float counter_ = 0.0f;
	// 0212 終了フラグ
	bool finished_ = false;

	bool returnToGame_ = false;
	bool returnToTitle_ = false;
};