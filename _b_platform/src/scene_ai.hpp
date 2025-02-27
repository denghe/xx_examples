﻿#pragma once

namespace AI {

	struct AStarCell {
		int32_t x{}, y{}, walkable{};
		int32_t neighborsIndex{}, neighborsCount{};
		AStarCell* parent{};

		// memory packed to 4 * 4 = 16 bytes
		float heuristicStartToEndLen{}, startToCurLen{}, heuristicCurToEndLen{};
		char heuristicCurToEndLen_hasValue{}, opened{}, closed{};
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
				memset(&c.heuristicStartToEndLen, 0, 4 * 4);	// heuristicStartToEndLen ~ closed
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
				s.push_back('\n');
			}
			for (auto& c : path) {
				s[c->y * (width + 1) + c->x] = U'＊';
			}
			s[from.y * (width + 1) + from.x] = U'ｃ';
			s[to.y * (width + 1) + to.x] = U'ｅ';
			return s;
		}
	};

	/***************************************************************************************/
	/***************************************************************************************/

	inline void Item::Init(Scene* scene_, XYi const& pos_) {
		scene = scene_;
		pos = pos_;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	XX_INLINE XYi Character::CRIndexToPos(XYi const& crIndex) const {
		auto p = MapCfg::cellSize * crIndex;	// calculate top left point pos
		p.x += MapCfg::cellSize.x >> 1;	// calculate bottom center point pos
		p.y += MapCfg::cellSize.y;
		assert(p.x >= 0 && p.y >= 0);
		return p;
	}

	inline Character& Character::Init(Scene* scene_, XYi const& crIndex) {
		auto p = CRIndexToPos(crIndex);
		Item::Init(scene_, p);
		_pos = p;	// sync float version
		return *this;
	}

	inline bool Character::Update() {
		// todo: AI search path logic
		// todo: run actions
		if (actions.Empty()) return false;
		auto& action = actions.Top();
		switch (action.type) {
		case ActionTypes::Move: {
			auto& a = (Action_Move&)action;
			auto tarPos = CRIndexToPos(a.tarCRIndex);
			if (pos != tarPos) {
				// todo: move logic
			}
			break;
		}
		case ActionTypes::Jump: {
			break;
		}
		case ActionTypes::Fall: {
			break;
		}
		}
		return false;
	}

	inline void Character::Draw() {
		auto& frame = *gRes.monster_3;
		auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
		q.pos = gLooper.camera.ToGLPos(_pos);
		q.anchor = cAnchor;
		q.scale = cScale * gLooper.camera.scale;
		q.radians = cRadians;
		q.colorplus = 1;
		q.color = xx::RGBA8_White;
		q.texRect.data = frame.textureRect.data;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline xx::Shared<Block> Block::Init(Scene* scene_, XYi const& crIndex) {

		auto p = MapCfg::cellSize * crIndex;	// calculate top left point pos

		Item::Init(scene_, p);

		size = MapCfg::cellSize;
		color = { 128,64,0,255 };

		return xx::SharedFromThis(this);
	}

	XX_INLINE bool Block::IsCross(XYi const& cPos, XYi const& cSize) const {
		if (cPos.x >= pos.x) {
			if (cPos.y >= pos.y) {
				if (cPos.x < pos.x + size.x) return cPos.y < pos.y + size.y;
			}
			else /* cPos.y < pos.y */ {
				if (cPos.x < pos.x + size.x) return cPos.y + cSize.y > pos.y;
			}
		}
		else /* cPos.x < pos.x */ {
			if (cPos.y >= pos.y) {
				if (cPos.x + cSize.x > pos.x) return cPos.y < pos.y + size.y;
			}
			else /* cPos.y < pos.y */ {
				if (cPos.x + cSize.x > pos.x) return cPos.y + cSize.y > pos.y;
			}
		}
		return false;
	}

	inline int32_t Block::CalcCrossLenX(int32_t cPosX, int32_t cSizeX) const {
		auto xLeft = std::min(pos.x, cPosX);
		auto xRight = std::max(pos.x + size.x, cPosX + cSizeX);
		return size.x + cSizeX - (xRight - xLeft);
	}

	inline void Block::Draw() {
		auto frame = gRes.quad;
		auto p = pos.As<float>();
		auto s = size.As<float>();
		auto q = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame->tex->GetValue(), 2);
		q[0].pos = gLooper.camera.ToGLPos(p);
		q[0].anchor = cAnchor;
		q[0].scale = s / cResSize * gLooper.camera.scale;
		q[0].radians = cRadians;
		q[0].colorplus = 1;
		q[0].color = color;
		q[0].texRect.data = frame->textureRect.data;

		q[1].pos = gLooper.camera.ToGLPos(p + 2.f);
		q[1].anchor = cAnchor;
		q[1].scale = s / cResSize * gLooper.camera.scale * ((s - 4.f) / s);
		q[1].radians = cRadians;
		q[1].colorplus = 1;
		q[1].color = xx::RGBA8_Black;
		q[1].texRect.data = frame->textureRect.data;
	}

	inline void Block::FillWayout() {
		// search neighbor & set wayout bit
		auto& bs = scene->blocks;
		auto cri = bs.PosToColRowIndex(pos);
		bool atEdge{};

		if (cri.y == 0) {
			wayout.up = false; atEdge = true;
		}
		else wayout.up = !bs.At({ cri.x, cri.y - 1 });

		if (cri.y + 1 == bs.numRows) {
			wayout.down = false; atEdge = true;
		}
		else wayout.down = !bs.At({ cri.x, cri.y + 1 });

		if (cri.x == 0) {
			wayout.left = false; atEdge = true;
		}
		else wayout.left = !bs.At({ cri.x - 1, cri.y });

		if (cri.x + 1 == bs.numCols) {
			wayout.right = false;
		}
		else wayout.right = !bs.At({ cri.x + 1, cri.y });

		if (atEdge && !(uint8_t&)wayout) {
			if (cri.y != 0) wayout.up = true;
			if (cri.y + 1 != bs.numRows) wayout.down = true;
			if (cri.x != 0) wayout.left = true;
			if (cri.x + 1 != bs.numCols) wayout.right = true;
		}
	}

	inline std::pair<XYi, PushOutWays> Block::PushOut(XYi const& cPos, XYi const& cSize) const {
		// calculate 4 way distance & choose min val
		auto bPosRB = pos + size;	// RB: right bottom
		auto bCenter = pos + XYi{ size.x >> 1, size.y >> 1 };
		auto cPosRB = cPos + cSize;
		auto cCenter = cPos + XYi{ cSize.x >> 1, cSize.y >> 1 };
		int32_t dLeft, dRight, dUp, dDown;
		if (cCenter.x >= bCenter.x) {
			if (wayout.left) dLeft = cPos.x - pos.x + cSize.x;
			else dLeft = 0x7FFFFFFF;
			if (wayout.right) dRight = bPosRB.x - cPos.x;
			else dRight = 0x7FFFFFFF;
			if (cCenter.y >= bCenter.y) {
				if (wayout.up) dUp = cPos.y - pos.y + cSize.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = bPosRB.y - cPos.y;
				else dDown = 0x7FFFFFFF;
			}
			else {
				if (wayout.up) dUp = cPosRB.y - pos.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = pos.y - cPos.y + cSize.y;
				else dDown = 0x7FFFFFFF;
			}
		}
		else {
			if (wayout.left) dLeft = cPosRB.x - pos.x;
			else dLeft = 0x7FFFFFFF;
			if (wayout.right) dRight = bPosRB.x - cPos.x;
			else dRight = 0x7FFFFFFF;
			if (cCenter.y >= bCenter.y) {
				if (wayout.up) dUp = cPos.y - pos.y + cSize.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = bPosRB.y - cPos.y;
				else dDown = 0x7FFFFFFF;
			}
			else {
				if (wayout.up) dUp = cPosRB.y - pos.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = pos.y - cPos.y + cSize.y;
				else dDown = 0x7FFFFFFF;
			}
		}
		if (dLeft == 0x7FFFFFFF && dRight == 0x7FFFFFFF && dUp == 0x7FFFFFFF && dDown == 0x7FFFFFFF)
			return { cPos, PushOutWays::Unknown };
		if (dRight <= dLeft && dRight <= dUp && dRight <= dDown)
			return { { cPos.x + dRight, cPos.y }, PushOutWays::Right };
		else if (dLeft <= dRight && dLeft <= dUp && dLeft <= dDown)
			return { { cPos.x - dLeft, cPos.y }, PushOutWays::Left };
		else if (dUp <= dLeft && dUp <= dRight && dUp <= dDown)
			return { { cPos.x, cPos.y - dUp }, PushOutWays::Up };
		else
			return { { cPos.x, cPos.y + dDown }, PushOutWays::Down };
	}


	/***************************************************************************************/
	/***************************************************************************************/

	inline void Scene::Init() {
		// Ｂ					block
		// ｃ					character
		// ｅ					end pos

		static std::u32string_view mapText{ UR"(
　　　　　　　　　　
Ｂ　　　　　　　　　　Ｂ
Ｂ　　　Ｂ　Ｂ　　　　Ｂ
Ｂｃ　ＢＢＢＢ　　　　Ｂ
ＢＢＢＢＢＢＢＢ　　　Ｂ
ＢＢＢＢＢＢＢＢＢＢ　Ｂ
Ｂ　　　　　　　　　　Ｂ
Ｂ　　　　　ＢＢＢＢＢＢ
Ｂ　　　　　　ＢＢＢＢＢ
Ｂ　ｅ　　　　　　　　Ｂ
ＢＢＢＢＢＢＢＢＢＢＢＢ
)" };

		mapText = mapText.substr(1, mapText.size() - 2);	// skip first & last new line

		// detect map max size
		int32_t maxX{}, x{}, y{};
		for (int32_t i = 0; i < mapText.size(); ++i) {
			auto c = mapText[i];
			switch (auto c = mapText[i]) {
			case U'\r': continue;
			case U'\n':
				if (maxX < x) maxX = x;
				x = 0;
				++y;
				continue;
			}
			++x;
		}
		blocks.Init(y + 1, maxX, MapCfg::cellSize);

		XYi beginPos{}, endPos{};

		// fill map contents
		x = 0;
		y = 0;
		for (int32_t i = 0; i < mapText.size(); ++i) {
			switch (auto c = mapText[i]) {
			case U'\r': continue;
			case U'\n':
				x = 0;
				++y;
				continue;
			case U'ｃ':
				beginPos = { x, y };
				break;
			case U'ｅ':
				endPos = { x, y };
				break;
			case U'Ｂ': {
				blocks.Add(xx::MakeShared<Block>()->Init(this, { x, y }));
				break;
			}
			}
			++x;
		}
		for (auto& o : blocks.items) o->FillWayout();

		character.Emplace()->Init(this, beginPos);

		gLooper.camera.SetOriginal({ character->_pos.x, blocks.gridSize.y / 2 });

		// astar test

		auto width = blocks.numCols;
		auto height = blocks.numRows;
		AStarGrid asg;
		asg.Init(width, height);

		// generate cells
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				//if (!blocks.At({ x, y })) {
				//	asg.InitCell(x, y, blocks.At({ x, y + 1 }));
				//}
				asg.InitCell(x, y, !blocks.At({ x, y }));
			}
		}

		// for search falling block
		auto FindFallingOffsetY = [&](int32_t x, int32_t y)->std::optional<int32_t> {
			std::optional<int32_t> r;
			if (x < 0 || y < 0 || x >= width || y >= height || blocks.At({ x, y })) return r;
			for (auto i = y + 1; i < height; ++i) {
				if (blocks.At({ x, i })) {
					r = i - y - 1;
					break;
				}
			}
			return r;
		};

		// fill neighbors		// todo: custom neighbors support
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				// keyboard number area style
				// 7 8 9
				// 4 5 6
				// 1 2 3

				// [???]
				// [?.?]
				// [???]
				if (auto c5 = asg.At(x, y); c5->walkable) {
					auto c1 = asg.TryAt(x - 1, y + 1)->walkable;
					auto c2 = asg.TryAt(x, y + 1)->walkable;
					auto c3 = asg.TryAt(x + 1, y + 1)->walkable;
					auto c4 = asg.TryAt(x - 1, y)->walkable;
					auto c6 = asg.TryAt(x + 1, y)->walkable;
					auto c7 = asg.TryAt(x - 1, y - 1)->walkable;
					auto c8 = asg.TryAt(x, y - 1)->walkable;
					auto c9 = asg.TryAt(x + 1, y - 1)->walkable;

					xx::CoutN(x, ", ", y, '\n'
						, c7 ? '.' : 'B', c8 ? '.' : 'B', c9 ? '.' : 'B', '\n'
						, c4 ? '.' : 'B', 'c', c6 ? '.' : 'B', '\n'
						, c1 ? '.' : 'B', c2 ? '.' : 'B', c3 ? '.' : 'B'
					);

					// can't move( in the sky )
					// [???]
					// [?c?]
					// [?.?]
					if (c2) 
						continue;

					// can't move
					// [B?B]
					// [BcB]
					// [?B?]
					if (!c7 && !c9 && !c4 && !c6 && !c2) 
						continue;

					// can move to 4 or falling to 3 / bellow
					// [???]
					// [.c.]
					// [BB.]
					// ~~~~~
					// [??B]
					if (c4 && c6 && !c1 && !c2 && c3) {
						XYi nos[] = { { -1, 0 }, { 1, 0 } };
						if (auto fy = FindFallingOffsetY(x + nos[1].x, y); fy.has_value()) {
							nos[1].y = *fy;
							asg.InitCellNeighbors(c5, nos, 2);
						}
						else {
							asg.InitCellNeighbors(c5, nos, 1);
						}
						continue;
					}

					// can move to 6 or falling to 1 / bellow
					// [???]
					// [.c.]
					// [.BB]
					// ~~~~~
					// [B??]
					if (c4 && c6 && c1 && !c2 && !c3) {
						XYi nos[] = { { 1, 0 }, { -1, 0 } };
						if (auto fy = FindFallingOffsetY(x + nos[1].x, y); fy.has_value()) {
							nos[1].y = *fy;
							asg.InitCellNeighbors(c5, nos, 2);
						}
						else {
							asg.InitCellNeighbors(c5, nos, 1);
						}
						continue;
					}


					// can jump to 7
					// [..B]
					// [BcB]
					// [?B?]
					if (c7 && c8 && !c9 && !c4 && !c6 && !c2) {
						XYi no{ -1, -1 };
						asg.InitCellNeighbors(c5, &no, 1);
						continue;
					}

					// can jump to 9
					// [B..]
					// [BcB]
					// [?B?]
					if (!c7 && c8 && c9 && !c4 && !c6 && !c2) {
						XYi no{ 1, -1 };
						asg.InitCellNeighbors(c5, &no, 1);
						continue;
					}

					// can jump to 7 or 9
					// [...]
					// [BcB]
					// [?B?]
					if (c7 && c8 && c9 && !c4 && !c6 && !c2) {
						XYi nos[] = { { -1, -1 }, { 1, -1 } };
						asg.InitCellNeighbors(c5, nos, 2);
						continue;
					}

					// can jump to 7 or move to 6
					// [..?]
					// [Bc.]
					// [?BB]
					if (c7 && c8 && !c4 && c6 && !c2 && !c3) {
						XYi nos[] = { { -1, -1 }, { 1, 0 } };
						asg.InitCellNeighbors(c5, nos, 2);
						continue;
					}

					// can move to 4 or jump to 9
					// [?..]
					// [.cB]
					// [BB?]
					if (c8 && c9 && c4 && !c6 && !c1 && !c2) {
						XYi nos[] = { { -1, 0 }, { 1, -1 } };
						asg.InitCellNeighbors(c5, nos, 2);
						continue;
					}

					// can move to 4
					// [??B]
					// [.cB]
					// [BB?]
					if (!c9 && c4 && !c6 && !c1 && !c2) {
						XYi no{ -1, 0 };
						asg.InitCellNeighbors(c5, &no, 1);
						continue;
					}

					// can move to 6
					// [B??]
					// [Bc.]
					// [?BB]
					if (!c7 && !c4 && c6 && !c2 && !c3) {
						XYi no{ 1, 0 };
						asg.InitCellNeighbors(c5, &no, 1);
						continue;
					}

					// can move to 4 or 6
					// [???]
					// [.c.]
					// [BBB]
					if (c4 && c6 && !c1 && !c2 && !c3) {
						XYi nos[] = { { -1, 0 }, { 1, 0 } };
						asg.InitCellNeighbors(c5, nos, 2);
						continue;
					}

					// can falling to 3 / bellow
					// [B??]
					// [Bc.]
					// [?B.]
					// ~~~~~
					// [??B]
					if (!c7 && !c4 && c6 && !c2 && c3) {
						if (auto fy = FindFallingOffsetY(x + 1, y); fy.has_value()) {
							XYi no{ 1, *fy };
							asg.InitCellNeighbors(c5, &no, 1);
						}
						continue;
					}

					// can falling to 1 / bellow
					// [??B]
					// [.cB]
					// [.B?]
					// ~~~~~
					// [B??]
					if (!c9 && c4 && !c6 && c1 && !c2) {
						if (auto fy = FindFallingOffsetY(x - 1, y); fy.has_value()) {
							XYi no{ -1, *fy };
							asg.InitCellNeighbors(c5, &no, 1);
						}
						continue;
					}

					// can jump to 7 or falling to 3 / bellow
					// [..?]
					// [Bc.]
					// [?B.]
					// ~~~~~
					// [??B]
					if (c7 && c8 && !c4 && c6 && !c2 && c3) {
						XYi nos[] = { { -1, -1 }, { 1, 0 } };
						if (auto fy = FindFallingOffsetY(x + nos[1].x, y); fy.has_value()) {
							nos[1].y = *fy;
							asg.InitCellNeighbors(c5, nos, 2);
						}
						else {
							asg.InitCellNeighbors(c5, nos, 1);
						}
						continue;
					}

					// can jump to 9 or falling to 1 or bellow
					// [?..]
					// [.cB]
					// [.B?]
					// ~~~~~
					// [B??]
					if (c8 && c9 && c4 && !c6 && c1 && !c2) {
						XYi nos[] = { { 1, -1 }, { -1, 0 } };
						if (auto fy = FindFallingOffsetY(x + nos[1].x, y); fy.has_value()) {
							nos[1].y = *fy;
							asg.InitCellNeighbors(c5, nos, 2);
						}
						else {
							asg.InitCellNeighbors(c5, nos, 1);
						}
						continue;
					}

					// can falling to 1 / bellow or 3 / bellow
					// [???]
					// [.c.]
					// [.B.]
					// ~~~~~
					// [??B]
					if (c4 && c6 && c1 && !c2 && c3) {
						XYi nos[2];
						int32_t i{};
						if (auto fy = FindFallingOffsetY(x - 1, y); fy.has_value()) {
							nos[i++] = { -1, *fy };
						}
						if (auto fy = FindFallingOffsetY(x + 1, y); fy.has_value()) {
							nos[i++] = { 1, *fy };
						}
						if (i) {
							asg.InitCellNeighbors(c5, nos, i);
						}
						continue;
					}

				}
			}
		}

		{
			auto b = asg.Search(beginPos, endPos);
			auto s = asg.Dump(beginPos, endPos);
			xx::CoutN(s);

			// path to character actions
			if (auto pathLen = asg.path.len; pathLen > 1) {
				auto& cas = character->actions;
				cas.Reserve(pathLen - 1);
				for (auto i = pathLen - 2; i >= 0; --i) {
					auto c = asg.path[i];
					auto lc = asg.path[i + 1];
					auto dx = c->x - lc->x;
					auto dy = c->y - lc->y;
					if (dy == 0) {		// walk
						assert(dx != 0);
						auto& a = (Action_Move&)cas.Emplace();
						a.type = ActionTypes::Move;
						//a.timeoutFrameNumber = gLooper.frameNumber + Cfg::fps * 0.5f;
						a.tarCRIndex = { c->x, c->y };
					}
					else if (dy > 0) {	// jump
						assert(dx != 0);
						auto& a = (Action_Jump&)cas.Emplace();
						a.type = ActionTypes::Jump;
						//a.timeoutFrameNumber = gLooper.frameNumber + Cfg::fps * 0.5f;
						a.tarCRIndex = { c->x, c->y };
					}
					else {
						assert(dy < 0);	// fall
						assert(dx != 0);
						auto& a = (Action_Fall&)cas.Emplace();
						a.type = ActionTypes::Fall;
						//a.timeoutFrameNumber = gLooper.frameNumber + Cfg::fps * 0.5f;
						a.tarCRIndex = { c->x, c->y };
					}
				}
			}

			asg.Cleanup();
		}

		//auto secs = xx::NowEpochSeconds();
		//for (int i = 0; i < 10000000; ++i) {
		//	auto b = asg.Search(beginPos, endPos);
		//	//auto s = asg.Dump(beginPos, endPos);
		//	asg.Cleanup();
		//}
		//xx::CoutN(xx::NowEpochSeconds(secs));
	}

	inline void Scene::Update() {
		character->Update();
	}

	inline void Scene::Draw() {
		for (auto& o : blocks.items) o->Draw();
		character->Draw();
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(ai) mouse click for move char", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
