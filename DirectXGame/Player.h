#pragma once
#include"KamataEngine.h"

using namespace KamataEngine;

//ヘッダ側ではポインタを持つだけなので前方宣言のほうが依存関係薄くなる
class MapChipField;

class Player {
public:

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
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

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// 外部からポイントをセットするためのsetter
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 移動入力
	void InputMove();

	//マップ衝突判定
	void CheckMapCollision(CollisionMapInfo& info);
	//上下左右
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	//geeter
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }//速度

private:
	WorldTransform worldTransform_;

	//マップチップフィールドによるフィールド
	MapChipField* mapChipField_ = nullptr;



	//モデル
	Model* model_ = nullptr;

	Camera* camera_ = nullptr;

	Vector3 velocity_ = {};

	//キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.04f;


	// フレームごとの加速度
	static inline const float kAcceleration = 0.01f;
	// 非入力時の摩擦係数
	static inline const float kAttenuation = 0.13f;
	// 最高速度
	static inline const float kLimitRunSpeed = 0.25f;
	// 顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間 <秒>
	static inline const float kTimeTurn = 0.3f;
	// 着地フラグ
	bool onGround_ = true;
	// ジャンプ定数
	static inline const float kGravityAcceleration = 0.98f / 60.0f;// 重力加速度(下方向)
	static inline const float kLimitFallSpeed = 0.5f;              // 最大落下速度(下方向)
	static inline const float kJumpAcceleration = 20.0f / 60.0f;   // ジャンプ初速(上方向)
};
