#pragma once
#include "xx_map.h"

namespace Map {

	struct Scene;

	/*********************************************************************************************/

	enum class MapItemTypes : int8_t {
		Unknown,
		Space,
		Block,
	};

	struct MapItem {
		MapItemTypes type{};
		MapItemTypes bottomNeighborType{};	// == map[ x, y + 1 ].type
		int16_t groupId{};

		// c
		// B
		XX_INLINE bool Walkable() const {
			return type == MapItemTypes::Space
				&& bottomNeighborType == MapItemTypes::Block;
		}

		// c
		// .
		XX_INLINE bool Fallable() const {
			return type == MapItemTypes::Space
				&& bottomNeighborType == MapItemTypes::Space;
		}
	};

	struct Map : xx::Map2d<MapItem> {
		using P = xx::Map2d<MapItem>;
		using P::P;

		XX_INLINE std::optional<int32_t> FindFallingY(int32_t x, int32_t y) const {
			assert(x >= 0 && x < size.x && y >= 0 && y < size.y);
			assert(((*this)[x, y].type == MapItemTypes::Space));
			for (; y < size.y; ++y) {
				if ((*this)[x, y].Walkable()) return y;
			}
			return {};
		};

		template<MapItemTypes t>
		XX_INLINE bool IsTypeAt(int32_t x, int32_t y) const {
			if (x < 0 || y < 0 || x >= size.x || y >= size.y) return false;
			return (*this)[x, y].type == t;
		}
		XX_INLINE bool IsSpaceAt(int32_t x, int32_t y) const {
			return IsTypeAt<MapItemTypes::Space>(x, y);
		}
	};

	struct SpaceGroup {
		//   .B?                ?B.
		bool leftEdgeCanFall{}, rightEdgeCanFall{};
		int32_t mapY{};
		xx::FromTo<int32_t> mapX{};
		xx::Listi32<int32_t> navTips;	// store navigation path's next group id. -1: unreachable
	};

	/*********************************************************************************************/

	struct Scene : xx::SceneBase {
		Map map;
		xx::Listi32<SpaceGroup> spaceGroups;
		xx::AStarGrid asg;

		XYi beginPos{}, endPos{};
		void InitMap(std::u32string_view mapText);
		void DumpMap();
		void InitSpaceGroups();
		void InitAStarGrid();
		void FillNavTips();

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
