#include "Skydome.h"
using namespace KamataEngine;

void Skydome::Initialize(Model* modelSkydome_, Camera* camera) {
	model_ = modelSkydome_;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Skydome::Update() {
	worldTransform_.TransferMatrix();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
}

void Skydome::Draw() {
	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}