#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込み(2D,3Dどちらも可能)
	textureHandle_ = TextureManager::Load("maguuNormalSpeed.png");
	//スプライトインスタンスの生成(2Dキャラ)
	//sprite_ = Sprite::Create(textureHandle_, {100.0f, 50.0f});
	
	// 3Dモデルの生成
	//player
	player_ = new Player();
	playerModel_ = Model::CreateFromOBJ("needle_Body");
	player_->Initialize(playerModel_,textureHandle_,&camera_);
	// ワールドトランスフォーマーの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.Initialize();

	//要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	//ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	//要素数を変更する


	//ライン描画が参照するカメラを指定する(アドレス渡し)
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
	//デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// サウンドデータの読み込み
	soundDataHandle_ = Audio::GetInstance()->LoadWave("mokugyo.wav");
	// 音声1回だけ再生(SE)
	//voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_); 
	// 音声ループ再生(BGM)
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);

	//軸方向の表示を有効化にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
}

void GameScene::Update() {
	// スプライトの今の座標を取得
	//Vector2 position = sprite_->GetPosition();
	//position.x += 2.0f;
	//position.y += 1.0f;
	// 移動した座標をスプライトに反映
	//sprite_->SetPosition(position);
	

	player_->Update();


#ifdef _DEBUG // デバックビルドのみ見れる

	//ImGuiのウィンドウ作成
	ImGui::Begin("Debug1");
	//デバックテキストの表示
	ImGui::Text("Player");
	//float3入力ボックス
	ImGui::InputFloat3("InputFloat3", inputFloat3);
	//float3スライダー
	ImGui::SliderFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);
	ImGui::End();
	//デモウィンドウの表示を有効化
	ImGui::ShowDemoWindow();

#endif // デバックビルドのみ見れる

	//デバックカメラの更新
	debugCamera_->Update();

	// 音声再生
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 音声停止
		Audio::GetInstance()->StopWave(voiceHandle_);
	}


}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//スプライト描画処理前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// ここにスプライトインスタンスの(2Dキャラ)描画処理を記述する
	//sprite_->Draw();
	
	// スプライト描画後処理
	Sprite::PostDraw();



	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	//model_->Draw(worldTransform_, camera_, textureHandle_);
	player_->Draw();
	

	// ラインを描画する                         始点座標   終点座標     色(RGBA)
	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

	// 3Dモデル描画後処理
	Model::PostDraw();



}

//デストラクタ
GameScene::~GameScene() {
	//delete sprite_;
	delete player_;
	delete debugCamera_;
}