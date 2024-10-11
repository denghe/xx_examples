#include "pch.h"
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
			radius = scene_->rnd.Next<int32_t>(32, 65);
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

		bool Monster::Update() {
			frameIndex = frameIndex + cFrameIndexStep;
			if (frameIndex >= cFrameIndexMax) {
				frameIndex = frameIndex - cFrameIndexMax;
			}

			auto ox = x;
			auto oy = y;

			// get round count and find move wayout
			auto count = scene->monsterSpace.cells[_sgcIdx].count;
			if (count > cNeighbourMaxCount) {
				if (!runawayMode) {
					runawayMode = true;
					timeout = scene->frameNumber + cTimeout;
					auto r = FX64{ scene->rnd.Next<int32_t>(-c314159, c314159) } * c1_100000;
					incX = r.CosFastest() * cMovementSpeed3;
					incY = r.SinFastest() * cMovementSpeed3;
				}
				x += incX;
				y += incY;
				if (timeout < scene->frameNumber) {
					runawayMode = false;
				}
			} else {
				runawayMode = false;
				if (FillCrossInc()) {
					x += incX;
					y += incY;
				}
			}

			if (ox != x || oy != y) {
				_x = x.ToInt();
				_y = y.ToInt();
				_radius = radius.ToInt();
				scene->monsterSpace.Update(this);
			}
			return false;
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
			q.pos = (XY{ x.ToFloat(), y.ToFloat() } - scene->monsterSpace.max / 2) * gLooper.camera.scale;
			q.anchor = *frame->anchor;
			q.scale = (radius / cResRadius).ToFloat() * gLooper.camera.scale;
			q.radians = radians.ToFloat();
			q.colorplus = 1;
			q.color = color;
			q.texRect.data = frame->textureRect.data;
		}

	}
}
