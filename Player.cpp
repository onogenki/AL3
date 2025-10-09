#include "Player.h"
#include<cassert>

void Player::Initialize(Model* playerModel, uint32_t textureHandle, Camera* camera) { 
	
	assert(playerModel);
	//モデル
	model_ = playerModel;
	//テクスチャハンドル
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();

	camera_ = camera;
}

void Player::Update() 
{ 
	worldTransform_.TransferMatrix(); 
}

void Player::Draw()
{
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}