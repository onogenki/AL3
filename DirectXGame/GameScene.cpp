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
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(12 + i * 2, 18);
		newEnemy->Initialize(enemyModel_, &camera_, enemyPosition);
		newEnemy->SetMapChipField(mapChipField_);
		enemies_.push_back(newEnemy);
	}

	///
	///ゴールドア
	///
	goal_ = new Goal();
	BlackGoalModel_ = Model::CreateFromOBJ("BlackGoal");//黒いドア
	LeftOpenGoalModel_ = Model::CreateFromOBJ("LeftGoal");//左のドア
	RightOpenGoalModel_ = Model::CreateFromOBJ("RightGoal");//右のドア

	IsGoalSpace_ = false;
	Vector3 GoalPosition = mapChipField_->GetMapChipPositionByIndex(35, 18);
	goal_->Initialize(BlackGoalModel_, LeftOpenGoalModel_, RightOpenGoalModel_, &camera_, GoalPosition);
	goal_->SetMapChipField(mapChipField_);



	// デスパーティクル
	deathParticlesModel_ = Model::CreateFromOBJ("deathParticle");
	

	///
	/// ブロック
	/// 
	// 単純な立方体モデルを自動生成(用意されてて軽量で作れる)
	// blockModel_ = Model::Create();

	blockModel_ = Model::CreateFromOBJ("block");

	// 天球
	skydome_ = new Skydome();
	// trueにすると反転描画になり、内側から見れる(天球用)
	skyDomeModel_ = Model::CreateFromOBJ("sky_under", true);
	skydome_->Initialize(skyDomeModel_, &camera_);

	// TABキーでポーズ
	TABFont_ = TextureManager::Load("2DTABPause3D.png");
	SpriteTABFont_ = Sprite::Create(TABFont_, {900.0f, -150.0f});
	
	// ポーズ画面タイトル
	PauseFont_ = TextureManager::Load("2DPause3D.png");
	SpritePauseFont_ = Sprite::Create(PauseFont_, {450.0f, -100.0f});

	//ポーズ画面続ける
	PauseResume_ = TextureManager::Load("2DResumPause3D.png");
	SpritePauseResume_ = Sprite::Create(PauseResume_, {450.0f, 50.0f});
	//ポーズ画面リトライ
	PauseRetry_ = TextureManager::Load("2DRetryPause3D.png");
	SpritePauseRetry_ = Sprite::Create(PauseRetry_, {450.0f, 150.0f});
	// ポーズ画面タイトルに戻る
	PauseTitle_ = TextureManager::Load("2DTitlePause3D.png");
	SpritePauseTitle_ = Sprite::Create(PauseTitle_, {450.0f, 280.0f});

	// ポーズ画面タイトルに戻る
	PauseEnter_ = TextureManager::Load("2DPauseEnter3D.png");
	SpritePauseEnter_ = Sprite::Create(PauseEnter_, {950.0f, 250.0f});

	// spaceフォント
	textureHandleSpace_ = TextureManager::Load("space.png");
	spriteSpace_ = Sprite::Create(textureHandleSpace_, {300.0f, 100.0f});
	// spaceフォント
	textureHandleClear_ = TextureManager::Load("Game Clear.png");
	spriteClear_ = Sprite::Create(textureHandleClear_, {300.0f, 0.0f});

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
	fade_->Start(Fade::Status::FadeIn, 0.7f,Fade::FadeType::Black);
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

//当たり判定(クリア、死ぬ判定)
void GameScene::CheckAllCollision() {

	if (phase_ != Phase::kPlay) {
		return;
	}

	AABB playerAABB = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		AABB enemyAABB = enemy->GetAABB();
		// コリジョン無効の敵はスキップ
		if (enemy->IsCollisionDisabled())
			continue;
		// 当たってるか
		if (!IsCollision(playerAABB, enemyAABB)) {
			continue;
		}

		// 高さ判定
		playerFootY = playerAABB.min.y;
		enemyHeadY = enemyAABB.max.y;

		// 落下中か
		isFalling = player_->GetVelocity().y < 0.0f;

		///
		/// player,enemyの当たった時
		///
		if (isFalling && playerFootY > enemyHeadY - player_->GetkBlank()) {
			player_->Bounce();           // 跳ねる
			enemy->OnCollision(player_); // 敵が倒れる

			DeathParticles* deathParticles = new DeathParticles();
			deathParticles->Initialize(deathParticlesModel_, &camera_, enemy->GetWorldPosition());
			deathParticles->Spawn(enemy->GetWorldPosition());

			deathParticlesList_.push_back(deathParticles);
			break;

		} else { // player倒れる
			{
				if (!player_->IsDead()) {
					player_->OnCollision(enemy); // playerが倒れる
					if (!deathParticle_) {
						deathParticle_ = new DeathParticles;
						const Vector3& deathParticlesPosition = player_->GetWorldPosition();
						deathParticle_->Initialize(deathParticlesModel_, &camera_, deathParticlesPosition);
						deathParticle_->Spawn(deathParticlesPosition);
					}

					exitRequest_ = ExitRequest::Death;
					fade_->Start(Fade::Status::FadeOut, 0.5f, Fade::FadeType::White);
					phase_ = Phase::kDeath;
				}
			}
		}
	}
	///
	/// ゴール当たり判定
	///
	IsGoalSpace_ = false;
	AABB goalAABB = goal_->GetAABB();
	IsGoalSpace_ = IsCollision(playerAABB, goalAABB);
	if (IsCollision(playerAABB, goalAABB)) {
		goal_->OnCollision(player_);
	}//クリアモーション
	if (goal_->IsOpening()) {
		player_->SetBehavior(Player::Behavior::kClear);
	}
	if (goal_->IsOpen()) {
		phase_ = Phase::kClear; // ドアが開いたらクリアシーンに
	}
}

void GameScene::Update() {
	// スプライトの今の座標を取得
	//Vector2 position = sprite_->GetPosition();
	// position.x += 2.0f;
	// position.y += 1.0f;
	// 移動した座標をスプライトに反映
	// sprite_->SetPosition(position);

	// 条件に合う要素だけ消す
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
	deathParticlesList_.remove_if([](DeathParticles* deathParticles) {
		if (deathParticles->IsFinished()) {
			delete deathParticles;
			return true; // リストから消す(デスパーティクルの時間まで)
		}
		return false;
	});

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

		if (!isPause_) {
			skydome_->Update();
			player_->Update();
			for (Enemy* enemy : enemies_) 
			{
				enemy->Update();
			}
			for (DeathParticles* deathParticles : deathParticlesList_) {
				deathParticles->Update();
			}
			goal_->Update();
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

			if (Input::GetInstance()->TriggerKey(DIK_P)) {
				isDebugCameraActive_ = !isDebugCameraActive_;
			}

			// 死亡デバック
			if (Input::GetInstance()->PushKey(DIK_1)) {
				if (!player_->IsDead()) {
					player_->OnCollision(nullptr); // playerが倒れる
					if (!deathParticle_) {
						deathParticle_ = new DeathParticles;
						const Vector3& deathParticlesPosition = player_->GetWorldPosition();
						deathParticle_->Initialize(deathParticlesModel_, &camera_, deathParticlesPosition);
						deathParticle_->Spawn(deathParticlesPosition);
					}
				}
				exitRequest_ = ExitRequest::Death;
				fade_->Start(Fade::Status::FadeOut, 1.0f, Fade::FadeType::White);
				phase_ = Phase::kDeath;
				break;
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
			if (Input::GetInstance()->TriggerKey(DIK_TAB))
			{
				isPause_ = true;
			}
			CheckAllCollision(); // 全ての当たり判定を行う
		} else // ポーズ画面
		{

			// 再開画面
			if (currentPauseState_ == PauseState::Resume) {
				if (Input::GetInstance()->TriggerKey(DIK_UP) ||
					Input::GetInstance()->TriggerKey(DIK_W)) {
					currentPauseState_ = PauseState::Title;
				} else if (Input::GetInstance()->TriggerKey(DIK_DOWN)|| 
					Input::GetInstance()->TriggerKey(DIK_S)) {
					currentPauseState_ = PauseState::Retry;
				}
				if (Input::GetInstance()->TriggerKey(DIK_RETURN)|| 
					Input::GetInstance()->TriggerKey(DIK_TAB)||
				Input::GetInstance()->TriggerKey(DIK_SPACE)){
					isPause_ = false;
				}

				// リトライ画面
			} else if (currentPauseState_ == PauseState::Retry) {
				if (Input::GetInstance()->TriggerKey(DIK_UP) || 
					Input::GetInstance()->TriggerKey(DIK_W)) {
					currentPauseState_ = PauseState::Resume;
				} else if (Input::GetInstance()->TriggerKey(DIK_DOWN)|| 
					Input::GetInstance()->TriggerKey(DIK_S)) {
					currentPauseState_ = PauseState::Title;
				}
				if (Input::GetInstance()->TriggerKey(DIK_RETURN)|| 
					Input::GetInstance()->TriggerKey(DIK_SPACE)) {
					exitRequest_ = ExitRequest::Retry;
					fade_->Start(Fade::Status::FadeOut, 0.5f, Fade::FadeType::Black);
					phase_ = Phase::kFadeOut;
				}
				if (Input::GetInstance()->TriggerKey(DIK_TAB)) {
					isPause_ = false;
					currentPauseState_ = PauseState::Resume;
				}
				//タイトルに戻る画面
			} else if (currentPauseState_ == PauseState::Title) {
				if (Input::GetInstance()->TriggerKey(DIK_UP) || 
					Input::GetInstance()->TriggerKey(DIK_W)) {
					currentPauseState_ = PauseState::Retry;
				} else if (Input::GetInstance()->TriggerKey(DIK_DOWN) ||
					Input::GetInstance()->TriggerKey(DIK_S)) {
					currentPauseState_ = PauseState::Resume;
				}
				if (Input::GetInstance()->TriggerKey(DIK_RETURN)||
					Input::GetInstance()->TriggerKey(DIK_SPACE)) {
					// タイトルに戻る
					exitRequest_ = ExitRequest::Title;
					fade_->Start(Fade::Status::FadeOut, 1.0f, Fade::FadeType::White);
					phase_ = Phase::kFadeOut;
				}
				if (Input::GetInstance()->TriggerKey(DIK_TAB)) {
					isPause_ = false;
					currentPauseState_ = PauseState::Resume;
				}
			}
		}

		break;

	// 死んだとき
	case Phase::kDeath:
		//skydome_->Update();

		//playerのデスパーティクルが終わるまで
		if (deathParticle_ && deathParticle_->IsFinished()) {
			phase_ = Phase::kFadeOut;
		}
		//player
		if (deathParticle_) {
			deathParticle_->Update();
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

		//クリア
	case Phase::kClear:
		
		player_->Update();
		goal_->Update();

		if (Input::GetInstance()->TriggerKey(DIK_SPACE) && player_->IsClearMotionFinished()) {
			exitRequest_ = ExitRequest::Clear;
			fade_->Start(Fade::Status::FadeOut, 1.0f, Fade::FadeType::White);
			phase_ = Phase::kFadeOut;
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

		skydome_->Draw();
		player_->Draw();
		for (Enemy* enemy : enemies_) {
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
		if (!player_->IsDead())
			player_->Draw();
		for (Enemy* enemy : enemies_) 
		{
			enemy->Draw();
		}
		goal_->Draw();

		// enemyデスパーティクルあれば描画
		for (DeathParticles* deathParticles : deathParticlesList_) {
			deathParticles->Draw();
		}
		// playerデスパーティクルあれば描画
		if (deathParticle_) {
			deathParticle_->Draw();
		}
		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				blockModel_->Draw(*worldTransformBlock, camera_);
			}
		}

		#ifdef _DEBUG // デバックビルドのみ見れる
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
#endif // デバックビルドのみ見れる

		// 3Dモデル描画後処理
		Model::PostDraw();

		// スプライト描画処理前処理
		Sprite::PreDraw(dxCommon->GetCommandList());

		// ここにスプライトインスタンスの(2Dキャラ)描画処理を記述する
		// sprite_->Draw();

		fade_->Draw();

		//TABキーでポーズ
		SpriteTABFont_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
		SpriteTABFont_->Draw();
		// ポーズ画面
		if (isPause_) {
			if (currentPauseState_ == PauseState::Resume) {
				SpritePauseFont_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseFont_->Draw();
				SpritePauseResume_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseResume_->Draw();
				SpritePauseRetry_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseRetry_->Draw();
				SpritePauseTitle_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseTitle_->Draw();
				SpritePauseEnter_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseEnter_->Draw();
			} else if (currentPauseState_ == PauseState::Retry) {
				SpritePauseFont_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseFont_->Draw();
				SpritePauseResume_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseResume_->Draw();
				SpritePauseRetry_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseRetry_->Draw();
				SpritePauseTitle_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseTitle_->Draw();
				SpritePauseEnter_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseEnter_->Draw();
			} else if (currentPauseState_ == PauseState::Title) {
				SpritePauseFont_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseFont_->Draw();
				SpritePauseResume_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseResume_->Draw();
				SpritePauseRetry_->SetColor({0.0f, 0.0f, 0.0f, 0.4f});
				SpritePauseRetry_->Draw();
				SpritePauseTitle_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseTitle_->Draw();
				SpritePauseEnter_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
				SpritePauseEnter_->Draw();
			}
		}
		//ドアの前だとドア開けるspace描画
		if (!isPause_&&IsGoalSpace_ && goal_->GetState() == Goal::State::kCanOpen) {
			spriteSpace_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
			spriteSpace_->Draw();
		}
		// スプライト描画後処理
		Sprite::PostDraw();

		break;

	case GameScene::Phase::kDeath:

		// 3Dモデル描画前処理
		Model::PreDraw(dxCommon->GetCommandList());

		skydome_->Draw();
		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}

		// enemyデスパーティクルあれば描画
		for (DeathParticles* deathParticles : deathParticlesList_) {
			deathParticles->Draw();
		}
		// playerデスパーティクルあれば描画
		if (deathParticle_) {
			deathParticle_->Draw();
		}

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

		//クリア
	case GameScene::Phase::kClear:

		// 3Dモデル描画前処理
		Model::PreDraw(dxCommon->GetCommandList());

		skydome_->Draw();
		player_->Draw();
		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}
		goal_->Draw();

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
		
		//spaceキー
		spriteSpace_->SetColor({0.0f, 0.0f, 0.0f, 0.9f});
		spriteSpace_->Draw();
		spriteClear_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
		spriteClear_->Draw();
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
	//enemy
	for (DeathParticles* deathParticles : deathParticlesList_) {
		delete deathParticles;
	}
	deathParticlesList_.clear();
	delete deathParticle_;//player
	delete debugCamera_;
	delete blockModel_;
	delete goal_;
	delete BlackGoalModel_;
	delete LeftOpenGoalModel_;
	delete RightOpenGoalModel_;
	//for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
	//	for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
	//		delete worldTransformBlock;
	//	}
	//}
	worldTransformBlocks_.clear();
	delete skyDomeModel_;
	delete mapChipField_;
	delete deathParticlesModel_;
	delete SpriteTABFont_;
	delete SpritePauseFont_;
	delete SpritePauseResume_;
	delete SpritePauseRetry_;
	delete SpritePauseTitle_;
	delete SpritePauseEnter_;
	delete spriteSpace_;
}