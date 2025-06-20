#include "Skydome.h"
using namespace KamataEngine;

void Skydome::Initialize(Model* modelSkydome_, Camera* camera) {

	assert(modelSkydome_);

	model_ = modelSkydome_;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Skydome::Update() {
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}