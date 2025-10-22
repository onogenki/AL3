#include "Player.h"
#include "Math.h"
#include <algorithm>
#include <cassert>
#include <numbers>
using namespace KamataEngine;

void Player::Initialize(Model* playerModel, uint32_t textureHandle, Camera* camera) { 
	
	assert(playerModel);
	//モデル
	model_ = playerModel;
	//テクスチャハンドル
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();

	//引数の内容をメンバ変数に記録
	camera_ = camera;
}

void Player::Update() 
{ 
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix(); 

}

void Player::Draw()
{
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}