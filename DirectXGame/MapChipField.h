#pragma once
#include "KamataEngine.h"
#include <vector>
#include "Math.h"

using namespace KamataEngine;

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {

	public:

	//指定座標がマップチップの何番目の位置か
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	//範囲矩形
	struct Rect {
		float left;   // 左
		float right;  // 右
		float bottom; // 下
		float top;    // 上
	};

	MapChipData mapChipData_;

	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }

	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	//ブロックの範囲取得関数
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	private:

	//1ブロックのサイズ(ブロック用のモデルのサイズに合わせる)
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	//ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

};
