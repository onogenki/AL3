#pragma once
#include "KamataEngine.h"
#include "math.h"

using namespace KamataEngine;

class MapChipField;
//includeの変わり、enemyのクラスのポインタ
class Enemy;

class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	//0207角
	enum Corner { 
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
		kNumCorner
	};

	//0214 振るまい
	enum class Behavior {
		kUnknown = -1,
		kRoot,   // 通常状態
		kAttack, // 攻撃中
	};

	//0214 攻撃フェーズ
	enum class AttackPhase {
		kUnknown = -1, // 無効な状態

		kAnticipation, // 予備動作
		kAction,       // 前進動作
		kRecovery,     // 余韻動作
	};

	/// 初期化
	void Initialize(Model* player_model, Model* modelAttack, Camera* camera, const Vector3& position);

	/// 更新
	void Update();

	/// 描画
	void Draw();

	//0206 getter
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }

	//0207
	void SetMapChipField(MapChipField* mapChipField) {
		mapChipField_ = mapChipField; }
	
	//0210ワールド座標を取得 constメンバ関数とする
	Vector3 GetWorldPosition() const;
	
	AABB GetAABB();

	//衝突応答
	void OnCollision(const Enemy* enemy);

	//0212 デスフラグのgetter
	bool IsDead() const { return isDead_; }

	//0214 通常行動更新
	void BehaviorRootUpdate();

	//0214 攻撃行動更新
	void BehaviorAttackUpdate();

	//0214 通常行動初期化
	void BehaviorRootInitialize();

	//0214 攻撃行動初期化
	void BehaviorAttackInitialize();

	//0215
	bool IsAttack() const { return behavior_ == Behavior::kAttack && attackPhase_ == AttackPhase::kAction; }

	//0215
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	void Reset();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	Camera* camera_ = nullptr;
	// 0205 移動量
	Vector3 velocity_ = {};
	//フレームごとの加速度
	static inline const float kAcceleration = 0.1f;
	//非入力時の摩擦係数
	static inline const float kAttenuation = 0.05f;
	//最高速度
	static inline const float kLimitRunSpeed = 0.3f;
	//顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//旋回時間 <秒>
	static inline const float kTimeTurn = 0.3f;
	//ジャンプ定数
	static inline const float kJumpAcceleration = 20.0f;     //重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.98f;  //最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.5f;        //ジャンプ初速(上方向)

	//0207マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	//0207キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	//壁抜け防ぎ
	static inline const float kNone = 0.04f;

	//0207移動入力
	void InputMove();

	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};
	//0207
	void CheckMapCollision(CollisionMapInfo& info);
	
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	//着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;
	//微小な数値
	static inline const float kGroundSearchHeight = 0.06f;
	//着地時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;
	//0212 デスフラグ
	bool isDead_ = false;

	//0214 振るまい
	Behavior behavior_ = Behavior::kRoot;

	//次の振るまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	//攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_ = 0;

	//攻撃フェーズ
	AttackPhase attackPhase_ = AttackPhase::kUnknown;

	//予備動作の時間
	static inline const uint32_t kAnticipationTime = 8;
	//前進動作の時間
	static inline const uint32_t kActionTime = 5;
	//余韻動作の時間
	static inline const uint32_t kRecoveryTime = 12;

	// 02_14 34枚目 攻撃エフェクト
	Model* modelAttack_ = nullptr;
	WorldTransform worldTransformAttack_;

	// 02_15 20枚目
	bool isCollisionDisabled_ = false; // 衝突無効化

	// 復活
	Vector3 respawnPosition_;             // 復活する位置
	float respawnTimer_ = 0.0f;           // 復活まで測る時間
	const float respawnCountTime_ = 2.0f; // 復活までのカウントダウン

};