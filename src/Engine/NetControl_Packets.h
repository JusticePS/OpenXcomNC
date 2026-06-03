#pragma once
#include <stdint.h>

namespace OpenXcom
{
	class NetControl;
	class Game;
namespace NetControlPackets
{
	enum class NetControlPacketTypes : int16_t
	{
		INVALID = -1,
		SETVOLUME = 0,
		INCREASE_GEOSCAPE_SPEED = 1,
		DECREASE_GEOSCAPE_SPEED = 2,
		ATTACK_UFO = 3,
		UFO_DETECTED = 4,
		OTHER_POPUP = 5,
		RETURNED_TO_GEOSCAPE = 6,
		UFO_DEFEATED = 7,
		REQUEST_RESEARCH_INFO = 8,
		RESEARCH_INFO_LABS = 9,
		RESEARCH_INFO_PROJECT = 10,
		RESEARCH_SET_SCIENTISTS = 11,
		UI_GET_INTERACTIVE_SURFACES_INFOS = 12, //NYI
		UI_INTERACTIVE_SURFACES_INFOS = 13, //NYI
		UI_PRESS_INTERACTIVE_SURFACE = 14, //NYI
		UI_RAW_TEXT = 15,
		GEOSCAPE_ROTATE = 16,
		MOVE_CURSOR = 17,
		BS_SET_LABEL_MODE = 18,
		BS_TARGET = 19,
		BS_CAMERA = 20,
		GEOSCAPE_SHOW_CURSOR = 21,
		GEOSCAPE_HIDE_CURSOR = 22,
		GEOSCAPE_MOVE_CURSOR = 23,
		GEOSCAPE_CLICK_CURSOR = 24,
		EQUIP_MOVE_ITEM = 25,
		EQUIP_MOVE_ITEM_NAME = 26,
		EQUIP_DROP_ITEM = 27,
		EQUIP_PICKUP_ITEM_NAME = 28,
		EQUIP_UNLOAD_ITEM = 29,
		EQUIP_USE_ITEM = 30, // right or middle click
	};


#pragma pack(push, 1)
struct NetControl_PacketBase
{
public:
	char sentinel[4]; //XCOM
	NetControlPacketTypes packetType;

	static void Execute(uint8_t buffer[1024], int packetSize, NetControl* nc, Game* game);

	NetControl_PacketBase() : NetControl_PacketBase(NetControlPacketTypes::INVALID)
	{
	}

	NetControl_PacketBase(NetControlPacketTypes inType) :
		sentinel{ 'X','C', 'O', 'M' },
		packetType(inType)
	{
	}

};

struct NetControl_PacketVolume : NetControl_PacketBase
{
public:
	//note: 0 - 128
	int16_t sound;
	int16_t music;
	int16_t ui;

	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_ChangeGeoscapeSpeed : NetControl_PacketBase
{
public:
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_AttackUfo : NetControl_PacketBase
{
public:
	int interceptor;
	int ufo_id;
	int64_t pilot;
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_UFODetected : NetControl_PacketBase
{
public:
	int ufo_id;
	int interceptorCount;
	inline static const int max_interceptors = 32;
	int interceptorIds[max_interceptors]; // that's 4 per base with maxed out 8 bases, so... should be enough
	struct InterceptorName
	{
		char name[32];
		InterceptorName() : name{ 0 }
		{

		}
	} interceptorNames[max_interceptors];

	NetControl_UFODetected() : NetControl_PacketBase(NetControlPacketTypes::UFO_DETECTED),
		ufo_id(-1),
		interceptorCount(-1),
		interceptorIds{0}
	{

	}
};

struct NetControl_UFODefeated : NetControl_PacketBase
{
public:
	int interceptorId;
	int ufoId;
	int64_t pilot;
	int points;
	int pilotScore;
	int pilotDowns;
	int pilotKills;
	int pilotLeaderboardPosition;
	uint8_t destroyed;

	NetControl_UFODefeated() : NetControl_PacketBase(NetControlPacketTypes::UFO_DEFEATED),
		interceptorId(0),
		ufoId(-1),
		pilot(0),
		points(0),
		pilotScore(0),
		pilotDowns(0),
		pilotKills(0),
		pilotLeaderboardPosition(0),
		destroyed(false)
	{
	}
};

struct NetControl_OtherPopup : NetControl_PacketBase
{
public:

};

struct NetControl_RequestResearchInfo : NetControl_PacketBase
{
public:
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_ResearchInfoLabs : NetControl_PacketBase
{
public:

	int scientistsAvailable;
	int labSpaceAvailable;
	int numProjects;

	NetControl_ResearchInfoLabs() : NetControl_PacketBase(NetControlPacketTypes::RESEARCH_INFO_LABS),
		scientistsAvailable(0),
		labSpaceAvailable(0),
		numProjects(0)

	{

	}
};

struct NetControl_ResearchInfoProject: NetControl_PacketBase
{
public:

	int scientistsAssigned;
	char name[32];
	char progress[16];

	NetControl_ResearchInfoProject() : NetControl_PacketBase(NetControlPacketTypes::RESEARCH_INFO_PROJECT),
		scientistsAssigned(0),
		name{ 0 },
		progress{ 0 }
	{

	}
};

struct NetControl_ResearchSetScientists : NetControl_PacketBase
{
public:

	int scientistsToAssign;
	char name[32];

	NetControl_ResearchSetScientists() : NetControl_PacketBase(NetControlPacketTypes::RESEARCH_SET_SCIENTISTS),
		scientistsToAssign(0),
		name{ 0 }
	{

	}

	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_UIPressInteractiveSurface : NetControl_PacketBase
{
public:
	char label[32];
	int16_t keyNumber;
	int xpos;
	int ypos;
	int access_level;

	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_UIRawText : NetControl_PacketBase
{
  public:
	char text[64];

	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_GeoscapeRotate : NetControl_PacketBase
{
  public:
	int amount;
	int8_t northSouth;

	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_MoveCursor : NetControl_PacketBase
{
	public:
		int xpos;
		int ypos;

		void _Execute(NetControl* nc, Game* game);
};

struct NetControl_BSSetLabelMode : NetControl_PacketBase
{
	public:
		int8_t labelMode;
		int color;

		void _Execute(NetControl* nc, Game* game);
};

struct NetControl_BSTarget : NetControl_PacketBase
{
  public:
	int xpos;
	int ypos;
	int8_t targetMode;

	void _Execute(NetControl* nc, Game* game);

	enum TargetModes
	{
		BTM_MOVE = 0,
		BTM_LCLICK = 1,
		BTM_RCLICK = 2,
		BTM_DCLICK = 3,
		BTM_CCLICK = 4, 
	};
};

struct NetControl_BSCamera : NetControl_PacketBase
{
  public:
	int x;
	int y;
	int z;
	int8_t mode;

	void _Execute(NetControl* nc, Game* game);

	enum ModeFlags
	{
		BCM_MOVEXABS = 1 << 0,
		BCM_MOVEYABS = 1 << 1,
		BCM_MOVEZABS = 1 << 2,
	};
};

struct NetControl_GeoscapeShowCursor : NetControl_PacketBase
{
  public:
	int xpos;
	int ypos;

	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_GeoscapeMoveCursor : NetControl_PacketBase
{
  public:
	int dx;
	int dy;

	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_GeoscapeHideCursor : NetControl_PacketBase
{
  public:
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_GeoscapeClickCursor : NetControl_PacketBase
{
  public:
	int8_t button;
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_EquipMoveItem : NetControl_PacketBase
{
  public:
	char srcSectionName[32];
	int srcX;
	int srcY;
	char destSectionName[32];
	int destX;
	int destY;
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_EquipMoveItemName : NetControl_PacketBase
{
  public:
	char srcName[32];
	char destSectionName[32];
	int destX;
	int destY;
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_DropItem : NetControl_PacketBase
{
  public:
	char srcName[32];
	int srcX;
	int srcY;
	int8_t isItemName;
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_PickupItemName : NetControl_PacketBase
{
  public:
	char srcName[32];
	char destSectionName[32];
	int destX;
	int destY;
	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_UnloadItem : NetControl_PacketBase
{
  public:
	char srcName[32];
	int srcX;
	int srcY;
	int8_t srcIsItemName;

	void _Execute(NetControl* nc, Game* game);
};

struct NetControl_UseItem : NetControl_PacketBase
{
	char srcName[32];
	int srcX;
	int srcY;
	int8_t srcIsItemName;
	int8_t button;
	void _Execute(NetControl* nc, Game* game);
};


#pragma pack(pop)
}
}
