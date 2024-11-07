#include "pch.h"
#include "looper.h"
#include "client.h"
#include "server.h"

void Client::CleanUp() {
	Close();
	clientId = 0;
	nextGenTime = 0;
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
					if (msg->clientId != clientId) {	// not self
						// if (scene->frameNumber != msg->frameNumber) // todo: rollback or fast forward
						assert(scene->frameNumber == msg->frameNumber);
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
					assert(scene->frameNumber == msg->frameNumber);
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
					assert(scene->frameNumber == msg->frameNumber);
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
		auto pos = gLooper.camera.ToLogicPos(gLooper.mouse.pos);
		//auto pos = Msgs::Global::Scene::mapSize_2 + gLooper.mouse.pos.FlipY() / gLooper.camera.scale;
		if (pos.x >= 0 && pos.y >= 0 && pos.x < Msgs::Global::Scene::mapSize.x && pos.y < Msgs::Global::Scene::mapSize.y) {
			if (gLooper.mouse.PressedMBLeft()) {
				if (lastMousePressed) {
					genDelay -= 1;
					if (genDelay < 1) {
						genDelay = 1;
					}
				}
				lastMousePressed = true;
				if (nextGenTime <= scene->frameNumber) {
					nextGenTime = scene->frameNumber + genDelay;
					for (int i = 0; i < 1; ++i) {
						auto msg = xx::MakeShared<Msgs::C2S::Summon>();
						msg->bornPos = pos;
						Send(Msgs::gSerdeInfo.MakeDataShared(msg));
					}
				}
			} else {
				lastMousePressed = false;
				genDelay = int(Looper::fps / 2);
			}
		}
	}

	co_yield 0;
	goto LabPlay;
}

void Client::Init() {
	gIsServer = false;
	task = Task();
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
