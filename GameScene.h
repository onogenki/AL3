#pragma once
#include "KamataEngine.h"

// ゲームシーン
class GameScene {
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//ファイル名を指定してテクスチャを読み込み
	textureHandle_ = TextureManager::Load("sample.png");

	KamataEngine::Sprite* sprite_ = nullptr;

public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();
};