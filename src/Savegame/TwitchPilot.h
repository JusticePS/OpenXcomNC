#pragma once
#include <string>
#include "../Engine/Script.h"
namespace OpenXcom
{
	class SavedGame;


	class TwitchPilot
	{
	public:
		static int last_id;

		void load(const YAML::YamlNodeReader& reader);
		void save(YAML::YamlNodeWriter writer) const;

	private:
		int64_t _id;
		int _kills;
		int _downs;
		int _points;

	public:
		int64_t getId() { return _id; }
		bool matches(const int64_t id) { return _id == id; }

		int getKills() { return _kills; }
		int getDowns() { return _downs; }
		int getPoints() { return _points; }

		void addKill() { ++_kills; }
		void addDown() { ++_downs; }
		void addPoints(int points) { _points += points; }

		static TwitchPilot* Create(int64_t id)
		{
			TwitchPilot* newPilot = new TwitchPilot();
			newPilot->_id = id;
			newPilot->_kills = 0;
			newPilot->_downs = 0;
			newPilot->_points = 0;
			return newPilot;
		}

	};
}

