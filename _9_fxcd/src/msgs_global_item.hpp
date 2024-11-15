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
				xx::AddTo(owner_->items, xx::SharedFromThis(this));
				owner = xx::WeakFromThis(owner_);
			} else {
				xx::AddTo(scene->items, xx::SharedFromThis(this));
			}
		}

		inline void Item::MoveTo(Monster* newOwner_) {
			assert(scene);
			assert(!owner || owner->items[indexAtContainer].pointer == this);

			if (newOwner_) {
				if (owner) {
					// old owner to new
					xx::AddTo(newOwner_->items, xx::MoveOutFrom(owner->items, indexAtContainer));
				} else {
					// scene floor to new
					xx::AddTo(newOwner_->items, xx::MoveOutFrom(scene->items, indexAtContainer));
				}
				owner = xx::WeakFromThis(newOwner_);
			} else {
				if (owner) {
					// old owner to scene floor
					xx::AddTo(scene->items, xx::MoveOutFrom(owner->items, indexAtContainer));
					owner.Reset();
				} else {
					// scene floor to scene floor ??
					assert(false);
				}
			}
		}

	}
}
