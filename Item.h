#pragma once
#include "KamataEngine.h"
#include"Player.h"
#include"Enemy.h"

using namespace KamataEngine;

class Item {
	public:

		void Initialize(Model* startItem_model,Model* playerItem_model, Model* enemyItem_model, Camera* camera, const Vector3& position);

		void Update(Player* player, Enemy* enemy);

		void Draw();

		AABB GetAABB();

		Vector3 GetWorldPosition() const;

		private:

		WorldTransform worldTransform_;

		Model* startItem_model_ = nullptr;
	    Model* playerItem_model_ = nullptr;
	    Model* enemyItem_model_ = nullptr;

		Camera* camera_ = nullptr;

		Vector3 position_{};

		// 当たり判定サイズ
	    static inline const float kWidth = 0.8f;
	    static inline const float kHeight = 0.8f;

		bool isStartColor_ = false;//初期陣
	    bool isPlayerColor_ = false;//playerが当たったかどうか(色の変化)
	    bool isEnemyColor_ = false;//enemyが当たったかどうか
};
