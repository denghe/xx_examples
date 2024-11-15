#pragma once

namespace Msgs {
	namespace Global {

		inline int32_t Monster::ReadFrom(xx::Data_r& dr) {
			return dr.Read(
				scene, owner
				, indexAtContainer
				, pos, tarPos
				, radius, radians, frameIndex
				, changeColorToWhiteElapsedTime

				, cfg
				, items
				, level, experience
				, life, energy
				, sp
			);
		}

		inline void Monster::WriteTo(xx::Data& d) const {
			d.Write(
				scene, owner
				, indexAtContainer
				, pos, tarPos
				, radius, radians, frameIndex
				, changeColorToWhiteElapsedTime

				, cfg
				, items
				, level, experience
				, life, energy
				, sp
			);
		}

		inline Monster::~Monster() {
			assert(scene);
			assert(scene->disposing || indexAtContainer == -1);
			if (_sgc) {
				_sgc->Remove(this);
			}
		}

		// auto sync indexAtContainer. swap remove with last
		inline void Monster::Kill() {
			xx::RemoveFrom(scene->monsters, indexAtContainer);
		}

		inline Monster* Monster::Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos) {
			AddTo(scene_->monsters, xx::SharedFromThis(this));
			scene = xx::WeakFromThis(scene_);
			owner = owner_.ToWeak();

			pos = bornPos;
			tarPos = scene->mapSize - bornPos;		// todo: search nearest bullet?

			//radius = scene_->rnd.Next<int32_t>(16, 33);
			radius = 32;
			radians = scene_->rnd.NextRadians<FX64>();
			frameIndex.SetZero();

			// todo: make Item( this, ...
			cfg = scene_->monsterDefaultConfig;
			level = 1;
			experience = 0;
			spSetDirty();
			UpdateSP();
			//life = sp.life;
			life = scene_->rnd.Next<int32_t>(1, sp.life.ToInt() + 1);
			energy = sp.energy;
			// ...

			_x = pos.x.ToInt();
			_y = pos.y.ToInt();
			_radius = radius.ToInt();
			scene->monsterSpace.Add(this);
			return this;
		}

		XX_INLINE void Monster::spSetDirty() {
			udByte1 = 1;
		}

		XX_INLINE void Monster::spClearDirty() {
			udByte1 = 0;
		}

		XX_INLINE bool Monster::spIsDirty() const {
			return udByte1;
		}

		inline void Monster::UpdateSP() {
			if (spIsDirty()) {
				spClearDirty();
				sp.Clear();
				// calculate points by level
				sp.health = cfg->initHealth + (this->level - 1) * cfg->levelToHealthRatio;
				sp.vitality = cfg->initVitality + (this->level - 1) * cfg->levelToVitalityRatio;
				sp.strength = cfg->initStrength + (this->level - 1) * cfg->levelToStrengthRatio;
				sp.dexterity = cfg->initDexterity + (this->level - 1) * cfg->levelToDexterityRatio;
				sp.defense = cfg->initDefense + (this->level - 1) * cfg->levelToDefenseRatio;
				sp.wisdom = cfg->initWisdom + (this->level - 1) * cfg->levelToWisdomRatio;
				sp.lucky = cfg->initLucky + (this->level - 1) * cfg->levelToLuckyRatio;
				// gather points & results from items
				if (auto itemsCount = items.Count(); itemsCount > 0) {
					for (int32_t ei = 0; ei < itemsCount; ++ei) {
						auto& e = items[ei];
						if (auto statsCount = e->stats.Count(); statsCount > 0) {
							for (int32_t si = 0; si < statsCount; ++si) {
								auto& s = e->stats[si];
								sp.At(s.type) += s.value;
							}
						}
					}
				}
				// calculate final results
				sp.life += sp.health * cfg->healthToLifeRatio;
				sp.energy += sp.health * cfg->healthToEnergyRatio;
				sp.lifeRegeneration += sp.vitality * cfg->vitalityToLifeRegenerationRatio;
				sp.energyRegeneration += sp.vitality * cfg->vitalityToEnergyRegenerationRatio;
				sp.damageScale += sp.strength * cfg->strengthToDamageScaleRatio;
				sp.defenseScale += cfg->defenseFactor / (cfg->defenseFactor + sp.defense);
				sp.evasion += cfg->evasionFactor / (cfg->evasionFactor + sp.dexterity);
				sp.movementSpeed += cfg->baseMovementSpeed + sp.dexterity * cfg->dexterityToMovementSpeedRatio;
				sp.experienceScale += sp.wisdom * cfg->wisdomToExperienceScaleRatio;
				sp.criticalChance += sp.lucky * cfg->luckyToCritialChanceScaleRatio;
				sp.criticalBonus += sp.lucky * cfg->luckyToCritialBonusScaleRatio;
				// handle value range
				if (sp.criticalChance > 1) sp.criticalBonus += sp.criticalChance - 1;
				if (sp.movementSpeed >= 1000) sp.movementSpeed = 1000;
				if (sp.defenseScale >= 1) sp.defenseScale = 0.99999;
				if (sp.evasion >= 1) sp.evasion = 0.99999;
			}
			// handle regenerations
			if (life < sp.life) {
				life += sp.lifeRegeneration;
				if (life > sp.life) life = sp.life;
			}
			if (energy < sp.energy) {
				energy += sp.energyRegeneration;
				if (energy > sp.energy) energy = sp.energy;
			}
		}

		inline void Monster::Update1() {
			UpdateSP();

			// make move to tar's vect
			auto d = tarPos - pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			FX64 _1_mag{};
			XYp v{};
			if (mag2 > FX64_1) {
				_1_mag = mag2.RSqrtFastest();
				v = d * _1_mag * cMovementSpeed;
			}
			
			// combine move force
			if (FillCrossInc(pos)) {
				assert(inc.x < scene->maxDistance);
				assert(inc.y < scene->maxDistance);
				assert(inc.x * inc.x + inc.y * inc.y <= Msgs::Global::Scene::maxDistance);
				newPos = pos + inc + v;
			} else {
				if (_1_mag.IsZero() || _1_mag > c1_MovementSpeed) {
					newPos = tarPos;
				} else {
					newPos = pos + v;
				}
			}

			// map edge protect
			scene->PosLimitByMapSize(newPos);

			// block push
			for (int i = 0; i < 3; ++i) {
				if (auto r = BlocksLimit(newPos); r < 0) {
					assert(false);
					newPos = pos;
					break;
				} else if (r == 0) break;
			}
		}

		inline int32_t Monster::Update2() {
			if (newPos != pos) {
				// frame animation step
				frameIndex = frameIndex + cFrameIndexStep/* todo: * moveDistance? */;
				while (frameIndex >= cFrameIndexMax) {
					frameIndex = frameIndex - cFrameIndexMax;
				}

				pos = newPos;

				// update space index
				_x = pos.x.ToInt();
				_y = pos.y.ToInt();
				_radius = radius.ToInt();
				scene->monsterSpace.Update(this);
			}
			return 0;
		}

		inline void Monster::Draw() {
			static constexpr xx::RGBA8 colors[] = { xx::RGBA8_Yellow, xx::RGBA8_Green, xx::RGBA8_Blue, xx::RGBA8_Red };
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
			q.colorplus = scene->frameNumber > changeColorToWhiteElapsedTime ? cColorPlusDefault : cColorPlusWhite;
			q.color = color;
			q.texRect.data = frame->textureRect.data;
		}

		inline void Monster::DrawBars() {
			auto& f = *gRes.quad;
			auto qs = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(f.tex->GetValue(), 2);
			auto p = pos.As<float>();
			auto hpBgPos = gLooper.camera.ToGLPos(p + XY{ -32, -39 });
			auto hpFgPos = gLooper.camera.ToGLPos(p + XY{ -31, -39 });
			auto bgScale = gLooper.camera.scale * XY{ 1, 0.1 };
			auto hpFgScale = gLooper.camera.scale * XY{ (float)life.ToInt() / sp.life.ToInt() * (62.f / 64.f), 0.1f * (4.4f / 6.4f) };
			{
				auto& q = qs[0];
				q.pos = hpBgPos;
				q.anchor = { 0, 0.5f };
				q.scale = bgScale;
				q.radians = 0;
				q.colorplus = 1;
				q.color = xx::RGBA8_Black;
				q.texRect.data = f.textureRect.data;
			}
			{
				auto& q = qs[1];
				q.pos = hpFgPos;
				q.anchor = { 0, 0.5f };
				q.scale = hpFgScale;
				q.radians = 0;
				q.colorplus = 1;
				q.color = xx::RGBA8_Red;
				q.texRect.data = f.textureRect.data;
			}
		}

		inline bool Monster::FillCrossInc(XYp const& pos_) {
			inc.Reset();
			int32_t count{};

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
					++count;
				}
				return false;
			}, this);

			return count > 0;
		}

		inline int32_t Monster::BlocksLimit(XYp& pos_) {
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

		inline bool Monster::Hurt(Bullet_Base* bullet_) {
			// todo: calculate damage
			life -= bullet_->damage;

			if (life <= 0) {
				scene->MakeEffectText(pos.As<float>(), XY{ 0, -1 }, xx::RGBA8_Red, bullet_->damage.ToInt());
				Kill();			// unsafe
				return true;
			} else {
				scene->MakeEffectText(pos.As<float>(), XY{ 0, -1 }, xx::RGBA8_Yellow, bullet_->damage.ToInt());
				changeColorToWhiteElapsedTime = scene->frameNumber + cColorPlusChangeDuration;
				return false;
			}
		}
	}
}
