#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

enum class MapChipType {
	kNone, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {

public:

	//0207
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};
	//0207範囲矩形
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	
	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

	//0207
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
	//0207
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:

	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;
};