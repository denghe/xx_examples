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

LabBegin:
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
		goto LabBegin;
	}

	if (xx::DataShared ds; recvs.TryPop(ds)) {
		if (xx::ReadTypeId(ds) != Msgs::S2C::Join_r::cTypeId) {
			Log_Msg_Wait_Join_r_Receive_Unknown();
			goto LabBegin;
		}
		if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::S2C::Join_r>(ds); !msg) {
			Log_Msg_Read_Join_r_Error();
			goto LabBegin;
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
		goto LabBegin;
	}

	// handle commands & sync
	{
		xx::DataShared ds;
		while (recvs.TryPop(ds)) {
			auto typeId = xx::ReadTypeId(ds);
			switch (typeId) {
			case Msgs::S2C::Summon_r::cTypeId: {
				if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::S2C::Summon_r>(ds); !msg) {
					Log_Msg_Read_Join_r_Error();
					goto LabBegin;
				} else {
					auto monster = xx::MakeShared<Msgs::Global::Monster>();
					monster->Init(scene, msg->data);
					scene->monsters.Add(monster);
					// todo: sync space
				}
				break;
			}
			default:
				Log_Msg_Wait_Commands_Receive_Unknown();
				break;
			}
		}
	}

	// handle local input
	if (!gLooper.mouseEventHandler) {
		if (gLooper.mouse.PressedMBLeft()) {
			if (std::abs(centerPos.x - gLooper.mouse.pos.x) < gLooper.width_2) {
				Send(Msgs::gSerdeInfo.MakeDataShared<Msgs::C2S::Summon>());
			}
		}
	}

	goto LabPlay;
}

void Client::Init(XY const& centerPos_) {
	gIsServer = false;
	task = Task();
	centerPos = centerPos_;
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

void Client::Close() {
	peer.Reset();
	recvs.Clear();
}

bool Client::Send(xx::DataShared ds) {
	if (!peer) return false;
	peer->recvs.Emplace(std::move(ds));
	return true;
}
