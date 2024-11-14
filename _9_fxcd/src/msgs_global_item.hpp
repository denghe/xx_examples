#pragma once

namespace Msgs {
	namespace Global {

		inline void Item::WriteTo(xx::Data& d) const {
			d.Write(
				scene,
				owner,
				cType,
				cQualitity,
				cEquipmentLocation,
				cQuantityLimit,
				cCastDelay,
				quantity,
				lastCastTime,
				stats
			);
		}

		inline int32_t Item::ReadFrom(xx::Data_r& dr) {
			return dr.Read(
				scene,
				owner,
				cType,
				cQualitity,
				cEquipmentLocation,
				cQuantityLimit,
				cCastDelay,
				quantity,
				lastCastTime,
				stats
			);
		}

		inline void Item::Init(Scene* scene_, Monster* owner_) {
			assert(!scene);
			assert(!owner);
			assert(scene_);
			assert(!owner_ || owner->scene.GetPointer() == scene_);
			scene = xx::WeakFromThis(scene_);
			if (owner_) {
				owner = xx::WeakFromThis(owner_);
				assert(scene_->items.Empty() || scene_->items.Top().pointer != this);	// auto add check
				indexAtContainer = scene_->items.Len();
				scene_->items.Emplace(xx::SharedFromThis(this));
			} else {
				assert(owner_->items.Empty() || owner_->items.Top().pointer != this);	// auto add check
				indexAtContainer = owner_->items.Len();
				owner_->items.Emplace(xx::SharedFromThis(this));
			}
		}

		inline void Item::MoveTo(Monster* owner_) {
			assert(scene);
			assert(!owner || owner->items[indexAtContainer].pointer == this);

			if (owner_) {
				if (owner) {											// old owner to new
					auto& p = owner->items[indexAtContainer];
					auto tmp = std::move(p);
					if (indexAtContainer + 1 < owner->items.Len()) {	// not last one: swap
						p = std::move(owner->items.Top());
						p->indexAtContainer = indexAtContainer;
					}
					owner->items.Pop();

					indexAtContainer = owner_->items.Len();
					owner_->items.Add(std::move(tmp));
				} else {												// scene floor to owner

				}
				//owner = xx::WeakFromThis(owner_);
			} else {
				if (owner) {		// old owner to scene floor

				} else {			// scene floor to scene floor ??
					assert(false);
				}
			}
		}

	}
}
