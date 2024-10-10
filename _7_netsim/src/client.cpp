#include "pch.h"
#include "looper.h"
#include "client.h"
#include "server.h"

void Client::CleanUp() {
	Close();
	clientId = 0;
	scene.Reset();
	// ...
}

xx::Task<> Client::Task() {

LabReset:
	CleanUp();

LabDial:
	co_yield 0;

	Dial();

	if (!Alive()) {
		Log_Dial_Fail_Retry();
		goto LabDial;
	}

	Send(Msgs::gSerdeInfo.MakeDataShared<Msgs::C2S::Join>());

LabWait_Join_r:
	co_yield 0;

	if (!Alive()) {
		Log_Msg_Wait_Join_r_Disconnected();
		goto LabReset;
	}

	if (xx::DataShared ds; recvs.TryPop(ds)) {
		if (auto typeId = xx::ReadTypeId(ds); typeId != Msgs::S2C::Join_r::cTypeId) {
			Log_Msg_Wait_Join_r_Receive_Unknown(typeId);
			goto LabReset;
		}
		if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::S2C::Join_r>(ds); !msg) {
			Log_Msg_ReadError<Msgs::S2C::Join_r>();
			goto LabReset;
		} else {
			clientId = msg->clientId;
			scene = std::move(msg->scene);
			scene->InitForDraw();
		}
	} else {
		// todo: timeout check?
		goto LabWait_Join_r;
	}

LabPlay:
	co_yield 0;

	if (!Alive()) {
		Log_Msg_Wait_Commands_Disconnected();
		goto LabReset;
	}

	// handle commands & sync
	{
		xx::DataShared ds;
		while (recvs.TryPop(ds)) {
			auto typeId = xx::ReadTypeId(ds);
			switch (typeId) {
			case Msgs::S2C::PlayerJoin::cTypeId: {
				if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::S2C::PlayerJoin>(ds); !msg) {
					Log_Msg_ReadError<Msgs::S2C::PlayerJoin>();
					goto LabReset;
				} else {
					if (msg->clientId != clientId) {
						// if (scene->frameNumber != msg->frameNumber) // todo: rollback or fast forward
						xx::MakeShared<Msgs::Global::Player>()->Init(scene, msg->clientId);
					}
				}
				break;
			}
			case Msgs::S2C::PlayerLeave::cTypeId: {
				if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::S2C::PlayerLeave>(ds); !msg) {
					Log_Msg_ReadError<Msgs::S2C::PlayerLeave>();
					goto LabReset;
				} else {
					// if (scene->frameNumber != msg->frameNumber) // todo: rollback or fast forward
					scene->RemovePlayer(msg->clientId);
				}
				break;
			}
			case Msgs::S2C::Summon::cTypeId: {
				if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::S2C::Summon>(ds); !msg) {
					Log_Msg_ReadError<Msgs::S2C::Summon>();
					goto LabReset;
				} else {
					// if (scene->frameNumber != msg->frameNumber) // todo: rollback or fast forward
					if (auto& player = scene->RefPlayer(msg->clientId); !player) {
						Log_Msg_Handle_Summon_r_Error_Player_Not_Found(msg->clientId);
						goto LabReset;
					} else {
						xx::MakeShared<Msgs::Global::Monster>()->Init(scene, player, msg->bornPos);
					}
				}
				break;
			}
			// ...
			default:
				Log_Msg_Wait_Commands_Receive_Unknown();
				break;
			}
		}
	}

	// handle local input
	if (!gLooper.mouseEventHandler) {
		if (gLooper.mouse.PressedMBLeft()) {
			if (gLooper.mouse.pos.x >= minX && gLooper.mouse.pos.x <= maxX) {
				auto msg = xx::MakeShared<Msgs::C2S::Summon>();
				msg->bornPos = scene->monsterSpace.max / 2 + gLooper.mouse.pos - centerPos;
				Send(Msgs::gSerdeInfo.MakeDataShared(msg));
			}
		}
	}

	goto LabPlay;
}

void Client::Init(XY const& centerPos_) {
	gIsServer = false;
	task = Task();
	centerPos = centerPos_;
	minX = centerPos.x - gLooper.width_2 / 2;
	maxX = centerPos.x + gLooper.width_2 / 2;
}

void Client::Update() {
	gIsServer = false;
	task();
	if (!scene) return;
	scene->Update();
}

void Client::Draw() {
	gIsServer = false;
	if (!scene) return;
	scene->Draw();
	xx::Quad().SetFrame(scene->frame).SetPosition(centerPos).Draw();
}

void Client::Dial() {
	assert(!peer);
	if (gLooper.server) {
		peer = gLooper.server->Accept(xx::SharedFromThis(this));
	}
}

bool Client::Alive() const {
	return peer;
}

bool Client::Joined() const {
	return clientId;
}

void Client::Close() {
	peer.Reset();
	recvs.Clear();
}

bool Client::Send(xx::DataShared ds) {
	if (!peer) return false;
	peer->recvs.Emplace(std::move(ds));
	return true;
}
