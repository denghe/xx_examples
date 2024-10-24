﻿#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

		int32_t Monster::ReadFrom(xx::Data_r& dr) {
			return dr.Read(
				scene, owner
				, x, y, radius, radians, frameIndex
				, incX, incY
				, timeout, runawayMode
			);
		}

		void Monster::WriteTo(xx::Data& d) const {
			d.Write(
				scene, owner
				, x, y, radius, radians, frameIndex
				, incX, incY
				, timeout, runawayMode
			);
		}

		Monster::~Monster() {
			if (_sgc) {
				_sgc->Remove(this);
			}
		}

		Monster* Monster::Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos) {
			scene_->monsters.Emplace(xx::SharedFromThis(this));
			scene = xx::WeakFromThis(scene_);
			owner = owner_.ToWeak();

			x = bornPos.x;
			y = bornPos.y;
			//radius = scene_->rnd.Next<int32_t>(16, 33);
			radius = 32;
			radians = FX64{ scene_->rnd.Next<int32_t>(-c314159, c314159) } * c1_100000;
			frameIndex.SetZero();

			_x = x.ToInt();
			_y = y.ToInt();
			_radius = radius.ToInt();
			scene->monsterSpace.Add(this);
			return this;
		}

		bool Monster::FillCrossInc() {
			incX.SetZero();
			incY.SetZero();

			scene->monsterSpace.Foreach9All<true>(_x, _y, [&](Monster* m)->bool {
				auto dx = _x - m->_x;
				auto dy = _y - m->_y;
				auto mag2 = dx * dx + dy * dy;
				auto r2 = _radius + m->_radius;
				auto rr = r2 * r2;
				if (mag2 < rr) {	// cross?
					if (mag2 > 0) {
						auto mag = FX64{ mag2 }.SqrtFastest();	// todo: check zero ?
						auto vx = FX64{ dx } / mag;
						auto vy = FX64{ dy } / mag;
						auto r = FX64{ r2 };
						auto s = (r - mag) / r * cMovementSpeed2;
						incX += vx * s;
						incY += vy * s;
					} else {
						auto r = FX64{ scene->rnd.Next<int32_t>(-c314159, c314159) } *c1_100000;
						incX += r.CosFastest() * cMovementSpeed2;
						incY += r.SinFastest() * cMovementSpeed2;
					}
				}
				return false;
			}, this);

			// todo: deactive logic
			
			return true;
		}

		int Monster::BlocksLimit() {
			auto& sg = scene->blockSpace;
			XYi pos{ x.ToInt(), y.ToInt() };
			xx::FromTo<XYi> aabb{ pos - _radius, pos + _radius };	// pos to aabb
			if (!sg.TryLimitAABB(aabb)) {
				return -1;	// bug?
			}
			bool changed{};
			sg.ForeachAABB(aabb);	// search
			for (auto b : sg.results) {
				if (b->PushOut(this, pos)) {
					changed = true;
				}
			}
			sg.ClearResults();	// after search
			if (pos.IsOutOfEdge(Msgs::Global::Scene::mapSize)) {
				return -2;	// bug?
			}
			if (changed) {
				x = pos.x;
				y = pos.y;
				return 1;
			}
			return 0;
		}

		int32_t Monster::Update() {
			frameIndex = frameIndex + cFrameIndexStep;
			if (frameIndex >= cFrameIndexMax) {
				frameIndex = frameIndex - cFrameIndexMax;
			}

			// backup
			auto ox = x;
			auto oy = y;
			auto oradius = radius;
			int32_t incCalcLine{};

			// get round count and find move wayout
			auto count = scene->monsterSpace.cells[_sgcIdx].count;
			if (count > cNeighbourMaxCount) {
				if (!runawayMode) {
					runawayMode = true;
					timeout = scene->frameNumber + cTimeout;
					auto r = FX64{ scene->rnd.Next<int32_t>(-c314159, c314159) } * c1_100000;
					incX = r.CosFastest() * cMovementSpeed3;
					incY = r.SinFastest() * cMovementSpeed3;
					incCalcLine = __LINE__;
				}
				x += incX;
				y += incY;
				if (timeout < scene->frameNumber) {
					runawayMode = false;
				}
			} else {
				runawayMode = false;
				if (FillCrossInc()) {
					// value edge protect
					if (incX < cMovementSpeed3n) incX = cMovementSpeed3n;
					else if (incX > cMovementSpeed3) incX = cMovementSpeed3;
					if (incY < cMovementSpeed3n) incY = cMovementSpeed3n;
					else if (incY > cMovementSpeed3) incY = cMovementSpeed3;

					// distance limit protect
					if (incX * incX + incY * incY > cMovementSpeed3Pow2) {
						auto r = FX64::Atan2Fastest(incY, incX);
						incX = r.CosFastest() * cMovementSpeed3;
						incY = r.SinFastest() * cMovementSpeed3;
						incCalcLine = __LINE__;
					} else {
						incCalcLine = __LINE__;
					}
					x += incX;
					y += incY;
				}
			}

			// map edge protect
			if (x < 0)
				x = 0;
			else if (x >= Msgs::Global::Scene::mapSizeX)
				x = Msgs::Global::Scene::mapSizeX;
			if (y < 0) 
				y = 0;
			else if (y >= Msgs::Global::Scene::mapSizeY)
				y = Msgs::Global::Scene::mapSizeY;

			// block push
			for (int i = 0; i < 3; ++i) {
				if (auto r = BlocksLimit(); r < 0)
					return r;
				else if (r == 0) break;
			}

			// changed? update space index
			if (ox != x || oy != y || oradius != radius) {
				_x = x.ToInt();
				_y = y.ToInt();
				_radius = radius.ToInt();
				scene->monsterSpace.Update(this);
			}
			return 0;
		}

		void Monster::Draw() {
			static constexpr xx::RGBA8 colors[] = { xx::RGBA8_Yellow, xx::RGBA8_Red, xx::RGBA8_Green, xx::RGBA8_Blue };
			xx::RGBA8 color;
			if (owner) {
				color = colors[owner->clientId & 0b11];
			} else {
				color = xx::RGBA8_White;
			}

			auto& frame = gRes.monster_[frameIndex.ToInt()];
			auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame->tex->GetValue(), 1);
			q.pos = (XY{ x.ToFloat(), y.ToFloat() } - Msgs::Global::Scene::mapSize_2f) * gLooper.camera.scale;
			q.anchor = *frame->anchor;
			q.scale = (radius / cResRadius).ToFloat() * gLooper.camera.scale;
			q.radians = radians.ToFloat();
			q.colorplus = 1;
			q.color = color;
			q.texRect.data = frame->textureRect.data;
		}

	}
}
