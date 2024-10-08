#pragma once
#include "looper.h"
#include "msgs.h"
#include "net.h"

struct Client : NetClient {
	int32_t clientId{};
	xx::Shared<Msgs::Global::Scene> scene;
	XY centerPos;
	xx::Task<> task;
	xx::Task<> Task();
	void CleanUp();
	void Init(XY const& centerPos_);
	void Update();
	void Draw();

	void Log_Dial_Retry() { xx::CoutTN("C Log_Dial_Retry"); };
	void Log_Send_Join_Fail() { xx::CoutTN("C Log_Send_Join_Fail"); };
	void Log_Msg_Wait_Join_r_Disconnected() { xx::CoutTN("C Log_Msg_Wait_Join_r_Disconnected"); };
	void Log_Msg_Read_Join_r_Error() { xx::CoutTN("C Log_Msg_Read_Join_r_Error"); };
	void Log_Msg_Wait_Join_r_Receive_Unknown() { xx::CoutTN("C Log_Msg_Wait_Join_r_Receive_Unknown"); };
	void Log_Msg_Wait_Commands_Disconnected() { xx::CoutTN("C Log_Msg_Wait_Commands_Disconnected"); };
};
