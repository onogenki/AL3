#include "GameScene.h"
#include"math.h"
using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込み
	textureHandle_ = TextureManager::Load("sample.png");

	//スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

	//3Dモデルの生成	
	model_ = Model::Create();
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
	modelSkydome_ = Model::CreateFromOBJ("Skydome", true);
	skydome_->Initialize(modelSkydome_, &camera_);
	//マップチップ
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	//プレイヤー生成
	player_ = new Player();
	//プレイヤーモデル
	player_model_ = Model::CreateFromOBJ("player");
	//0205 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);
	
	//0207
	player_->SetMapChipField(mapChipField_);

	player_->Initialize(player_model_, &camera_, playerPosition);

	// 02_06カメラコントローラ
	CController_ = new CameraController(); // 生成
	CController_->Initialize(&camera_);    // 初期化
	CController_->SetTarget(player_);      // 追従対象セット
	CController_->Reset();                 // リセット

	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	CController_->SetMovableArea(cameraArea);

	//0209敵クラス
	enemy_ = new Enemy();
	//敵モデル
	enemy_model_ = Model::CreateFromOBJ("enemy");
	//敵位置決めて敵クラス初期化
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(14, 18);
	enemy_->Initialize(enemy_model_, &camera_, enemyPosition);

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
			}
		}
	}
}

void GameScene::Update() {

	player_->Update();
	skydome_->Update();
	CController_->Update();

	enemy_->Update();
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// フラグをトグル
		isDebugCameraActive_ = !isDebugCameraActive_;
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

	// デバッグカメラの更新
	debugCamera_->Update();
}


void GameScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 自キャラの描画
	player_->Draw();

	// 天球描画
	skydome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			block_model_->Draw(*worldTransformBlock, camera_);
		}
	}

		enemy_->Draw();

	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();
}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;

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
	delete enemy_;
}