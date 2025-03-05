#pragma once
#include "xx_math.h"
#include "xx_list.h"
#include "xx_ptr.h"

namespace xx {

	// space index with size limited boxs
	// requires T:: (XYi) pos size  (int32_t) indexAtItems indexAtCells
	// requires T:: ( T* )prev next	      ( if enableDoubleLink == true )
	template<typename T, bool enableDoubleLink>
	struct SpaceIndexBox {
		XYi cellSize{}, gridSize{};					// gridSize = cellSize * numCR
		int32_t numRows{}, numCols{};
		xx::Listi32<xx::Shared<T>> items;
		int32_t cellsLen{};
		std::unique_ptr<T*[]> cells;

		void Init(int32_t numRows_, int32_t numCols_, XYi const& cellSize_, int32_t cap = 10000) {
			assert(!cells);
			assert(numCols_ > 0 && numRows_ > 0 && cellSize_.x > 0 && cellSize_.y > 0);
			numRows = numRows_;
			numCols = numCols_;
			cellSize = cellSize_;
			gridSize = { cellSize_.x * numCols_, cellSize_.y * numRows_ };

			cellsLen = numRows * numCols;
			cells = std::make_unique_for_overwrite<T*[]>(cellsLen);
			memset(cells.get(), 0, sizeof(T*) * cellsLen);

			items.Reserve(cap);
		}

		template<typename U>
		xx::Shared<U>& Add(xx::Shared<U> c) requires std::is_base_of_v<T, U> {
			assert(c && c->indexAtItems == -1 && c->indexAtCells == -1);
			auto cri = PosToColRowIndex(c->pos);
			auto ci = ColRowIndexToCellIndex(cri);

			if constexpr (enableDoubleLink) {
				// link
				assert(!c->prev && !c->next);
				assert(!cells[ci] || !cells[ci]->prev);
				if (cells[ci]) {
					cells[ci]->prev = c;
				}
				c->next = cells[ci];
				c->indexAtCells = ci;
				cells[ci] = c;
				assert(!cells[ci]->prev);
				assert(c->next != c);
				assert(c->prev != c);
			} else {
				assert(!cells[ci]);
				c->indexAtCells = ci;
				cells[ci] = c;
			}

			// store
			c->indexAtItems = items.len;
			return (xx::Shared<U>&)items.Emplace(std::move(c));
		}

		void Remove(T* c) {
			assert(c);
			assert(c->indexAtItems != -1 && c->indexAtCells != -1);

			if constexpr (enableDoubleLink) {
				assert(c->next != c && c->prev != c);
				assert(!c->prev && cells[c->indexAtCells] == c || c->prev->next == c && cells[c->indexAtCells] != c);
				assert(!c->next || c->next->prev == c);

				// unlink
				if (c->prev) {	// isn't header
					assert(cells[c->indexAtCells] != c);
					c->prev->next = c->next;
					if (c->next) {
						c->next->prev = c->prev;
						c->next = {};
					}
					c->prev = {};
				} else {
					assert(cells[c->indexAtCells] == c);
					cells[c->indexAtCells] = c->next;
					if (c->next) {
						c->next->prev = {};
						c->next = {};
					}
				}
				assert(cells[c->indexAtCells] != c);
			} else {
				assert(cells[c->indexAtCells] == c);
				cells[c->indexAtCells] = nullptr;
			}

			// clear
			auto ii = c->indexAtItems;
			items.Back()->indexAtItems = ii;
			items.SwapRemoveAt(ii);
		}

		void Update(T* c) requires enableDoubleLink {
			assert(c);
			assert(c->indexAtItems != -1 && c->indexAtCells != -1);
			assert(c->next != c && c->prev != c);
			assert(!c->prev && cells[c->indexAtCells] == c || c->prev->next == c && cells[c->indexAtCells] != c);
			assert(!c->next || c->next->prev == c);

			auto cri = PosToColRowIndex(c->pos);
			auto ci = ColRowIndexToCellIndex(cri);
			if (ci == c->indexAtCells) return;	// no change
			assert(!cells[ci] || !cells[ci]->prev);

			// unlink
			if (c->prev) {	// isn't header
				assert(cells[c->indexAtCells] != c);
				c->prev->next = c->next;
				if (c->next) {
					c->next->prev = c->prev;
				}
			} else {
				assert(cells[c->indexAtCells] == c);
				cells[c->indexAtCells] = c->next;
				if (c->next) {
					c->next->prev = {};
				}
			}
			assert(cells[c->indexAtCells] != c);
			assert(ci != c->indexAtCells);

			// link
			if (cells[ci]) {
				cells[ci]->prev = c;
			}
			c->prev = {};
			c->next = cells[ci];
			cells[ci] = c;
			c->indexAtCells = ci;
			assert(!cells[ci]->prev);
			assert(c->next != c);
			assert(c->prev != c);
		}

		XX_INLINE XYi PosToColRowIndex(XYi const& pos) const {
			auto cri = pos / cellSize;
			assert(!(cri.x < 0 || cri.x >= numCols || cri.y < 0 || cri.y >= numRows));
			return cri;
		}

		XX_INLINE int32_t ColRowIndexToCellIndex(XYi const& cri) const {
			auto ci = cri.y * numCols + cri.x;
			assert(ci >= 0 && ci < cellsLen);
			return ci;
		}

		XX_INLINE XYi CellIndexToColRowIndex(int32_t ci) const {
			assert(ci >= 0 && ci < cellsLen);
			auto ri = ci / numCols;
			return { ci - ri * numCols, ri };
		}

		// out of range: return nullptr
		XX_INLINE T* At(XYi const& cri) const {
			if (cri.x < 0 || cri.x >= numCols) return nullptr;
			if (cri.y < 0 || cri.y >= numRows) return nullptr;
			auto ci = cri.y * numCols + cri.x;
			return cells[ci];
		}

		// todo: more search funcs
	};

}
