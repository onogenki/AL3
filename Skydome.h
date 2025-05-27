#pragma once
#include"KamataEngine.h"
#include"Math.h"
//#include "GameScene.h"

class Skydome {

	public:

	void Initialize(Model* modelSkydome_, Camera* camera_);

	void Update();

	void Draw();

	private:
		//ワールド変換データ
	  KamataEngine:: WorldTransform worldTransform_;
		//モデル
	    KamataEngine::Model* model_ = nullptr;

		KamataEngine::Camera* camera_ = nullptr;
		

};
