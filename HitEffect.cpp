#include "HitEffect.h"
#include "Math.h"
#include <cassert>
#include <numbers>
#include <random>

using namespace KamataEngine;

//0216 静的メンバ変数の実体
Model* HitEffect::hitEffectModel_ = nullptr;
Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(const Vector3& position,Camera* camera) {
	//インスタンス生成
	HitEffect* instance = new HitEffect();
	//newの失敗を検出
	assert(instance);
	instance->camera_ = camera;
	//インスタンスの初期化 発生座標
	instance->Initialize(position);
	//初期化したインスタンスを返す
	return instance;
}

void HitEffect::Initialize(const Vector3& position) {

	std::random_device seedGenerator;
	std::mt19937_64 randomEngine;
	randomEngine.seed(seedGenerator());
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	// 楕円エフェクト
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.Initialize();
		worldTransform.rotation_ = {0.0f, 0.0f, rotationDistribution(randomEngine)};
		worldTransform.translation_ = position;
		worldTransform.translation_.z = -0.5f;
		//楕円エフェクトのトランスレーションを発生座標で初期化
	}

	// 円形エフェクト
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.translation_.z = -0.5f;
	objectColor_.Initialize();
}

void HitEffect::Update() {

	if (IsDead()) {
		return; // 既に消滅している場合は更新しない
	}

	switch (state_) {
	case State::kSpread: {
		++counter_;
		float scale = 0.5f + static_cast<float>(counter_) / kSpreadTime * 0.5f;
		const float slashScale = 2.0f;
		for (auto& slashWorldTransform : ellipseWorldTransforms_) {
			slashWorldTransform.scale_ = {0.1f, scale * slashScale, 1.0f};
		}

		const float circleScale = 1.0;

		circleWorldTransform_.scale_ = {scale * circleScale, scale * circleScale, 1.0f};

		if (counter_ >= kSpreadTime) {
			state_ = State::kFade;
			counter_ = 0; // カウンターをリセット
		}
		break;
	}
	case State::kFade: {
		++counter_;
		objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f - static_cast<float>(counter_) / kFadeTime});

		if (++counter_ >= kFadeTime) {
			state_ = State::kDead;
		}

		break;
	}
	default:
		break;
	}

	for (auto& slashWorldTransform : ellipseWorldTransforms_) {
		WorldTransformUpdate(slashWorldTransform);
	}
	//円形エフェクト
	WorldTransformUpdate(circleWorldTransform_);
}

void HitEffect::Draw() {
	assert(hitEffectModel_);
	assert(camera_);

	if (IsDead()) {
		return; // 既に消滅している場合は描画しない
	}

	for (auto& slashWorldTransform : ellipseWorldTransforms_) {
		hitEffectModel_->Draw(slashWorldTransform, *camera_, &objectColor_);
	}
	//円形エフェクト
	hitEffectModel_->Draw(circleWorldTransform_, *camera_, &objectColor_);
}