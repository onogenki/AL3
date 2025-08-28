#include "GameScene.h"
#include"math.h"
using namespace KamataEngine;

// エフェクトを生成
void GameScene::CreateEffect(const Vector3& position) {
	HitEffect* newHitEffect = HitEffect::Create(position,&camera_);

	hitEffects_.push_back(newHitEffect);
}

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込み
	textureHandle_ = TextureManager::Load("sample.png");

	//スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

	//3Dモデルの生成	
	//model_ = Model::Create();
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラの初期化
	camera_.Initialize();
	
	// ブロックモデルの生成
	block_model_ = Model::CreateFromOBJ("block");

	// デバックカメラの生成                 画面横幅              画面縦幅
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 0203天球
	// skydome生成
	skydome_ = new Skydome();
	// 初期化
	modelSkydome_ = Model::CreateFromOBJ("sky_sphere", true);
	skydome_->Initialize(modelSkydome_, &camera_);
	//マップチップ
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	//プレイヤー生成
	player_ = new Player();
	//プレイヤーモデル
	player_model_ = Model::CreateFromOBJ("player");
	// 0205 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 16);
	//プレイヤー攻撃エフェクトモデル
	modelAttack_ = Model::CreateFromOBJ("attack_effect");
	//0207
	player_->SetMapChipField(mapChipField_);

	player_->Initialize(player_model_,modelAttack_, &camera_, playerPosition);

	//0206 カメラコントローラ
	CController_ = new CameraController(); // 生成
	CController_->Initialize(&camera_);    // 初期化
	CController_->SetTarget(player_);      // 追従対象セット
	CController_->Reset();                 // リセット

	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	CController_->SetMovableArea(cameraArea);

	//敵モデル
	enemy_model_ = Model::CreateFromOBJ("enemy");

	const int32_t enemyYPositions[] = {9, 17};
	//0210
	for (int32_t i = 0; i < 2; ++i)
	{
		Enemy* newEnemy = new Enemy();
		//                                     2体ずつ異なる座標をセット
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(10 + i*10, enemyYPositions[i]);

		newEnemy->Initialize(enemy_model_, &camera_, enemyPosition);
		
		//MapChipFieldを修正
		 newEnemy->SetMapChipField(mapChipField_);

		//0216
		newEnemy->SetGameScene(this);

		enemies_.push_back(newEnemy);
	}

	//モデル読み込み
	deathParticle_model_ = Model::CreateFromOBJ("deathParticle");

	// 02_16
	particle_model_ = Model::CreateFromOBJ("particle");

	phase_ = Phase::kFadeIn;

	//0213
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	//0216
	HitEffect::SetModel(particle_model_);
	HitEffect::SetCamera(&camera_);

	//アイテム
	startItem_model_ = Model::CreateFromOBJ("startFlag");
	playerItem_model_ = Model::CreateFromOBJ("playerFlag");
	enemiesItem_model_ = Model::CreateFromOBJ("enemyFlag");

	//座標をマップチップ番号で指定(if文でモデルを変える)
	Vector3 itemPositionU = mapChipField_->GetMapChipPositionByIndex(13, 9);
	Vector3 itemPositionU2 = mapChipField_->GetMapChipPositionByIndex(18, 9);
	Vector3 itemPositionD = mapChipField_->GetMapChipPositionByIndex(14, 17);

	Item* newItem1 = new Item();
	newItem1->Initialize(startItem_model_, playerItem_model_, enemiesItem_model_, &camera_, itemPositionU);
	items_.push_back(newItem1);

	Item* newItem2 = new Item();
	newItem2->Initialize(startItem_model_, playerItem_model_, enemiesItem_model_, &camera_, itemPositionU2);
	items_.push_back(newItem2);

	Item* newItem3 = new Item();
	newItem3->Initialize(startItem_model_, playerItem_model_, enemiesItem_model_, &camera_, itemPositionD);
	items_.push_back(newItem3);

	//timer
	gameTimer_ = 0.0f;


}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
			if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		    break;
	case Phase::kPlay:
		// Initialize関数のいきなりパーティクル発生処理は消す
		if (player_->IsDead()) {
			// 死亡演出
			phase_ = Phase::kDeath;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(deathParticle_model_, &camera_, deathParticlesPosition);

			fade_->Start(Fade::Status::FadeOut, 1.0f);
			}
		break;
	case Phase::kDeath:
		if (fade_->IsFinished()) {
			Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 16);
			player_->Initialize(player_model_, modelAttack_, &camera_, playerPosition);

			fade_->Start(Fade::Status::FadeIn, 1.0f);
			phase_ = Phase::kPlay;
		}
		break;
	}
}

void GameScene::GenerateBlocks() {
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
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			
				//奥に配置する
				worldTransformBlocks_[i][j]->translation_.z += 0.8f;
			
			}
		}
	}
}

void GameScene::Update() {

	//0216 デスフラグの立ったエフェクトを削除
	hitEffects_.remove_if([](HitEffect* hitEffect) {
		if (hitEffect->IsDead()) {
			delete hitEffect;
			return true;
		}
		return false;
	});

	//0215 デスフラグの立った敵を削除
	//enemies_.remove_if([](Enemy* enemy) {
	//	if (enemy->IsDead()) {
	//		delete enemy;
	//		return true;
	//	}
	//	return false;
	//});

	// エフェクトの更新
	for (auto it = hitEffects_.begin(); it != hitEffects_.end();) {
		(*it)->Update();
		if ((*it)->IsDead()) {
			delete *it;
			it = hitEffects_.erase(it); // 死んだら削除
		} else {
			++it;
		}
	}

	fade_->Update();
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kPlay;
		}

		skydome_->Update();
		CController_->Update();

		player_->Update();

		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// アイテムの更新
		for (Item* item : items_) {
			item->Update(player_, enemies_);
		}

		// ヒットエフェクトリストの更新をfor文で行う
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			// フラグをトグル
			//isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
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
		// ヒットエフェクトリストの更新をfor文で行う
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}
		break;
	case Phase::kPlay:

		gameTimer_ += 1.0f / 60.0f;
		if (gameTimer_ >= gameTimeOver_ || player_->IsDead())
		{
			fade_->Start(Fade::Status::FadeOut, 3.0f);
			phase_ = Phase::kFadeOut;
		}

		skydome_->Update();
		CController_->Update();

		// 自キャラの更新
		player_->Update();

		//敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		//アイテムの更新
		for (Item* item : items_) {
			item->Update(player_, enemies_);
		}

		// #ifdef _DEBUG
		//if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			// フラグをトグル
			//isDebugCameraActive_ = !isDebugCameraActive_;
		//}
//#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;

				// アフィン変換～DirectXに転送
				//WorldTransformUpdate(*worldTransformBlock);
			}
		}

		CheckAllCollisions();
		break;

	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 3.0f);
			phase_ = Phase::kDeath;
		}

		skydome_->Update();
		CController_->Update();

		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		if (deathParticles_) {
			deathParticles_->Update();
		}
		//ヒットエフェクトリストの更新をfor文で行う
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			//ゲーム終了時に陣確認
			CountItems();//関数を呼び出す
			finished_ = true;
		}

		skydome_->Update();
		CController_->Update();

		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		break;
	}
}

void GameScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 天球描画
	skydome_->Draw();

	// 自キャラの描画
	if (!player_->IsDead())
		player_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			block_model_->Draw(*worldTransformBlock, camera_);
		}
	}

		//enemy_->Draw();
	for (Enemy* enemy : enemies_)
	{
		enemy->Draw();
	}

	//デスパーティクルあれば描画
	if (deathParticles_)
	{
		deathParticles_->Draw();
	}

	//ヒットエフェクトリストの描画をfor文で行う
	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Draw();
	}

	for (Item* item : items_) {
		item->Draw();
	}

	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();

	//0213
	fade_->Draw();

}

//0210
void GameScene::CheckAllCollisions() {

	//判定対象1と2の座標
	AABB aabb1, aabb2;

#pragma region 自キャラと敵キャラの当たり判定
	{
		//自キャラの座標
		aabb1 = player_->GetAABB();

		//自キャラと敵弾全ての当たり判定
		for (Enemy* enemy : enemies_) {

			// コリジョン無効の敵はスキップ
			if (enemy->IsCollisionDisabled()) {
				continue;
			}
			//敵弾の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定
			//死んだときに分けてるため、どちらも初期化されることを防いでる
			if (IsCollision(aabb1, aabb2)) {
				if (player_->IsAttack()) {

					enemy->OnCollision(player_);
				} else {
					// "自キャラ"の衝突時関数を呼び出す
					player_->OnCollision(enemy);
					// "敵"の衝突時関数を呼び出す
					// enemy->OnCollision(player_);
				}
			}
		}
	}
#pragma endregion
}

//陣確認
void GameScene::CountItems() {
	int32_t playerCount = 0;//player陣の変数
	int32_t enemyCount = 0;//enemy陣の変数

	// 陣リストをループして所有者ごとにカウント
	for (Item* item : items_) {
		if (item->GetOwner() == Item::Owner::kPlayer) {//player陣が見つかったら+1
			playerCount++;
		} else if (item->GetOwner() == Item::Owner::kEnemy) {//enemy陣なら+1
			enemyCount++;
		}
	}

	// カウント結果を元に勝利判定
	if (playerCount > enemyCount)
	{
		isPlayerWinner_ = true;
	} else {
		isPlayerWinner_ = false;
	}
}

GameScene::~GameScene() {
	delete sprite_;
	//delete model_;

	delete block_model_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;
	delete modelSkydome_;
	delete player_;
	delete mapChipField_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	delete deathParticles_;
	delete deathParticle_model_;

	//0216
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}

	//アイテム
	for (Item* item : items_) {
		delete item;
	}
}