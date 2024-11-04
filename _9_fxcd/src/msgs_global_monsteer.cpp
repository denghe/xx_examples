#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

		int32_t Monster::ReadFrom(xx::Data_r& dr) {
			return dr.Read(
				scene, owner
				, pos, tarPos
				, radius, radians, frameIndex
			);
		}

		void Monster::WriteTo(xx::Data& d) const {
			d.Write(
				scene, owner
				, pos, tarPos
				, radius, radians, frameIndex
			);
		}

		Monster::~Monster() {
			if (_sgc) {
				_sgc->Remove(this);
			}
		}

		Monster* Monster::Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos) {
			assert(scene_->monsters.Empty() || scene_->monsters.Top().pointer != this);	// auto add check
			scene_->monsters.Emplace(xx::SharedFromThis(this));
			scene = xx::WeakFromThis(scene_);
			owner = owner_.ToWeak();

			pos = bornPos;
			tarPos = scene->mapSize - bornPos;		// todo: search nearest bullet?

			//radius = scene_->rnd.Next<int32_t>(16, 33);
			radius = 32;
			radians = scene_->rnd.NextRadians<FX64>();
			frameIndex.SetZero();

			_x = pos.x.ToInt();
			_y = pos.y.ToInt();
			_radius = radius.ToInt();
			scene->monsterSpace.Add(this);
			return this;
		}

		bool Monster::FillCrossInc(XYp const& pos_) {
			inc.Reset();

			auto p = pos_.As<int32_t>();
			scene->monsterSpace.Foreach9All<true>(p.x, p.y, [&](Monster* m)->bool {
				XYi d{ p.x - m->_x, p.y - m->_y };
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r2 = _radius + m->_radius;
				auto rr = r2 * r2;
				if (mag2 < rr) {	// cross?
					if (mag2 > 0) {
						auto mag = FX64{ mag2 }.SqrtFastest();	// todo: check zero ?
						auto v = d.As<FX64>() / mag;
						FX64 r{ r2 };
						auto s = (r - mag) / r * cMovementSpeed;
						inc += v * s;
					} else {
						auto r = scene->rnd.NextRadians<FX64>();
						inc.x += r.CosFastest() * cMovementSpeed;
						inc.y += r.SinFastest() * cMovementSpeed;
					}
				}
				return false;
			}, this);

			return true;
		}

		int32_t Monster::BlocksLimit(XYp& pos_) {
			auto& sg = scene->blockSpace;
			auto p = pos_.As<int32_t>();
			xx::FromTo<XYi> aabb{ p - _radius, p + _radius };	// p to aabb
			if (!sg.TryLimitAABB(aabb)) {
				return -1;	// bug?
			}
			bool changed{};
			sg.ForeachAABB(aabb);	// search
			for (auto b : sg.results) {
				if (b->PushOut(this, p)) {
					changed = true;
				}
			}
			sg.ClearResults();	// after search
			if (p.IsOutOfEdge(Msgs::Global::Scene::mapSize)) {
				return -2;	// bug?
			}
			if (changed) {
				pos_.x = p.x;
				pos_.y = p.y;
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
			auto d = tarPos - pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			XYp v{};
			if (mag2 > FX64_1) {
				v = d * mag2.RSqrtFastest() * cMovementSpeed;
			}

			if (FillCrossInc(pos)) {
				assert(inc.x < scene->maxDistance);
				assert(inc.y < scene->maxDistance);
				assert(inc.x * inc.x + inc.y * inc.y <= Msgs::Global::Scene::maxDistance);
				newPos = pos + inc + v;
			} else {
				newPos = pos + v;
			}

			// map edge protect
			scene->PosLimitByMapSize(newPos);

			// block push
			for (int i = 0; i < 3; ++i) {
				if (auto r = BlocksLimit(newPos); r < 0)
					return r;
				else if (r == 0) break;
			}

			return 0;
		}

		int32_t Monster::Update2() {
			if (newPos == pos) return 0;
			pos = newPos;

			// update space index
			_x = pos.x.ToInt();
			_y = pos.y.ToInt();
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
			q.pos = gLooper.camera.ToGLPos(pos.As<float>());
			q.anchor = *frame->anchor;
			q.scale = (radius / cResRadius).ToFloat() * gLooper.camera.scale;
			q.radians = radians.ToFloat();
			q.colorplus = 1;
			q.color = color;
			q.texRect.data = frame->textureRect.data;
		}

	}
}
