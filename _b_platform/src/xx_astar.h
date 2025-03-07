#pragma once
#include "xx_xy.h"

namespace xx {

	// need cleanup for next search
	struct AStarCellBase {
		struct AStarCell* parent{};
		float heuristicStartToEndLen{}, startToCurLen{}, heuristicCurToEndLen{};
		char heuristicCurToEndLen_hasValue{}, opened{}, closed{};
	};

	struct AStarCell : AStarCellBase {
		int32_t x{}, y{}, walkable{};
		int32_t neighborsIndex{}, neighborsCount{};
	};

	struct AStarGrid {
		AStarCell empty{};
		int32_t width{}, height{};								// map size
		xx::Listi32<AStarCell> cells;							// map data
		xx::Listi32<AStarCell*> openList;						// tmp
		xx::Listi32<AStarCell*> path;							// search result
		xx::Listi32<std::pair<AStarCell*, float>> neighbors;	// global container. pair.second: distance

		static constexpr float sqrt_2 = 1.414213562373095;

		XX_INLINE AStarCell* At(int32_t x, int32_t y) {
			assert(x >= 0 && y >= 0 && x < width && y < height);
			return &cells[(size_t)y * width + x];
		}

		XX_INLINE AStarCell* TryAt(int32_t x, int32_t y) {
			if (x < 0 || y < 0 || x >= width || y >= height) return &empty;
			return &cells[(size_t)y * width + x];
		}

		XX_INLINE void OpenListAdd(AStarCell* c) {
			c->opened = 1;
			openList.Emplace(c);
			std::push_heap(openList.buf, openList.buf + openList.len, [](auto a, auto b) {
				return a->heuristicStartToEndLen > b->heuristicStartToEndLen;
			});
		}

		XX_INLINE AStarCell* OpenListPop() {
			auto first = openList[0];
			first->opened = 0;
			first->closed = 1;
			std::pop_heap(openList.buf, openList.buf + openList.len, [](auto a, auto b) {
				return a->heuristicStartToEndLen > b->heuristicStartToEndLen;
			});
			openList.PopBack();
			return first;
		}

		bool Search(XYi const& from, XYi const& to) {
			assert(width && height);
			assert(!path.len);
			assert(from != to);

			auto startCell = At(from.x, from.y);
			auto endCell = At(to.x, to.y);

			OpenListAdd(startCell);
			while (!openList.Empty()) {
				auto c = OpenListPop();
				if (c == endCell) {
					do {
						path.Emplace(c);
					} while ((c = c->parent));
					return true;
				}
				for (int32_t i = 0; i < c->neighborsCount; ++i) {
					auto [n, d] = neighbors[c->neighborsIndex + i];
					if (!n->walkable || n->closed) continue;
					auto len = c->startToCurLen + d;
					if (len < n->startToCurLen || !n->opened) {
						n->startToCurLen = len;
						if (!n->heuristicCurToEndLen_hasValue) {
							n->heuristicCurToEndLen = std::sqrtf(float((n->x - endCell->x) * (n->x - endCell->x) + (n->y - endCell->y) * (n->y - endCell->y)));
							n->heuristicCurToEndLen_hasValue = 1;
						}
						n->heuristicStartToEndLen = n->startToCurLen + n->heuristicCurToEndLen;
						n->parent = c;
						if (!n->opened) {
							OpenListAdd(n);
						}
					}
				}
			}
			return false;
		}

		// for next Search but use old map data
		XX_INLINE void Cleanup() {
			openList.Clear();
			path.Clear();
			for (auto&& c : cells) {
				memset(&c, 0, sizeof(AStarCellBase));
			}
		}

		void Init(int32_t width_, int32_t height_) {
			cells.Clear();
			neighbors.Clear();
			Cleanup();
			width = width_;
			height = height_;
			cells.Resize((size_t)width * height);
			// next step, need InitCell & InitCellNeighbors for all cells by caller
		}

		void InitCell(int32_t x, int32_t y, bool walkable) {
			auto c = At(x, y);
			c->x = x;
			c->y = y;
			c->walkable = walkable ? 1 : 0;
		}

		void InitCellNeighbors(AStarCell* c, XYi* neighborOffsets, int32_t neighborsCount) {
			assert(neighborOffsets && neighborsCount > 0);
			auto cx = c->x;
			auto cy = c->y;
			c->neighborsIndex = neighbors.len;
			c->neighborsCount = neighborsCount;
			for (int32_t i = 0; i < neighborsCount; ++i) {
				auto& o = neighborOffsets[i];
				auto len = std::sqrtf(float(o.x * o.x + o.y * o.y));
				auto n = At(cx + o.x, cy + o.y);
				neighbors.Emplace(n, len);
			}
		}

		std::u32string Dump(XYi const& from, XYi const& to) {
			std::u32string s;
			for (int32_t y = 0; y < height; ++y) {
				for (int32_t x = 0; x < width; ++x) {
					if (At(x, y)->walkable) s.push_back(U'　');
					else s.push_back(U'Ｂ');
				}
				s.push_back(U'\n');
			}
			for (auto& c : path) {
				s[c->y * (width + 1) + c->x] = U'＊';
			}
			s[from.y * (width + 1) + from.x] = U'ｃ';
			s[to.y * (width + 1) + to.x] = U'ｅ';
			return s;
		}
	};

}
