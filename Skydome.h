#pragma once
#include"KamataEngine.h"
#include"Math.h"

using namespace KamataEngine;
class Skydome {
public:
	void Initialize(Model* skydomeModel_, Camera* camera_);

	void Update();

	void Draw();

	private:

	//ワールド変換データ
	    WorldTransform worldTransform_;

		Model* model_ = nullptr;

		Camera* camera_ = nullptr;
};
