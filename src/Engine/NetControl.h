#pragma once
#include <SDL_net.h>

namespace OpenXcom
{
class Game;
class GeoscapeState;
class UfoDetectedState;
namespace NetControlPackets
{
	struct NetControl_PacketBase;
}
class NetControl
{
private:
	Game* _game;
	UDPsocket sendSock;
	UDPsocket recvSock;
	uint8_t packetBuffer[1024];
	IPaddress* sendAddress;
	bool lastStateWasGeoscape;
public:
	NetControl(Game* game);
	~NetControl();

	GeoscapeState* geoscape;
	UfoDetectedState* detectionState;

	void Update(void);
	void SendPacketBot(NetControlPackets::NetControl_PacketBase* packet, int packetSize);
};

}
