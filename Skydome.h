#pragma once
#include"KamataEngine.h"

class Skydome {

	public:
	void Initialize();

	void Update();

	void Draw();

	private:
		//ワールド変換データ
	    KamataEngine::WorldTransform worldTransform_;
		//モデル
	    Model* model_ = nullptr;







};
