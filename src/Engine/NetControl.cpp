#include "NetControl.h"
#include "Game.h"
#include <SDL_net.h>
#include "Exception.h"
#include "NetControl_Packets.h"
#include "../Geoscape/GeoscapeState.h"


namespace OpenXcom
{
using namespace NetControlPackets;

NetControl::NetControl(Game* game)
{
	memset(packetBuffer, 0, sizeof(packetBuffer));
	_game = game;
	detectionState = nullptr;
	geoscape = nullptr;
	recvSock = SDLNet_UDP_Open(32918);
	sendSock = SDLNet_UDP_Open(0);
	if (recvSock == nullptr || sendSock == nullptr)
		throw Exception(SDL_GetError());
	sendAddress = new IPaddress();
	SDLNet_ResolveHost(sendAddress, "127.0.0.1", 32919);
	lastStateWasGeoscape = false;
}

NetControl::~NetControl()
{
	if (recvSock != nullptr)
		SDLNet_UDP_Close(recvSock);
	if (sendSock != nullptr)
		SDLNet_UDP_Close(sendSock);
	if (sendAddress != nullptr)
		delete sendAddress;
}

void NetControl::Update(void)
{
	if (recvSock == nullptr || sendSock == nullptr)
		return;
	//poll for new messages and execute/respond to them
	UDPpacket packet;
	packet.data = packetBuffer;
	packet.maxlen = sizeof(packetBuffer);
	if (SDLNet_UDP_Recv(recvSock, &packet) == 1)
	{
		if (_game != nullptr && packet.len >= sizeof(NetControl_PacketBase) && packetBuffer[0] == 'X' && packetBuffer[1] == 'C' && packetBuffer[2] == 'O' && packetBuffer[3] == 'M' )
		{
			//should be valid.  Check our size matches expected size
			NetControl_PacketBase::Execute(packetBuffer, packet.len, this, _game);
		}
	}

	if (geoscape != nullptr && _game != nullptr && _game->isState(geoscape))
	{
		if (!lastStateWasGeoscape)
		{
			NetControl_PacketBase basePacket(NetControlPacketTypes::RETURNED_TO_GEOSCAPE);
			SendPacketBot(&basePacket, sizeof(NetControl_PacketBase));
			lastStateWasGeoscape = true;
		}
	}
	else
	{
		lastStateWasGeoscape = false;
	}

}

void NetControl::SendPacketBot(NetControl_PacketBase* inPacket, int packetSize)
{
	if (sendSock == nullptr)
		return;

	UDPpacket packet;
	packet.data = (uint8_t*)inPacket;
	packet.maxlen = packetSize;
	packet.len = packetSize;
	packet.address = *sendAddress;


	if (SDLNet_UDP_Send(sendSock, -1, &packet) != 1)
	{
		//failed
	}
}
}
