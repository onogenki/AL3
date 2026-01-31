#pragma once
#include"KamataEngine.h"
#include"Math.h"

using namespace KamataEngine;

//ヘッダ側ではポインタを持つだけなので前方宣言のほうが依存関係薄くなる
class GameScene;
class MapChipField;
class Enemy;
class Goal;

class Player {
public:

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	//アクション
	enum Behavior {
		kWalk,         // 歩行状態
		kClear,         // クリア
	};


	//マップとの当たり判定の情報
	struct CollisionMapInfo {
		bool ceiling = false;//天井衝突
		bool landing = false;//着地
		bool hitWall = false;//壁判定
		Vector3 move;//移動量
	};

	//角
	enum Corner
	{
		kRightBottom,//右下
		kLeftBottom,//左下
		kRightTop,//右上
		kLeftTop,//左上

		kNumCorner//要素数
	};

	void Initialize(Model* playerModel, Camera* camera,const Vector3& position);

	void Update();

	void Draw();

	// 外部からポイントをセットするためのsetter
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	//ワールド座標を取得(当たり判定をとるため)
	Vector3 GetWorldPosition() const;

	AABB GetAABB();

	// 移動入力
	void InputMove();

	//マップ衝突判定
	void CheckMapCollision(CollisionMapInfo& info);
	//上下左右
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	//クリアモーション
	void UpdateClearMove();

	//角っこ
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	//設置状態の切り替え処理
	void UpdateOnGround(const CollisionMapInfo& info);

	// 壁に接触している場合の処理
	void UpdateOnWall(const CollisionMapInfo& info);

	//geeter
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }//速度
	const float GetkBlank() const { return kBlank_; }//敵とplayerの当たるまでの空白
	Behavior GetBehavior() const { return behavior_; } // 状態取得

	// クリア後フラグ
	bool IsClearMotionFinished() const { return isClearMotionFinished_; } 
	// デスフラグ
	bool IsDead() const { return isDead_; }

	void Bounce();

	void OnCollision(const Enemy* enemy);

	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	// seeter 状態変更
	void SetBehavior(Behavior behavior) { behavior_ = behavior; }

	//kFadeInでplayerを描画させるもの
	void UpdateTransformOnly();

private:
	WorldTransform worldTransform_;

	//マップチップフィールドによるフィールド
	MapChipField* mapChipField_ = nullptr;

	//モデル
	Model* model_ = nullptr;

	Camera* camera_ = nullptr;

	Vector3 velocity_ = {};

	//キャラクターの当たり判定サイズ
	static inline const float kWidth_ = 0.8f;
	static inline const float kHeight_ = 0.8f;
	static inline const float kBlank_ = 0.04f;

	//デスフラグ
	bool isDead_ = false;

	// フレームごとの加速度
	static inline const float kAcceleration = 0.006f;
	// 非入力時の摩擦係数
	static inline const float kAttenuation = 0.13f;
	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;
	// 最高速度
	static inline const float kLimitRunSpeed = 0.19f;
	// 顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間 <秒>
	static inline const float kTimeTurn = 0.45f;
	// 着地フラグ
	bool onGround_ = true;
	// ジャンプ定数
	static inline const float kGravityAcceleration = 0.98f / 65.0f;// 重力加速度(下方向)
	static inline const float kLimitFallSpeed = 0.25f;              // 最大落下速度(下方向)
	static inline const float kJumpAcceleration = 20.0f / 65.0f;   // ジャンプ初速(上方向)

	//微小な数値
	static inline const float kGroundSearchHeight = 0.06f;

	//着地時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;

	//クリアモーション
	Behavior behavior_ = Behavior::kWalk;
	static inline const float kClearTime_ = 0.6f;
	static inline const float kClearMotionAngleStart_ = 0.0f;
	static inline const float kClearMotionAngleEnd_ = -60.0f;
	float counter_ = 0.0f; // カウンター
	bool isClearMotionFinished_ = false;//クリアモーション止める

	// 衝突無効化
	bool isCollisionDisabled_ = false; 

};
