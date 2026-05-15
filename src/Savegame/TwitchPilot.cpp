#include <string>
#include "TwitchPilot.h"
#include "SavedGame.h"

namespace OpenXcom
{
	int TwitchPilot::last_id = 0;
	void TwitchPilot::save(YAML::YamlNodeWriter writer) const
	{
		writer.setAsMap();

		writer.write("id", _id);
		writer.write("kills", _kills);
		writer.write("downs", _downs);
		writer.write("points", _points);
	}

	void TwitchPilot::load(const YAML::YamlNodeReader& node)
	{
		const auto& reader = node.useIndex();
		reader.tryRead("id", _id);
		reader.tryRead("kills", _kills);
		reader.tryRead("downs", _downs);
		reader.tryRead("points", _points);
	}
}
