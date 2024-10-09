#pragma once
#include "looper.h"
#include "msgs.h"

struct Peer;
struct Client {
	xx::Shared<Msgs::Global::Scene> scene;
	XY centerPos;

	int32_t clientId{};
	xx::Weak<Peer> peer;
	xx::Queue<xx::DataShared> recvs;
	xx::Task<> task;
	xx::Task<> Task();

	void Init(XY const& centerPos_);
	void Update();
	void Draw();

	void CleanUp();
	void Dial();
	bool Alive() const;
	void Close();
	bool Send(xx::DataShared ds);

	void Log_Dial_Fail_Retry() { xx::CoutTN("C Log_Dial_Fail_Retry"); };
	void Log_Send_Join_Fail() { xx::CoutTN("C Log_Send_Join_Fail"); };
	void Log_Msg_Wait_Join_r_Disconnected() { xx::CoutTN("C Log_Msg_Wait_Join_r_Disconnected"); };
	void Log_Msg_Read_Join_r_Error() { xx::CoutTN("C Log_Msg_Read_Join_r_Error"); };
	void Log_Msg_Wait_Join_r_Receive_Unknown() { xx::CoutTN("C Log_Msg_Wait_Join_r_Receive_Unknown"); };
	void Log_Msg_Wait_Commands_Disconnected() { xx::CoutTN("C Log_Msg_Wait_Commands_Disconnected"); };
	void Log_Msg_Wait_Commands_Receive_Unknown() { xx::CoutTN("C Log_Msg_Wait_Commands_Receive_Unknown"); };
};
