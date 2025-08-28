#include "Math.h"
#include "RuleScene.h"
#include <numbers>

void RuleScene::Initialize() {

	modelRule_ = Model::CreateFromOBJ("ruleFont", true);
	modelRule1_ = Model::CreateFromOBJ("ruleFont1", true);
	modelRule2_ = Model::CreateFromOBJ("ruleFont2", true);
	modelRule3_ = Model::CreateFromOBJ("ruleFont3", true);
	modelRule4_ = Model::CreateFromOBJ("ruleFont4", true);

	modelSpace_ = Model::CreateFromOBJ("spaceFont", true);

	// カメラ初期化
	camera_.Initialize();

	const float kPlayerTitle = 2.0f;

	const float kRuleTitle = 1.0f;

	// ruleFont
	worldTransformRule_.Initialize();

	worldTransformRule_.translation_.y = +17.0f;

	worldTransformRule_.scale_ = {kRuleTitle, kRuleTitle, kRuleTitle};

	// ruleFont1
	worldTransformRule1_.Initialize();

	worldTransformRule1_.translation_.y = +13.0f;

	worldTransformRule1_.scale_ = {kRuleTitle, kRuleTitle, kRuleTitle};

	// ruleFont2
	worldTransformRule2_.Initialize();

	worldTransformRule2_.translation_.y = +7.0f;

	worldTransformRule2_.scale_ = {kRuleTitle, kRuleTitle, kRuleTitle};

	// ruleFont3
	worldTransformRule3_.Initialize();

	worldTransformRule3_.scale_ = {kRuleTitle, kRuleTitle, kRuleTitle};

	// ruleFont4
	worldTransformRule4_.Initialize();

	worldTransformRule4_.translation_.y = -7.0f;

	worldTransformRule4_.scale_ = {kRuleTitle, kRuleTitle, kRuleTitle};

	// spaceFont
	worldTransformSpace_.Initialize();

	worldTransformSpace_.translation_.x = -2.0f;


	worldTransformSpace_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};

	// 0203天球
	// skydome生成
	skydome_ = new Skydome();
	// 初期化
	modelSkydome_ = Model::CreateFromOBJ("sky_sphere", true);
	skydome_->Initialize(modelSkydome_, &camera_);

	// 0213
	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 2.0f);
}

void RuleScene::Update() {

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		// タイトルシーンの終了条件
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimeSpaceMove);

	float angle = counter_ / kTimeSpaceMove * 2.0f * std::numbers::pi_v<float>;

	worldTransformSpace_.translation_.y = -18.0f + std::sin(angle);

	camera_.TransferMatrix();

	// アフィン変換～DirectXに転送
	WorldTransformUpdate(worldTransformRule_);
	WorldTransformUpdate(worldTransformRule1_);
	WorldTransformUpdate(worldTransformRule2_);
	WorldTransformUpdate(worldTransformRule3_);
	WorldTransformUpdate(worldTransformRule4_);

	WorldTransformUpdate(worldTransformSpace_);
}

void RuleScene::Draw() {

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	// 天球描画
	skydome_->Draw();

	modelRule_->Draw(worldTransformRule_, camera_);
	modelRule1_->Draw(worldTransformRule1_, camera_);
	modelRule2_->Draw(worldTransformRule2_, camera_);
	modelRule3_->Draw(worldTransformRule3_, camera_);
	modelRule4_->Draw(worldTransformRule4_, camera_);

	modelSpace_->Draw(worldTransformSpace_, camera_);

	Model::PostDraw();

	// 0213
	fade_->Draw();
}

RuleScene::~RuleScene() {
	delete modelRule_;
	delete modelRule1_;
	delete modelRule2_;
	delete modelRule3_;
	delete modelRule4_;
	delete modelSpace_;
	delete modelSkydome_;
	// 0213
	delete fade_;
}