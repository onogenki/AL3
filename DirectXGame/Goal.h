#pragma once
#include"KamataEngine.h"
#include "math.h"

using namespace KamataEngine;

class GameScene;
class MapChipField;
class Player;

class Goal 
{
public:
	enum class State
	{
		kClosed, // 閉まってる状態
		kCanOpen,// 開ける状態(space押せる)
		kOpening,// 開く状態(space押されて)
		kOpen    // 開いた状態(クリア)
	};

	void Initialize(Model* goalBModel,Model* goalLOModel,Model* goalROModel, Camera* camera, const Vector3& position);

	void Update();

	void Draw();

	// 外部からポイントをセットするためのsetter
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// ワールド座標を取得(当たり判定をとるため)
	Vector3 GetWorldPosition() const;

	AABB GetAABB();

	// 衝突判定
	void OnCollision(const Player* player); 

	bool IsOpen() const { return state_ == State::kOpen; }

	bool IsOpening() const { return state_ == State::kOpening; }

	bool CanOpen() const { return state_ == State::kCanOpen; }

	//getter
	State GetState() const { return state_; }
	//音のハンドルをセットする関数
	void SetSeHandle(uint32_t handle) { seClearHandle_ = handle; }

	private:
	WorldTransform worldTransformB_; //黒いドア
	WorldTransform worldTransformLO_;//Left開くドア
	WorldTransform worldTransformRO_;//Right開くドア

	MapChipField* mapChipField_ = nullptr;

	Model* modelB_ = nullptr;//黒いドア
	Model* modelLO_ = nullptr;//Left開くドア
	Model* modelRO_ = nullptr;//Right開くドア

	Camera* camera_ = nullptr;

	Vector3 velocity_ = {};

	// 当たり判定サイズ
	static inline const float kWidth_ = 0.8f;
	static inline const float kHeight_ = 0.8f;

	//最初閉まってる状態
	State state_ = State::kClosed;
	float openTimer_ = 0.0f;

	//クリアse
	uint32_t seClearHandle_ = 0;
};
