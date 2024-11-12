#pragma once

namespace Msgs {
	namespace Global {

		inline void Player::WriteTo(xx::Data& d) const {
			d.Write(scene, clientId, score);
		}

		inline int32_t Player::ReadFrom(xx::Data_r& dr) {
			return dr.Read(scene, clientId, score);
		}

		inline void Player::Init(Scene* scene_, int32_t clientId_) {
			assert(scene_->players.Empty() || scene_->players.Top().pointer != this);	// auto add check
			scene_->players.Emplace(xx::SharedFromThis(this));
			scene = xx::WeakFromThis(scene_);
			clientId = clientId_;
		}

		inline void Player::Update() {
			assert(false);
		}

		inline void Player::Draw() {
			assert(false);
		}

	}
}
