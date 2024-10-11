#include "pch.h"
#include "looper.h"
#include "msgs.h"

namespace Msgs {
	namespace Global {

        void Block::WriteTo(xx::Data& d) const {
            d.Write(
                scene, pos, halfSize, (uint8_t&)wayout
            );
        }

        int32_t Block::ReadFrom(xx::Data_r& dr) {
            return dr.Read(
                scene, pos, halfSize, (uint8_t&)wayout
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
            auto left = bs.ExistsPoint(_aabb.from + XYi{ -1, 0 });
            auto up = bs.ExistsPoint(_aabb.from + XYi{ 0, -1 });
            auto right = bs.ExistsPoint(_aabb.to + XYi{ 1, 0 });
            auto down = bs.ExistsPoint(_aabb.to + XYi{ 0, 1 });
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

        XX_INLINE bool Block::IntersectCircle(XYi const& p, int32_t radius) {
            return xx::Calc::Intersects::BoxCircle<int32_t>(pos.x, pos.y, halfSize.x, halfSize.y, p.x, p.y, radius);
        }

        XX_INLINE void Block::PushOut(Monster* m) {
            assert(m);
            auto idx = (uint8_t&)wayout;
            assert(idx < _countof(xx::TranslateControl::pushOutFuncsInt32));
            xx::TranslateControl::pushOutFuncsInt32[idx](pos.x, pos.y, halfSize.x, halfSize.y, m->_x, m->_y, m->_radius);
        }

        XX_INLINE void Block::Draw() {
            auto& f = *gRes.quad;
            auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(f.tex->GetValue(), 1);
            q.pos = gLooper.camera.ToGLPos(_aabb.from);
            q.anchor = { 0, 1 };
            q.scale = (_aabb.to - _aabb.from) / 64 * gLooper.camera.scale;
            q.radians = 0;
            q.colorplus = 1;
            q.color = { 55,55,55,255 };
            q.texRect.data = f.textureRect.data;
        }

	}
}
