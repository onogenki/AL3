#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() 
{ 
	textureHandle_ = TextureManager::Load("mario.jpg");

	sprite_ = Sprite::Create(textureHandle_, {100, 50});
}

void GameScene::Update() {
	//スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	position.x += 2.0f;
	position.y += 1.0f;
	//移動した座標をスプライトに反映
	sprite_->SetPosition(position);
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());

	sprite_->Draw();

	Sprite::PostDraw();
}

GameScene::~GameScene() 
{ 
	
	delete sprite_; 
}