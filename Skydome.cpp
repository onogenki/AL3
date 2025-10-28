#include "Skydome.h"
using namespace KamataEngine;

void Skydome::Initialize(Model* skydomeModel_,Camera* camera)
{ 
	model_ = skydomeModel_;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Skydome::Update() {
	worldTransform_.TransferMatrix();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
}

void Skydome::Draw() {}
//model_->Draw(worldTransform_, *camera_);}