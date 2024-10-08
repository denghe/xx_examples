#include "pch.h"
#include "looper.h"
#include "client.h"

void Client::CleanUp() {
	Close();
	clientId = 0;
	scene.Reset();
	// ...
}

xx::Task<> Client::Task() {
LabBegin:
	co_yield 0;
	CleanUp();
LabDial:
	co_yield 0;
	Dial();
	if (!Alive()) {
		Log_Dial_Retry();
		goto LabDial;
	}
	if (!Send_Join()) {
		Log_Send_Join_Fail();
		goto LabBegin;
	}
LabWait_Join_r:
	co_yield 0;
	if (!Alive()) {
		Log_Msg_Wait_Join_r_Disconnected();
		goto LabBegin;
	}
	{
		xx::DataShared ds;
		if (recvs.TryPop(ds)) {
			auto dr = Msgs::gSerdeInfo.MakeDataEx_r(ds);
			xx::Shared<xx::SerdeBase> ssb;
			if (dr.Read(ssb)) {
				Log_Msg_Read_Join_r_Error();
				goto LabBegin;
			}
			switch (ssb->typeId) {
			case Msgs::S2C::Join_r::cTypeId: {
				auto& msg = ssb.Cast<Msgs::S2C::Join_r>();
				clientId = msg->clientId;
				scene = std::move(msg->scene);
				break;
			}
			default:
				Log_Msg_Wait_Join_r_Receive_Unknown();
				goto LabBegin;
			}
		}
	}
	goto LabWait_Join_r;
LabPlay:
	co_yield 0;
	if (!Alive()) {
		Log_Msg_Wait_Commands_Disconnected();
		goto LabBegin;
	}
	// todo: handle commands & sync
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
