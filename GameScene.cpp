#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {}

void GameScene::Update() {}
#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込み
	textureHandle_ = TextureManager::Load("sample.png");

	// スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {120, 50});

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//カメラの初期化
	camera_.Initialize();
	

}

void GameScene::Update() {}

void GameScene::Draw() {}

GameScene::~GameScene()
{ delete sprite_; }