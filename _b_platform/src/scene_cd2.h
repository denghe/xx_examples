#pragma once

namespace CollisionDetection2 {

	// space index with size limited boxs
	// requires T:: (XYi) pos size  (int32_t) indexAtItems indexAtCells
	// requires T:: ( T* )prev next
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

		xx::Shared<T>& Add(xx::Shared<T> c) {
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
				cells[ci] = c;
			}

			// store
			c->indexAtItems = items.len;
			return items.Emplace(std::move(c));
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

		// out of range: return nullptr
		XX_INLINE T* At(XYi const& cri) const {
			if (cri.x < 0 || cri.x >= numCols) return nullptr;
			if (cri.y < 0 || cri.y >= numRows) return nullptr;
			auto ci = cri.y * numCols + cri.x;
			return cells[ci];
		}

		// todo: more search funcs
	};

	struct Scene;

	struct Item {
		static constexpr XYi cResSize{ 64, 64 };

		Scene* scene{};
		XYi pos{}, size{};							// pos: left-top
		xx::RGBA8 color{ xx::RGBA8_White };

		void Init(Scene* scene_, XYi const& pos_, XYi const& size_);
		void Draw();
	};

	struct Character : Item {
		XYi mouseOffset{}, lastPos{};
		bool dragging{}, lastMBState{};

		Character& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		void Update();

		void HandleCollision();
		bool HasCross(XYi const& newPos_) const;
	};

	enum class PushOutWays : uint32_t {
		Unknown = 0, Up = 1, Right = 2, Down = 4, Left = 8
	};


	struct Block : Item {
		//Block* prev{}, * next{};						// for space index
		int32_t indexAtItems{ -1 }, indexAtCells{ -1 };	// for space index
		xx::Math::BlockWayout wayout{};

		Block& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		bool IsCross(XYi const& cPos, XYi const& cSize) const;
		void FillWayout();
		std::pair<XYi, PushOutWays> PushOut(XYi const& cPos, XYi const& cSize) const;
	};

	struct Scene : xx::SceneBase {
		xx::Shared<Character> character;
		SpaceIndexBox<Block, false> blocks;

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
