#pragma once
#include "KamataEngine.h"

//前方宣言
class Player;

using namespace KamataEngine;

class CameraController {

	public:

		//矩形 これから使うときは { 左(最小値)　右(最大値)　下　上 } となる
		struct Rect
		{
		    float left = 0.0f;  //左
		    float right = 1.0f; //右
		    float bottom = 0.0f;//下
		    float top = 1.0f;   //上
		};

		void Initialize(Camera* camera);

		void Update();
		//外部から値をセットするため
		void SetTarget(Player* target) { target_ = target; }

		void Reset();

		void SetMovableArea(Rect area) { movableArea_ = area; }

		private:
		// カメラ
		Camera* camera_ = nullptr;
		Player* target_ = nullptr;

		// 追従対象とカメラの座標の差(オフセット)
	    Vector3 targetOffset_ = {0, 0, -15.0f};

		// カメラ移動範囲(あくまで移動制限のため　
		// 見える範囲の設定はGameScene.cppで)
	    Rect movableArea_ = {10.5f, 100 - 12.0f, 6.0f, 6.0f};

		//カメラの目標座標
	    Vector3 destination_;

		//座標補間割合
	    static inline const float kInterpolationRate = 0.1f;
		//速度掛け算
	    static inline const float kVelocityBias = 15.0f;
		//追従対象の各方向へのカメラ移動範囲
	    static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};
};
