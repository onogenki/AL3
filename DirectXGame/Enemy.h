#pragma once
#include"KamataEngine.h"

#include "math.h"

using namespace KamataEngine;

// ヘッダ側ではポインタを持つだけなので前方宣言のほうが依存関係薄くなる
class MapChipField;

class Enemy {

public:

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	void Initialize(Model* enemyModel, Camera* camera, const Vector3& position);

	void Update();
	
	void Draw();

	// 外部からポイントをセットするためのsetter
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// kFadeInでplayerを描画させるもの
	void UpdateTransformOnly();

	private:

		WorldTransform worldTransform_;

		// マップチップフィールドによるフィールド
	    MapChipField* mapChipField_ = nullptr;

		Model* model_ = nullptr;

		Camera* camera_ = nullptr;

		Vector3 velocity_ = {};

		// 顔の向き
	    LRDirection lrDirection_ = LRDirection::kLeft;

		//歩行の速さ
	    static inline const float kWalkSpeed = 0.02f;

		float walkTimer_ = 0.0f;

		// 当たり判定サイズ
	    static inline const float kWidth = 0.8f;
	    static inline const float kHeight = 0.8f;

};
