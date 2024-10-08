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
		Log_Dial_Retry();
		goto LabDial;
	}
	{
		auto d = Msgs::gSerdeInfo.MakeDataEx();
		auto msg = xx::MakeShared<Msgs::C2S::Join>();
		d.Write(msg);
		if (!Send(xx::DataShared(std::move(d)))) {
			Log_Send_Join_Fail();
			goto LabBegin;
		}
	}
LabWait_Join_r:
	co_yield 0;
	if (!Alive()) {
		Log_Msg_Wait_Join_r_Disconnected();
		goto LabBegin;
	}
	if (xx::DataShared ds; recvs.TryPop(ds)) {
		auto typeId = xx::ReadTypeId(ds);
		if (!typeId) {
			Log_Msg_Receive_Bad_Data();
			goto LabBegin;
		}
		if (typeId != Msgs::S2C::Join_r::cTypeId) {
			Log_Msg_Wait_Join_r_Receive_Unknown();
			goto LabBegin;
		}
		auto dr = Msgs::gSerdeInfo.MakeDataEx_r(ds);
		xx::Shared<Msgs::S2C::Join_r> msg;
		if (dr.Read(msg)) {
			Log_Msg_Read_Join_r_Error();
			goto LabBegin;
		}
		clientId = msg->clientId;
		scene = std::move(msg->scene);
		scene->InitForDraw();
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
