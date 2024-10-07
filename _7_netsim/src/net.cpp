#include "pch.h"
#include "net.h"
#include "server.h"
#include "client.h"

bool NetPeer::Alive() const {
	return nc;
}

bool NetPeer::Send(xx::DataShared ds) {
	if (!nc) return false;
	nc->recvs.Emplace(std::move(ds));
	return true;
}

xx::Weak<NetPeer> NetServer::Accept(xx::Shared<NetClient> const& nc) {
	auto& p = peers.Emplace();
	p.Emplace();
	p->nc = nc.ToWeak();
	return p.ToWeak();
}

void NetClient::Dial() {
	assert(!ns);
	if (gLooper.server) {
		ns = gLooper.server->Accept(xx::SharedFromThis(this));
	}
}

bool NetClient::Alive() const {
	return ns;
}

void NetClient::Close() {
	ns.Reset();
	recvs.Clear();
}

bool NetClient::Send(xx::DataShared ds) {
	if (!ns) return false;
	ns->recvs.Emplace(std::move(ds));
	return true;
}

bool NetClient::Send_Join() {
	assert(ns);
	auto d = Msgs::gSerdeInfo.MakeDataEx();
	auto msg = xx::MakeShared<Msgs::C2S::Join>();
	d.Write(msg);
	return Send(xx::DataShared(std::move(d)));
}
