#pragma once
#include "KamataEngine.h"
#include<vector>
#include "Skydome.h"
#include"Player.h"
#include "MapChipField.h"
#include "math.h"


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
	void GetNumBlockHorizontal();

private:
	//3Dモデル
	KamataEngine::Model* blockModel_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* sprite_ = nullptr;

	KamataEngine::Model* model_ = nullptr;

	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	//カメラ
	KamataEngine::Camera camera_;

	//ブロックモデル
	KamataEngine::Model* block_model_ = nullptr;
	//もう一度std::vectorを重ねる
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	//デバックカメラ有効
	bool isDebugCameraActive_ = false;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	//0203天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	Player* player_ = nullptr;

	MapChipField* mapChipField_;

	math* math_ = nullptr;
};