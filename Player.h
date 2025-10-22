#pragma once
#include"KamataEngine.h"

using namespace KamataEngine;

class Player {
public:
	void Initialize(Model* playerModel, Camera* camera);

	void Update();

	void Draw();

private:
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;

	Camera* camera_ = nullptr;

};
