#pragma once
#include "KamataEngine.h"
#include<vector>

// ゲームシーン
class GameScene {

private:
	//3Dモデル
	/*KamataEngine::Model* blockModel_ = nullptr;*/

	//テクスチャハンドル
	/*uint32_t textureHandle_ = 0;*/

	/*KamataEngine::Model* model_ = nullptr;*/

	//ワールドトランスフォーム
	/*KamataEngine::WorldTransform worldTransform_;*/
	//カメラ
	KamataEngine::Camera camera_;

	//ブロックモデル
	KamataEngine::Model* block_model_ = nullptr;
	std::vector<KamataEngine::WorldTransform*> worldTransformBlocks_;

public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	~GameScene();
};