#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

		int32_t Monster::ReadFrom(xx::Data_r& dr) {
			return dr.Read(
				scene, owner
				, x, y, radius, radians, frameIndex
				, tarX, tarY
			);
		}

		void Monster::WriteTo(xx::Data& d) const {
			d.Write(
				scene, owner
				, x, y, radius, radians, frameIndex
				, tarX, tarY
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

			auto offsetBornPos = scene->mapSize_2 - bornPos;
			auto tarPos = scene->mapSize_2 + offsetBornPos;

			x = bornPos.x;
			y = bornPos.y;

			tarX = tarPos.x;
			tarY = tarPos.y;

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

		bool Monster::FillCrossInc(FX64 const& x_, FX64 const& y_) {
			incX.SetZero();
			incY.SetZero();

			auto px = x_.ToInt();
			auto py = y_.ToInt();
			scene->monsterSpace.Foreach9All<true>(px, py, [&](Monster* m)->bool {
				auto dx = px - m->_x;
				auto dy = py - m->_y;
				auto mag2 = dx * dx + dy * dy;
				auto r2 = _radius + m->_radius;
				auto rr = r2 * r2;
				if (mag2 < rr) {	// cross?
					if (mag2 > 0) {
						auto mag = FX64{ mag2 }.SqrtFastest();	// todo: check zero ?
						auto vx = FX64{ dx } / mag;
						auto vy = FX64{ dy } / mag;
						auto r = FX64{ r2 };
						auto s = (r - mag) / r * cMovementSpeed;
						incX += vx * s;
						incY += vy * s;
					} else {
						auto r = FX64{ scene->rnd.Next<int32_t>(-c314159, c314159) } *c1_100000;
						incX += r.CosFastest() * cMovementSpeed;
						incY += r.SinFastest() * cMovementSpeed;
					}
				}
				return false;
			}, this);

			return true;
		}

		int32_t Monster::BlocksLimit(FX64& x_, FX64& y_) {
			auto& sg = scene->blockSpace;
			XYi pos{ x_.ToInt(), y_.ToInt() };
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
				x_ = pos.x;
				y_ = pos.y;
				return 1;
			}
			return 0;
		}

		int32_t Monster::Update1() {
			frameIndex = frameIndex + cFrameIndexStep;
			if (frameIndex >= cFrameIndexMax) {
				frameIndex = frameIndex - cFrameIndexMax;
			}

			// make move to tar's vect
			auto dx = tarX - x;
			auto dy = tarY - y;
			auto mag2 = dx * dx + dy * dy;
			FX64 vx{}, vy{};
			if (mag2 > FX64::One) {
				auto _1_mag_s = mag2.RSqrtFastest() * cMovementSpeed;
				vx = dx *_1_mag_s;
				vy = dy *_1_mag_s;
			}

			if (FillCrossInc(x, y)) {
				assert(incX < scene->limitFX64);
				assert(incY < scene->limitFX64);
				assert(incX * incX + incY * incY <= cMovementSpeed3Pow2m100);
				newX = x + incX + vx;
				newY = y + incY + vy;
			} else {
				newX = x + vx;
				newY = y + vy;
			}

			// map edge protect
			if (newX < FX64::Zero)
				newX = FX64::Zero;
			else if (newX >= Msgs::Global::Scene::mapSizeX)
				newX = Msgs::Global::Scene::mapSizeX;
			if (newY < FX64::Zero)
				newY = FX64::Zero;
			else if (newY >= Msgs::Global::Scene::mapSizeY)
				newY = Msgs::Global::Scene::mapSizeY;

			// block push
			for (int i = 0; i < 3; ++i) {
				if (auto r = BlocksLimit(newX, newY); r < 0)
					return r;
				else if (r == 0) break;
			}

			return 0;
		}

		int32_t Monster::Update2() {
			if (newX == x && newY == y) return 0;

			//auto bakIncX = incX;
			//auto bakIncY = incY;
			//if (FillCrossInc(newX, newY)) {
			//	assert(incX < scene->limitFX64);
			//	assert(incY < scene->limitFX64);
			//	assert(incX * incX + incY * incY <= cMovementSpeed3Pow2);
			//	if (incX * incX + incY * incY < bakIncX * bakIncX + bakIncY + bakIncY) {
			//		x = newX;
			//		y = newY;
			//	} else return 0;
			//} else {
				x = newX;
				y = newY;
			//}

			// update space index
			_x = x.ToInt();
			_y = y.ToInt();
			_radius = radius.ToInt();
			scene->monsterSpace.Update(this);
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
