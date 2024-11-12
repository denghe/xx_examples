#pragma once

namespace Msgs {
	namespace Global {

        inline void Block::WriteTo(xx::Data& d) const {
            d.Write(
                scene, pos, halfSize, (uint8_t&)wayout, isMapCorner, _aabb.from, _aabb.to
            );
        }

        inline  int32_t Block::ReadFrom(xx::Data_r& dr) {
            return dr.Read(
                scene, pos, halfSize, (uint8_t&)wayout, isMapCorner, _aabb.from, _aabb.to
            );
        }

        inline Block::~Block() {
            if (_sgc) {
                _sgc->Remove(this);
            }
        }

        inline Block& Block::Init(Scene* scene_, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY) {
            assert(scene_->blocks.Empty() || scene_->blocks.Top().pointer != this);	// auto add check
            scene_->blocks.Emplace(xx::SharedFromThis(this));
            scene = xx::WeakFromThis(scene_);
            _aabb.from.x = minX;
            _aabb.from.y = minY;
            _aabb.to.x = maxX;
            _aabb.to.y = maxY;
            auto siz = _aabb.to - _aabb.from;
            halfSize = siz / 2;
            pos = _aabb.from + halfSize;
            scene_->blockSpace.Add(this);
            return *this;
        }

        inline Block& Block::Init(Scene* scene_, XYi const& pos_, XYi const& siz_) {
            auto half = siz_ / 2;
            assert(half * 2 == siz_);
            return Init(scene_, pos_.x - half.x, pos_.y - half.y, pos_.x + half.x, pos_.y + half.y);
        }

        inline void Block::FillWayout() {
            // search neighbor & set wayout bit
            auto& bs = scene->blockSpace;
            (uint8_t&)wayout = 0;
            static constexpr int dx{ 5 }, dy{ 5 };
            auto left = bs.ExistsPoint(_aabb.from + XYi{ -dx, dy });
            auto up = bs.ExistsPoint(_aabb.from + XYi{ dy, -dx });
            auto right = bs.ExistsPoint(_aabb.to + XYi{ dx, -dy });
            auto down = bs.ExistsPoint(_aabb.to + XYi{ -dy, dx });

            isMapCorner = left == 2 && up == 2
                || up == 2 && right == 2
                || right == 2 && down == 2
                || down == 2 && left == 2;

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

        inline  void Block::AuthWayout(xx::Math::BlockWayout bw) {
            assert((uint8_t&)bw == (uint8_t&)wayout);
        }

        inline bool Block::IntersectCircle(XYi const& p, int32_t radius) {
            return xx::Calc::Intersects::BoxCircle<int32_t>(pos.x, pos.y, halfSize.x, halfSize.y, p.x, p.y, radius);
        }

        inline bool Block::PushOut(Monster* m, XYi& mp) {
            assert(m);
            return xx::Math::MoveCircleIfIntersectsBox<int32_t>(wayout, pos.x, pos.y, halfSize.x, halfSize.y, mp.x, mp.y, m->_radius, isMapCorner);
        }

        inline  void Block::Draw() {
            auto& frame = *gRes.quad;
            auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
            q.pos = gLooper.camera.ToGLPos(pos.As<float>());
            q.anchor = { 0.5f, 0.5f };
            q.scale = XY{ halfSize } / 32.f * gLooper.camera.scale;
            q.radians = 0;
            q.colorplus = 1;
            q.color = { 55,55,55,255 };
            q.texRect.data = frame.textureRect.data;
        }

	}
}
