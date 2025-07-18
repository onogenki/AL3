#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

//タイトルシーン
class TitleScene {
public:
	~TitleScene();
	
	void Initialize();

	void Update();

	void Draw();

	//26枚目 デスフラグのgetter
	bool IsFinished() const { return finished_; }

private:
	static inline const float kTimeTitleMove = 2.0f;

	//ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformTitle_;
	WorldTransform worldTransformPlayer_;

	Model* modelPlayer_ = nullptr;
	Model* modelTitle_ = nullptr;

	float counter_ = 0.0f;
	//26枚目 終了フラグ
	bool finished_ = false;
};