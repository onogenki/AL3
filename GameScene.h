#pragma once
#include "KamataEngine.h"
#include "Player.h"

// ゲームシーン
class GameScene {
public:
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	KamataEngine::Model* model_ = nullptr;

	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	//カメラ
	KamataEngine::Camera camera_;

	//自キャラ
	Player* player_ = nullptr;

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	~GameScene();
};