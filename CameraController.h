#pragma once
#include "Player.h"

//前方宣言
class Player;

using namespace KamataEngine;

class CameraController {

	public:

		void Initialize(Camera* camera);

		void Update();

		void SetTarset(Player* target) { target_ = target; }

		void Reset();

		private:
		// カメラ
	    Camera camera_;
		Player* target_ = nullptr;

		// 追従対象とカメラの座標の差(オフセット)
	    Vector3 targetOffset_ = {0, 0, -15.0f};

};
