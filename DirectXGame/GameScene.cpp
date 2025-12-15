#include "GameScene.h"
#include "Math.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込み(2D,3Dどちらも可能)
	// textureHandle_ = TextureManager::Load("maguuNormalSpeed.png");
	// スプライトインスタンスの生成(2Dキャラ)
	// sprite_ = Sprite::Create(textureHandle_, {100.0f, 50.0f});

	// まずマップを読み込む
	//  マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GeneratedBlocks();

	// 3Dモデルの生成
	// player
	player_ = new Player(); // 生成
	// 3Dモデルファイルを読み込む(OBJとフォルダ名を一致させること)
	playerModel_ = Model::CreateFromOBJ("needle_Body");
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);
	player_->Initialize(playerModel_, &camera_, playerPosition); // 初期化
	// ワールドトランスフォーマーの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.Initialize();

	player_->SetMapChipField(mapChipField_);

	// ブロック
	// 単純な立方体モデルを自動生成(用意されてて軽量で作れる)
	// blockModel_ = Model::Create();

	blockModel_ = Model::CreateFromOBJ("block");

	// 天球
	skydome_ = new Skydome();
	// trueにすると反転描画になり、内側から見れる(天球用)
	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(skyDomeModel_, &camera_);

	// ライン描画が参照するカメラを指定する(アドレス渡し)
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// サウンドデータの読み込み
	soundDataHandle_ = Audio::GetInstance()->LoadWave("mokugyo.wav");
	// 音声1回だけ再生(SE)
	// voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_);
	// 音声ループ再生(BGM)再生止めるもの
	// voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);

	// 軸方向の表示を有効化にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	if (isDebugCameraActive_) {

		AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
	} else {
		AxisIndicator::GetInstance()->SetTargetCamera(&camera_);
	}

	// カメラコントローラ
	CController_ = new CameraController(); // 生成
	CController_->Initialize(&camera_);    // 初期化
	CController_->SetTarget(player_);      // 追尾対象セット
	CController_->Reset();                 // リセット

	// フェードを持ってくる
	fade_ = new Fade();
	fade_->Initialize();
	// フェード時間はここで決める
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void GameScene::GeneratedBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();

				// マップチップの左下座標
				// Vector3 pos = mapChipField_->GetMapChipPositionByIndex(j, i);

				// block.obj は中心が原点なので 0.5f ずらす
				// pos.x += 0.5f;
				// pos.y += 0.5f;
				//
				// worldTransform->translation_ = pos;
				// worldTransformBlocks_[i][j] = worldTransform;

				// ずらしてないときの処理はこっち
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {

	// フェードイン
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;

	case Phase::kPlay:
		if (Input::GetInstance()->PushKey(DIK_1)) {
			fade_->Start(Fade::Status::FadeOut, 2.0f);
			phase_ = Phase::kDeath;
		}
		break;

	case Phase::kDeath:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true; // シーン終了
		}
		break;
	}
}

void GameScene::Update() {
	// スプライトの今の座標を取得
	// Vector2 position = sprite_->GetPosition();
	// position.x += 2.0f;
	// position.y += 1.0f;
	// 移動した座標をスプライトに反映
	// sprite_->SetPosition(position);

	fade_->Update();
	ChangePhase();

	switch (phase_) {

		// フェードイン
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kPlay;
		}

		skydome_->Update();
		player_->Update();
		CController_->Update();

		// デバックカメラの更新
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			camera_.UpdateMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換～DirectXに転送
				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		break;

	// プレイ
	case Phase::kPlay:
		skydome_->Update();
		player_->Update();
		CController_->Update();

#ifdef _DEBUG // デバックビルドのみ見れる

		// ImGuiのウィンドウ作成
		ImGui::Begin("Debug1");
		// デバックテキストの表示
		ImGui::Text("Player");
		// float3入力ボックス
		ImGui::InputFloat3("InputFloat3", inputFloat3);
		// float3スライダー
		ImGui::SliderFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);
		Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(15, 13);
		ImGui::End();
		// デモウィンドウの表示を有効化
		ImGui::ShowDemoWindow();

		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}

#endif // デバックビルドのみ見れる

		// デバックカメラの更新
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			camera_.UpdateMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換～DirectXに転送
				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		// 敵player当たり判定

		// 音声再生
		if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
			// 音声再生
			voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, false);
			// 音声停止
			// Audio::GetInstance()->StopWave(voiceHandle_);
		}

		break;

	// 死んだとき
	case Phase::kDeath:
		skydome_->Update();

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換～DirectXに転送
				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		break;

	// フェードアウト
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		skydome_->Update();

		break;
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	skydome_->Draw();
	player_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}

	// ラインを描画する

	PrimitiveDrawer* drawer = PrimitiveDrawer::GetInstance();

	// 1マスのサイズ(マップチップ1ブロック分)
	float gridSize = 1.0f;

	int numRows = mapChipField_->GetNumBlockVirtical();   // 縦方向
	int numCols = mapChipField_->GetNumBlockHorizontal(); // 横方向

	// 横線
	for (int row = 0; row <= numRows; ++row) {
		float y = 0.5f + row * gridSize;
		drawer->DrawLine3d(
		    {0.0f, y, 0.0f}, {0.0f + numCols * gridSize, y, 0.0f}, {0.6f, 0.6f, 0.6f, 1.0f} // グレー
		);
	}

	// 縦線
	for (int col = 0; col <= numCols; ++col) {
		float x = 0.5f + col * gridSize;
		drawer->DrawLine3d({x, 0.0f, 0.0f}, {x, 0.0f + numRows * gridSize, 0.0f}, {0.6f, 0.6f, 0.6f, 1.0f});
	}

	// 3Dモデル描画後処理
	Model::PostDraw();

	// スプライト描画処理前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// ここにスプライトインスタンスの(2Dキャラ)描画処理を記述する
	// sprite_->Draw();

	fade_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
}

// デストラクタ
GameScene::~GameScene() {
	// delete sprite_;
	delete player_;
	delete debugCamera_;
	delete blockModel_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete skyDomeModel_;
	delete mapChipField_;
}