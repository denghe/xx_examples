#pragma once
#include "xx_astar.h"

namespace xx {

	template<typename T>
	struct MapBase {
		XYi size{};
		std::unique_ptr<T[]> cells;

		XX_INLINE void Init(XYi size_) {
			assert(!cells);
			assert(size_.x > 0 && size_.y);
			size = size_;
			cells = std::make_unique_for_overwrite<T[]>(size_.x * size_.y);
		}
		XX_INLINE void Init(int32_t width, int32_t height) {
			Init({ width, height });
		}

		// out of range: return nullptr
		XX_INLINE T* TryAt(XYi pos) const {
			if (pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y) return nullptr;
			return (T*)&cells[pos.y * size.x + pos.x];
		}
		XX_INLINE T* TryAt(int32_t x, int32_t y) const {
			return TryAt({ x, y });
		}

		XX_INLINE T* At(XYi pos) const {
			assert(pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y);
			return (T*)&cells[pos.y * size.x + pos.x];
		}
		XX_INLINE T* At(int32_t x, int32_t y) const {
			return At({ x, y });
		}

		XX_INLINE T& operator[](XYi pos) const {
			assert(pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y);
			return (T&)cells[pos.y * size.x + pos.x];
		}
		XX_INLINE T& operator[](int32_t x, int32_t y) const {
			return operator[]({ x, y });
		}
	};

	enum class MapCellTypes : int8_t {
		Unknown,
		Space,
		Block,
	};

	struct MapCell {
		MapCellTypes type{};
		MapCellTypes bottomNeighborType{};	// == map[ x, y + 1 ].type
		int16_t groupId{};

		// c
		// B
		XX_INLINE bool Walkable() const {
			return type == MapCellTypes::Space
				&& bottomNeighborType == MapCellTypes::Block;
		}

		// c
		// .
		XX_INLINE bool Fallable() const {
			return type == MapCellTypes::Space
				&& bottomNeighborType == MapCellTypes::Space;
		}
	};

	struct MapCellGroup {
		//   .B?                ?B.
		bool leftEdgeCanFall{}, rightEdgeCanFall{};
		int32_t mapY{};
		xx::FromTo<int32_t> mapX{};
		xx::Listi32<int32_t> navTips;	// store navigation path's next group id. -1: unreachable
	};

	struct Map : xx::MapBase<MapCell> {
		using P = xx::MapBase<MapCell>;
		using P::P;

		XX_INLINE std::optional<int32_t> FindFallingY(int32_t x, int32_t y) const {
			assert(x >= 0 && x < size.x && y >= 0 && y < size.y);
			assert(At(x, y)->type == MapCellTypes::Space);
			for (; y < size.y; ++y) {
				if (At(x, y)->Walkable()) return y;
			}
			return {};
		};

		template<MapCellTypes t>
		XX_INLINE bool IsTypeAt(int32_t x, int32_t y) const {
			if (x < 0 || y < 0 || x >= size.x || y >= size.y) return false;
			return At(x, y)->type == t;
		}
		XX_INLINE bool IsSpaceAt(int32_t x, int32_t y) const {
			return IsTypeAt<MapCellTypes::Space>(x, y);
		}

		// [&](auto x, auto y){}
		template<typename CHandleFunc, typename EHandleFunc>
		void InitByText(std::u32string_view s, CHandleFunc&& cf, EHandleFunc&& ef) {
			// '　'				space
			// 'Ｂ'				block
			// 'ｃ'				begin pos( character ? )
			// 'ｅ'				end pos
			assert(s.size());

			// auto remove head empty line
			while (s[0] == U'\n' || s[0] == U'\r') {
				s = s.substr(1);
			}

			// auto remove last empty line
			while (s.back() == U'\n' || s.back() == U'\r') {
				s = s.substr(0, s.size() - 1);
			}

			// scan first line length
			int32_t width{};
			for (; width < s.size(); ++width) {
				if (s[width] == U'\n' || s[width] == U'\r') break;
			}

			// scan line count
			int32_t height{ 1 };
			for (auto& c : s) {
				if (c == U'\n') ++height;
			}

			Init({ width, height });

			// fill map contents
			int32_t x{}, y{};
			for (auto c : s) {
				switch (c) {
				case U'\r': continue;
				case U'\n':
					assert(x == width);
					x = 0;
					++y;
					continue;
				case U'ｃ':
					cf(x, y);
					goto LabDefault;
				case U'ｅ':
					ef(x, y);
					goto LabDefault;
				case U'Ｂ': {
					At(x, y)->type = xx::MapCellTypes::Block;
					break;
				}
				default: {
				LabDefault:
					At(x, y)->type = xx::MapCellTypes::Space;
				}
				}
				++x;
			}

			for (int32_t y = 0; y < height - 1; ++y) {
				for (int32_t x = 0; x < width; ++x) {
					At(x, y)->bottomNeighborType = (*this)[x, y + 1].type;
				}
			}
		}

		std::u32string Dump() {
			std::u32string s;

			auto width = size.x;
			auto height = size.y;
			for (int32_t y{}; y < height; ++y) {
				for (int32_t x{}; x < width; ++x) {
					switch (At(x, y)->type) {
					case xx::MapCellTypes::Space:
						s.push_back(U'　');
						break;
					case xx::MapCellTypes::Block:
						s.push_back(U'Ｂ');
						break;
					}
				}
				s.push_back(U'\n');
			}

			return s;
		}

	};

}
