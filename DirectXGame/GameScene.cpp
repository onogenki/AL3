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

	///
	/// player
	///
	player_ = new Player(); // 生成
	// 3Dモデルファイルを読み込む(OBJとフォルダ名を一致させること)
	playerModel_ = Model::CreateFromOBJ("needle_Body");
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);
	player_->Initialize(playerModel_, &camera_, playerPosition); // 初期化
	player_->SetMapChipField(mapChipField_);
	// ワールドトランスフォーマーの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.Initialize();

	///
	///enemy
	///
	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		enemyModel_ = Model::CreateFromOBJ("becher");
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(12 + i * 10, 18);
		newEnemy->Initialize(enemyModel_, &camera_, enemyPosition);
		newEnemy->SetMapChipField(mapChipField_);
		enemies_.push_back(newEnemy);
	}

	///
	/// ブロック
	/// 
	// 単純な立方体モデルを自動生成(用意されてて軽量で作れる)
	// blockModel_ = Model::Create();

	blockModel_ = Model::CreateFromOBJ("block");

	// 天球
	skydome_ = new Skydome();
	// trueにすると反転描画になり、内側から見れる(天球用)
	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(skyDomeModel_, &camera_);
	
	// ポーズ画面タイトル
	PauseFont_ = TextureManager::Load("2DPause3D.png");
	SpritePauseFont_ = Sprite::Create(PauseTitle_, {10.0f, 2.0f});

	//ポーズ画面続ける
	PauseResum_ = TextureManager::Load("2DResumPause3D.png");
	SpritePauseResum_ = Sprite::Create(PauseResum_, {10.0f, 20.0f});
	//ポーズ画面リトライ
	PauseRetry_ = TextureManager::Load("2DRetryPause3D.png");
	SpritePauseRetry_ = Sprite::Create(PauseRetry_, {10.0f, 35.0f});
	// ポーズ画面タイトルに戻る
	PauseTitle_ = TextureManager::Load("2DTitlePause3D.png");
	SpritePauseTitle_ = Sprite::Create(PauseTitle_, {10.0f, 50.0f});

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

	phase_ = Phase::kFadeIn;
	// フェードを持ってくる
	fade_ = new Fade();
	fade_->Initialize();
	// 開幕フェード時間はここで決める
	fade_->Start(Fade::Status::FadeIn, 1.0f,Fade::FadeType::Black);
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

void GameScene::Update() {
	// スプライトの今の座標を取得
	// Vector2 position = sprite_->GetPosition();
	// position.x += 2.0f;
	// position.y += 1.0f;
	// 移動した座標をスプライトに反映
	// sprite_->SetPosition(position);


	switch (phase_) {

		// フェードイン
	case Phase::kFadeIn:
		player_->UpdateTransformOnly();
		for (Enemy* enemy : enemies_) {
			enemy->UpdateTransformOnly();
		}
		skydome_->Update();

		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
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

		//死亡デバック
		if (Input::GetInstance()->TriggerKey(DIK_1)) {
			exitRequest_ = ExitRequest::Death;
			fade_->Start(Fade::Status::FadeOut, 1.0f, Fade::FadeType::White);
			phase_ = Phase::kFadeOut;
			break;
		}

		if (!isPause_) {
			skydome_->Update();
			player_->Update();
			for (Enemy* enemy : enemies_) 
			{
				enemy->Update();
			}
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

			//ポーズ画面へ
			if (Input::GetInstance()->PushKey(DIK_TAB))
			{
				isPause_ = true;
			}

		} else // ポーズ画面
		{

			// 再開画面
			if (currentPauseState_ == PauseState::Resume) {
				if (Input::GetInstance()->TriggerKey(DIK_UP)) {
					currentPauseState_ = PauseState::Title;
				} else if (Input::GetInstance()->TriggerKey(DIK_DOWN)) {
					currentPauseState_ = PauseState::Retry;
				}
				if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
					isPause_ = false;
				}

				// リトライ画面
			} else if (currentPauseState_ == PauseState::Retry) {
				if (Input::GetInstance()->TriggerKey(DIK_UP)) {
					currentPauseState_ = PauseState::Resume;
				} else if (Input::GetInstance()->TriggerKey(DIK_DOWN)) {
					currentPauseState_ = PauseState::Title;
				}
				if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
					exitRequest_ = ExitRequest::Retry;
					fade_->Start(Fade::Status::FadeOut, 0.5f, Fade::FadeType::Black);
					phase_ = Phase::kFadeOut;
				}

				//タイトルに戻る画面
			} else if (currentPauseState_ == PauseState::Title) {
				if (Input::GetInstance()->TriggerKey(DIK_UP)) {
					currentPauseState_ = PauseState::Retry;
				} else if (Input::GetInstance()->TriggerKey(DIK_DOWN)) {
					currentPauseState_ = PauseState::Resume;
				}
				if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
					// タイトルに戻る
					exitRequest_ = ExitRequest::Title;
					fade_->Start(Fade::Status::FadeOut, 1.0f, Fade::FadeType::White);
					phase_ = Phase::kFadeOut;
				}
			}
		}

		break;

	// 死んだとき
	case Phase::kDeath:
		//skydome_->Update();

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
		break;
	}
}
void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	switch (phase_) {
		// フェードイン描画
	case GameScene::Phase::kFadeIn:

		// 3Dモデル描画前処理
		Model::PreDraw(dxCommon->GetCommandList());

		player_->Draw();
		for (Enemy* enemy : enemies_) 
		{
			enemy->Draw();
		}
		skydome_->Draw();

		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				blockModel_->Draw(*worldTransformBlock, camera_);
			}
		}
		// 3Dモデル描画後処理
		Model::PostDraw();

		// スプライト描画処理前処理
		Sprite::PreDraw(dxCommon->GetCommandList());

		fade_->Draw();

		Sprite::PostDraw();

		break;

	// プレイ画面
	case GameScene::Phase::kPlay:

		// 3Dモデル描画前処理
		Model::PreDraw(dxCommon->GetCommandList());

		skydome_->Draw();
		player_->Draw();
		for (Enemy* enemy : enemies_) 
		{
			enemy->Draw();
		}

		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				blockModel_->Draw(*worldTransformBlock, camera_);
			}
		}

		// ラインを描画する

		drawer_ = PrimitiveDrawer::GetInstance();

		// 1マスのサイズ(マップチップ1ブロック分)
		gridSize_ = 1.0f;

		numRows_ = mapChipField_->GetNumBlockVirtical();   // 縦方向
		numCols_ = mapChipField_->GetNumBlockHorizontal(); // 横方向

		// 横線
		for (int row = 0; row <= numRows_; ++row) {
			float y = 0.5f + row * gridSize_;
			drawer_->DrawLine3d(
			    {0.0f, y, 0.0f}, {0.0f + numCols_ * gridSize_, y, 0.0f}, {0.6f, 0.6f, 0.6f, 1.0f} // グレー
			);
		}

		// 縦線
		for (int col = 0; col <= numCols_; ++col) {
			float x = 0.5f + col * gridSize_;
			drawer_->DrawLine3d({x, 0.0f, 0.0f}, {x, 0.0f + numRows_ * gridSize_, 0.0f}, {0.6f, 0.6f, 0.6f, 1.0f});
		}

		// 3Dモデル描画後処理
		Model::PostDraw();

		// スプライト描画処理前処理
		Sprite::PreDraw(dxCommon->GetCommandList());

		// ここにスプライトインスタンスの(2Dキャラ)描画処理を記述する
		// sprite_->Draw();

		fade_->Draw();
		// ポーズ画面
		if (isPause_) {
			if (currentPauseState_ == PauseState::Resume) {
				SpritePauseFont_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseFont_->Draw();
				SpritePauseResum_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseResum_->Draw();
				SpritePauseRetry_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseRetry_->Draw();
				SpritePauseTitle_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseTitle_->Draw();
			} else if (currentPauseState_ == PauseState::Retry) {
				SpritePauseFont_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseFont_->Draw();
				SpritePauseResum_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseResum_->Draw();
				SpritePauseRetry_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseRetry_->Draw();
				SpritePauseTitle_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseTitle_->Draw();
			} else if (currentPauseState_ == PauseState::Title) {
				SpritePauseFont_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseFont_->Draw();
				SpritePauseResum_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseResum_->Draw();
				SpritePauseRetry_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseRetry_->Draw();
				SpritePauseTitle_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseTitle_->Draw();
			}
		}

		// スプライト描画後処理
		Sprite::PostDraw();

		break;

	case GameScene::Phase::kDeath:

		// 3Dモデル描画前処理
		Model::PreDraw(dxCommon->GetCommandList());

		skydome_->Draw();

		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				blockModel_->Draw(*worldTransformBlock, camera_);
			}
		}

		// 3Dモデル描画後処理
		Model::PostDraw();

		// スプライト描画処理前処理
		Sprite::PreDraw(dxCommon->GetCommandList());

		fade_->Draw();

		Sprite::PostDraw();

		break;

		//フェードアウト
	case GameScene::Phase::kFadeOut:

		// 3Dモデル描画前処理
		Model::PreDraw(dxCommon->GetCommandList());

		skydome_->Draw();
		//player_->Draw();

		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				blockModel_->Draw(*worldTransformBlock, camera_);
			}
		}

		// 3Dモデル描画後処理
		Model::PostDraw();

		// スプライト描画処理前処理
		Sprite::PreDraw(dxCommon->GetCommandList());

		fade_->Draw();

		Sprite::PostDraw();

		break;
	}
}

// デストラクタ
GameScene::~GameScene() {
	// delete sprite_;
	delete player_;
	for (Enemy* enemy : enemies_)
	{
		delete enemy;
	}
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
	delete SpritePauseFont_;
	delete SpritePauseResum_;
	delete SpritePauseRetry_;
	delete SpritePauseTitle_;
}