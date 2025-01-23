#pragma once

namespace CollisionDetection2 {

	// space index with size limited boxs
	// requires T:: (XYi) pos size  (int32_t) indexAtItems indexAtCells
	// requires T:: ( T* )prev next
	template<typename T>
	struct SpaceIndexBox {
		XYi cellSize;							// max value
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

			cellsLen = numRows * numCols;
			cells = std::make_unique_for_overwrite<T*[]>(cellsLen);
			memset(cells.get(), 0, sizeof(T*) * cellsLen);

			items.Reserve(cap);
		}

		xx::Shared<T>& Add(xx::Shared<T> c) {
			assert(c && c->indexAtItems == -1 && c->indexAtCells == -1);
			assert(!c->prev && !c->next);
			auto cri = PosToColRowIndex(c->pos);
			auto ci = ColRowIndexToCellIndex(cri);
			assert(!cells[ci] || !cells[ci]->prev);

			// link
			if (cells[ci]) {
				cells[ci]->prev = c;
			}
			c->next = cells[ci];
			c->indexAtCells = ci;
			cells[ci] = c;
			assert(!cells[ci]->prev);
			assert(c->next != c);
			assert(c->prev != c);

			// store
			c->indexAtItems = items.len;
			return items.Emplace(std::move(c));
		}

		void Remove(T* c) {
			assert(c);
			assert(c->indexAtItems != -1 && c->indexAtCells != -1);
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

			// clear
			auto ii = c->indexAtItems;
			items.Back()->indexAtItems = ii;
			items.SwapRemoveAt(ii);
		}

		void Update(T* c) {
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
			auto r = pos / cellSize;
			assert(!(r.x < 0 || r.x >= numCols || r.y < 0 || r.y >= numRows));
			return r;
		}

		XX_INLINE int32_t ColRowIndexToCellIndex(XYi const& cri) const {
			auto r = cri.y * numCols + cri.x;
			assert(r >= 0 && r < cellsLen);
			return r;
		}

		// out of range: return nullptr
		XX_INLINE T* At(XYi const& cri) const {
			if (cri.x < 0 || cri.x >= numCols) return nullptr;
			if (cri.y < 0 || cri.y >= numRows) return nullptr;
			auto cIndex = cri.y * numCols + cri.x;
			return cells[cIndex];
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
		Block* prev{}, * next{};						// for space index
		int32_t indexAtItems{ -1 }, indexAtCells{ -1 };	// for space index

		Block& Init(Scene* scene_, XYi const& pos_ = {}, XYi const& size_ = cResSize);
		bool IsCross(XYi const& cPos, XYi const& cSize) const;

		//template<int32_t isPushX>
		//std::pair<int32_t, PushOutWays> PushOut(XYi const& cPos, XYi const& cSize) const;
	};

	struct Scene : xx::SceneBase {
		xx::Shared<Character> character;
		SpaceIndexBox<Block> blocks;

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
