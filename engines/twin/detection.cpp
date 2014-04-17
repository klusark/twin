/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "engines/advancedDetector.h"
#include "engines/obsolete.h"

#include "engines/twin/twin.h"

#include "common/system.h"
#include "common/savefile.h"

namespace Twin {

struct TwinGameDescription {
	ADGameDescription desc;
	TwinGameType gameType;
};

static const PlainGameDescriptor twinGames[] = {
	{"lba", "Little Big Adventure"},
	{"lba2", "Little Big Adventure 2"},
	{0, 0}
};

static const TwinGameDescription gameDescriptions[] = {
	{
		// Little Big Adventure
		{
			"lba",
			"",
			AD_ENTRY1s("INVOBJ.HQR", "0f536b55677b3b38fecfd25b09ea6870", 38059),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_UNSTABLE,
			GUIO0()
		},
		GType_LBA
	},
	{
		// Little Big Adventure 2
		{
			"lba2",
			"",
			AD_ENTRY1s("LBA2.HQR", "e235f425cc8086383971dfe280c267c8", 250788),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_UNSTABLE,
			GUIO0()
		},
		GType_LBA2
	},

	{ AD_TABLE_END_MARKER, GType_LBA }
};


class TwinMetaEngine : public AdvancedMetaEngine {
public:
	TwinMetaEngine() : AdvancedMetaEngine(Twin::gameDescriptions, sizeof(Twin::TwinGameDescription), twinGames) {
		_guioptions = GUIO_NOMIDI;
	}

	virtual GameDescriptor findGame(const char *gameid) const {
		return Engines::findGameID(gameid, _gameids);
	}

	virtual const char *getName() const {
		return "Twin Engine";
	}

	virtual const char *getOriginalCopyright() const {
		return "Adeline Software International";
	}

	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;

};

bool TwinMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const TwinGameDescription *tgd = (const TwinGameDescription*)desc;
	*engine = new TwinEngine(syst, tgd->gameType);

	return true;
}

} // end of namespace Twin

#if PLUGIN_ENABLED_DYNAMIC(TWIN)
	REGISTER_PLUGIN_DYNAMIC(TWIN, PLUGIN_TYPE_ENGINE, Twin::TwinMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(TWIN, PLUGIN_TYPE_ENGINE, Twin::TwinMetaEngine);
#endif

