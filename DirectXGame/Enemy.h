#pragma once
#include"KamataEngine.h"
#include"Player.h"

#include "math.h"

using namespace KamataEngine;

class GameScene;
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

	// ワールド座標を取得(当たり判定をとるため)
	Vector3 GetWorldPosition() const;

	AABB GetAABB();

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
	    static inline const float kWalkSpeed_ = 0.02f;


		// 当たり判定サイズ
	    static inline const float kWidth_ = 0.8f;
	    static inline const float kHeight_ = 0.8f;

		// アニメーションの周期となる時間
	    static inline const float kWalkMotionAngle_ = 0.3f;//傾く範囲
	    static inline const float kWalkMotionTime_ = 0.3f;//傾く時間

		// 経過時間
	    float walkTimer_ = 0.0f;

		//やられモーション
		static inline const float kDefeatedTime_ = 0.6f;
	    static inline const float kDefeatedMotionAngleStart_ = 0.0f;
	    static inline const float kDefeatedMotionAngleEnd_ = -60.0f;
	    float counter_ = 0.0f; // カウンター

};
