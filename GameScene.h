#pragma once
#include "CameraController.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "skydome.h"
#include <vector>

using namespace KamataEngine;

// ゲームシーン
class GameScene {

public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	~GameScene();

	void GenerateBlocks();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// スプライト
	Sprite* sprite_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

	// ブロックモデル
	Model* blockModel_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// カメラ
	Camera camera_;

	// プレイヤー
	Player* player_ = nullptr;
	Model* player_model_ = nullptr;

	// ブロックモデル
	Model* block_model_ = nullptr;
	// もう一度std::vectorを重ねる
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// デバックカメラ有効
	bool isDebugCameraActive_ = false;

	DebugCamera* debugCamera_ = nullptr;

	// 0203天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// カメラ移動
	CameraController* CController_ = nullptr;
};