#pragma once

namespace Msgs {
	namespace Global {

        inline void Bullet_Base::WriteTo(xx::Data& d) const {
            Base::WriteTo(d);
            d.Write(
                damage, hitBlackList
            );
        }

        inline int32_t Bullet_Base::ReadFrom(xx::Data_r& dr) {
            if (auto r = Base::ReadFrom(dr); r) return r;
            return dr.Read(
                damage, hitBlackList
            );
        }

        inline void Bullet_Base::Init(Scene* scene_, XYp const& pos_, FX64 radians_, int32_t damage_) {
            assert(scene_->bullets.Empty() || scene_->bullets.Top().pointer != this);	// auto add check
            scene_->bullets.Emplace(xx::SharedFromThis(this));
            scene = xx::WeakFromThis(scene_);
            pos = pos_;
            radians = radians_;
            FillDirectionByRadians();
            damage = damage_;
        }

        inline void Bullet_Base::HitBlackListClear(int32_t pierceDelay_) {
            auto now = scene->frameNumber;
            auto newTime = now + pierceDelay_;
            for (auto i = hitBlackList.len - 1; i >= 0; --i) {
                if (hitBlackList[i].second < now) {
                    auto lastIndex = hitBlackList.len - 1;
                    hitBlackList[i] = hitBlackList[lastIndex];
                    hitBlackList.RemoveAt(lastIndex);
                }
            }
        }

        inline bool Bullet_Base::HitBlackListTryAdd(int32_t pierceDelay_, Monster* m) {
            auto listLen = hitBlackList.len;
            for (auto i = 0; i < listLen; ++i) {
                if (hitBlackList[i].first.GetPointer() == m) return false;
            }
            hitBlackList.Emplace(std::make_pair<xx::Weak<Monster>, int64_t>(
                xx::WeakFromThis(m), scene->frameNumber + pierceDelay_));
            return true;
        }

	}
}
