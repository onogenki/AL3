#pragma once
#include"KamataEngine.h"
#include "Player.h"
#include "math.h"

using namespace KamataEngine;

class GameScene;
// ヘッダ側ではポインタを持つだけなので前方宣言のほうが依存関係薄くなる
class MapChipField;

class Player;

class Enemy {

public:

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	//振る舞い
	enum Behavior { 
		kUnknown = -1,//無効な状態
		kWalk,        //歩行状態
		kDefeated,    //やられ
	};

	struct CollisionMapInfo
	{
		bool ceiling; // 天井衝突
		bool landing;//着地
		bool hitWall;//壁
		Vector3 move;//移動量
	};

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	void Initialize(Model* enemyModel, Camera* camera, const Vector3& position);

	void Update();
	
	void Draw();


	// 外部からポイントをセットするためのsetter
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// ワールド座標を取得(当たり判定をとるため)
	Vector3 GetWorldPosition() const;

	AABB GetAABB();

	void InputMove();

	//衝突判定
	void OnCollision(const Player* player); 


	// マップ衝突判定
	void CheckMapCollision(CollisionMapInfo& info);
	// 上下左右
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	// 角っこ
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// 設置状態の切り替え処理
	void UpdateOnGround(const CollisionMapInfo& info);

	// 壁に接触している場合の処理
	void UpdateOnWall(const CollisionMapInfo& info);

	//無効フラグ
	bool IsDead() const { return isDead_; }

	bool IsCollisionDisabled() const { return isCollisionDisabled_; }


	// kFadeInでplayerを描画させるもの
	void UpdateTransformOnly();

	private:

		WorldTransform worldTransform_;

		// マップチップフィールドによるフィールド
	    MapChipField* mapChipField_ = nullptr;

		Model* model_ = nullptr;

		Camera* camera_ = nullptr;

		Vector3 velocity_ = {};



		//歩行の速さ
	    static inline const float kWalkSpeed_ = 0.009f;

		// 当たり判定サイズ
	    static inline const float kWidth_ = 0.8f;
	    static inline const float kHeight_ = 0.8f;
	    static inline const float kBlank_ = 0.04f;

		// デスフラグ
	    bool isDead_ = false;

		// 顔の向き
	    LRDirection lrDirection_ = LRDirection::kLeft;

		// 着地時の速度減衰率
	    static inline const float kAttenuationLanding = 0.0f;

		// 旋回開始時の角度
	    float turnFirstRotationY_ = 0.0f;
	    // 旋回タイマー
	    float turnTimer_ = 0.0f;
	    // 旋回時間 <秒>
	    static inline const float kTimeTurn = 0.3f;
	    // 着地フラグ
	    bool onGround_ = true;

		static inline const float kGravityAcceleration = 0.98f / 60.0f; // 重力加速度(下方向)
	    static inline const float kLimitFallSpeed = 0.5f;               // 最大落下速度(下方向)

		// 微小な数値
	    static inline const float kGroundSearchHeight = 0.06f;

	    // 着地時の速度減衰率
	    static inline const float kAttenuationWall = 0.2f;

		// アニメーションの周期となる時間
	    static inline const float kWalkMotionAngle_ = 0.3f;//傾く範囲
	    static inline const float kWalkMotionTime_ = 0.3f;//傾く時間

		// 経過時間
	    float walkTimer_ = 0.0f;

		Behavior behavior_ = Behavior::kWalk;
	    Behavior behaviorRequest_ = Behavior::kUnknown;

		//やられモーション
		static inline const float kDefeatedTime_ = 0.6f;
	    static inline const float kDefeatedMotionAngleStart_ = 0.0f;
	    static inline const float kDefeatedMotionAngleEnd_ = -60.0f;
	    float counter_ = 0.0f; // カウンター

		// 衝突無効化
		bool isCollisionDisabled_ = false;
};
