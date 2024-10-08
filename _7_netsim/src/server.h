#pragma once
#include "looper.h"
#include "msgs.h"
#include "net.h"

struct Server : NetServer {
	int32_t autoClientId{};
	xx::Shared<Msgs::Global::Scene> scene;

	void Init();
	void Update();

	void Log_Msg_Read_Error() { xx::CoutTN("S Log_Msg_Read_Error"); };
	void Log_Msg_Receive_Unknown() { xx::CoutTN("S Log_Msg_Receive_Unknown"); };
	void Log_Msg_Receive_Bad_Data() { xx::CoutTN("S Log_Msg_Receive_Bad_Data"); };
};
