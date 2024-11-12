#pragma once

namespace Msgs {
	namespace Global {

        inline void Projectile::WriteTo(xx::Data& d) const {
            d.Write(
                scene, pos, radians
            );
        }

        inline int32_t Projectile::ReadFrom(xx::Data_r& dr) {
            if (auto r = dr.Read(
                scene, pos, radians
            ); r) return r;
            FillDirectionByRadians();
            return 0;
        }

        inline void Projectile::FillDirectionByRadians() {
            direction.x = radians.CosFastest();
            direction.y = radians.SinFastest();
        }

        /*********************************************************************************************/

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

        /*********************************************************************************************/

        inline void Bullet_Sector::WriteTo(xx::Data& d) const {
            Base::WriteTo(d);
            d.Write(
                radius, theta
            );
        }

        inline int32_t Bullet_Sector::ReadFrom(xx::Data_r& dr) {
            if (auto r = Base::ReadFrom(dr); r) return r;
            return dr.Read(
                radius, theta
            );
        }

        // todo: damage args
        inline Bullet_Sector& Bullet_Sector::Init(Scene* scene_, XYp const& pos_, FX64 radians_, FX64 radius_, FX64 theta_) {
            Base::Init(scene_, pos_, radians_, 5);
            radius = radius_;
            theta = theta_;
            return *this;
        }

        inline int32_t Bullet_Sector::Update() {
            // rotate
            radians += cRotateStep;
            if (radians > FX64_PI) {
                radians = FX64_N2PI + radians;
            }
            FillDirectionByRadians();
            auto& monsters = scene->monsters;

            HitBlackListClear(cPierceDelay);

#if 1
            // search monster by range
            scene->monsterSpace.ForeachByRange(gLooper.sgrdd, pos.x.ToInt(), pos.y.ToInt(), radius.ToInt(), [&](Monster* m) {
                if (xx::Math::IsSectorCircleIntersect<XYp>(pos, radius, direction, theta, m->pos, m->radius)) {
                    if (HitBlackListTryAdd(cPierceDelay, m)) {
                        m->Hurt(this);
                    }
                }
            });
#else
            // foreach monster
            for (int32_t i = monsters.len - 1; i >= 0; --i) {
                auto& m = monsters[i];
                if (xx::Math::IsSectorCircleIntersect<XYp>(pos, radius, direction, theta, m->pos, m->radius)) {
                    if (HitBlackListTryAdd(cPierceDelay, m)) {
                        m->Hurt(this);
                    }
                }
            }
#endif

            return 0;
        }

        inline void Bullet_Sector::Draw() {
            auto p = gLooper.camera.ToGLPos(pos.As<float>());
            auto r = radius.ToFloat();
            auto u = radians.ToFloat();
            auto a = theta.ToFloat();

            xx::LineStrip{}.FillSectorPoints({}, r, u, a, 5)
                .SetPosition(p)
                .SetScale(gLooper.camera.scale)
                .SetRotate(radians.ToFloat())
                .Draw();
        }

        /*********************************************************************************************/

        inline void Bullet_Box::WriteTo(xx::Data& d) const {
            Base::WriteTo(d);
            d.Write(
                size
            );
        }

        inline int32_t Bullet_Box::ReadFrom(xx::Data_r& dr) {
            if (auto r = Base::ReadFrom(dr); r) return r;
            return dr.Read(
                size
            );
        }

        inline Bullet_Box& Bullet_Box::Init(Scene* scene_, XYp const& pos_, FX64 radians_, XYp const& size_) {
            Base::Init(scene_, pos_, radians_, 5);
            size = size_;
            return *this;
        }

        inline int32_t Bullet_Box::Update() {
            // rotate
            radians += cRotateStep;
            if (radians > FX64_PI) {
                radians = FX64_N2PI + radians;
            }
            FillDirectionByRadians();
            auto& monsters = scene->monsters;

            HitBlackListClear(cPierceDelay);

            auto radius = size.x.ToInt();   // todo: + min(size.x, size.y) / 2 * 1.4 ?

#if 1
            // search monster by range
            scene->monsterSpace.ForeachByRange(gLooper.sgrdd, pos.x.ToInt(), pos.y.ToInt(), radius, [&](Monster* m) {
                if (xx::Math::IsBoxCircleIntersect<XYp>(pos, size, direction, m->pos, m->radius)) {
                    if (HitBlackListTryAdd(cPierceDelay, m)) {
                        m->Hurt(this);
                    }
                }
            });
#else
            for (int32_t i = monsters.len - 1; i >= 0; --i) {
                auto& m = monsters[i];
                if (xx::Math::IsBoxCircleIntersect<XYp>(pos, size, direction, m->pos, m->radius)) {
                    if (HitBlackListTryAdd(cPierceDelay, m)) {
                        m->Hurt(this);
                    }
                }
            }
#endif

            return 0;
        }

        inline void Bullet_Box::Draw() {
            auto p = gLooper.camera.ToGLPos(pos.As<float>());
            auto wh = size.As<float>();
            xx::LineStrip{}.FillBoxPoints({}, wh)
                .SetPosition(p)
                .SetScale(gLooper.camera.scale)
                .SetRotate(radians.ToFloat())
                .Draw();
        }

	}
}
