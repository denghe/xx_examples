#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

        void Block::WriteTo(xx::Data& d) const {
            d.Write(
                scene, pos, halfSize, (uint8_t&)wayout, _aabb.from, _aabb.to
            );
        }

        int32_t Block::ReadFrom(xx::Data_r& dr) {
            return dr.Read(
                scene, pos, halfSize, (uint8_t&)wayout, _aabb.from, _aabb.to
            );
        }

        Block::~Block() {
            if (_sgc) {
                _sgc->Remove(this);
            }
        }

        void Block::Init(Scene* scene_, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY) {
            scene = xx::WeakFromThis(scene_);
            scene_->blocks.Emplace(xx::SharedFromThis(this));
            _aabb.from.x = minX;
            _aabb.from.y = minY;
            _aabb.to.x = maxX;
            _aabb.to.y = maxY;
            auto siz = _aabb.to - _aabb.from;
            halfSize = siz / 2;
            pos = _aabb.from + halfSize;
            scene_->blockSpace.Add(this);
        }

        void Block::FillWayout() {
            // search neighbor & set wayout bit
            auto& bs = scene->blockSpace;
            (uint8_t&)wayout = 0;
            auto left = bs.ExistsPoint(_aabb.from + XYi{ -5, 0 });
            auto up = bs.ExistsPoint(_aabb.from + XYi{ 0, -5 });
            auto right = bs.ExistsPoint(_aabb.to + XYi{ 5, 0 });
            auto down = bs.ExistsPoint(_aabb.to + XYi{ 0, 5 });
            if (left > 0 && up > 0 && right > 0 && down > 0) {
                wayout.left = left == 1;
                wayout.up = up == 1;
                wayout.right = right == 1;
                wayout.down = down == 1;
            } else {
                wayout.left = left == 0;
                wayout.up = up == 0;
                wayout.right = right == 0;
                wayout.down = down == 0;
            }
        }

        bool Block::IntersectCircle(XYi const& p, int32_t radius) {
            return xx::Calc::Intersects::BoxCircle<int32_t>(pos.x, pos.y, halfSize.x, halfSize.y, p.x, p.y, radius);
        }

        bool Block::PushOut(Monster* m, XYi& mp) {
            assert(m);
            auto idx = (uint8_t&)wayout;
            assert(idx < _countof(xx::TranslateControl::pushOutFuncsInt32));
            return xx::TranslateControl::pushOutFuncsInt32[idx](pos.x, pos.y, halfSize.x, halfSize.y, mp.x, mp.y, m->_radius);
        }

        void Block::Draw() {
            auto& frame = *gRes.quad;
            auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
            q.pos = (XY{ pos } - Msgs::Global::Scene::mapSize_2f) * gLooper.camera.scale;
            q.anchor = { 0.5f, 0.5f };
            q.scale = XY{ halfSize } / 32.f * gLooper.camera.scale;
            q.radians = 0;
            q.colorplus = 1;
            q.color = { 55,55,55,255 };
            q.texRect.data = frame.textureRect.data;
        }

	}
}
