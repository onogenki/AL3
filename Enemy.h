#pragma once

#include "KamataEngine.h"
#include "Player.h"
#include "math.h"

using namespace KamataEngine;

class GameScene;

class Enemy {
public:

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 0207角
	enum Corner { 
		kRightBottom, 
		kLeftBottom, 
		kRightTop, 
		kLeftTop, 
		kNumCorner 
	};

	//0215 振るまい
	enum class Behavior {
		kUnknown = -1, // 無効な状態
		kWalk,         // 歩行状態
		kDefeated,     // やられ状態
	};

	//0209
	void Initialize(Model* enemyModel, Camera* camera, const Vector3& position);
	
	void Update();
	
	void Draw();

	// 0207
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	//0210
	AABB GetAABB();
	//ワールド座標を取得
	const Vector3 GetWorldPosition()const;
	//衝突応答
	void OnCollision(const Player* player);

	//0215 無効フラグ
	bool IsDead() const { return isDead_; }

	// 通常行動更新
	void BehaviorRootUpdate();

	// 通常行動初期化
	void BehaviorRootInitialize();

	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	//0216
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	//ワールド
	WorldTransform worldTransform_;
	//モデル
	Model* enemyModel_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

	//歩行の速さ
	static inline const float kWalkSpeed = 0.02f;
	//速度
	Vector3 velocity_ = {};

	//最初の角度
	static inline const float kWalkMotionAngleStart = 0.0f;
	//最後の角度
	static inline const float kWalkMotionAngleEnd = 5.0f;
	//アニメーションの周期となる時間
	static inline const float kWalkMotionTime = 1.0f;
	//経過時間
	float walkTimer = 0.0f;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間 <秒>
	static inline const float kTimeTurn = 0.3f;

	// 顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;

	//当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 壁抜け防ぎ
	static inline const float kNone = 0.04f;

	// 移動入力
	void UpdateMove();

	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};
	
	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	//進めるか判定
	bool CanMove(const Vector3& dir);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// 0207マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	//0215
	bool isDead_ = false;

	//0215
	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	//0215
	static inline const float kDefeatedTime = 0.6f;
	static inline const float kDefeatedMotionAngleStart = 0.0f;
	static inline const float kDefeatedMotionAngleEnd = -60.0f;
	float counter_ = 0.0f; // カウンター

	//0215
	bool isCollisionDisabled_ = false;

	//0216
	GameScene* gameScene_ = nullptr;

	//復活
	Vector3 respawnPosition_;//復活する位置
	float respawnTimer_ = 0;//復活まで測る時間
	const float respawnCountTime_ = 5.0f; //復活までのカウントダウン


};
