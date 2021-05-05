/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/** @file
 *  Game functions specific to KotOR.
 */

#include "game.h"

#include "../common/pathutil.h"

#include "../resource/keybifprovider.h"
#include "../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static constexpr char kPatchFilename[] = "patch.erf";
static constexpr char kWavesDirectoryName[] = "streamwaves";
static constexpr char kExeFilename[] = "swkotor.exe";

static vector<string> g_nonTransientLipFiles { "global.mod", "localization.mod" };

void Game::initResourceProvidersForKotOR() {
    Resources::instance().indexKeyFile(getPathIgnoreCase(_path, kKeyFilename));
    Resources::instance().indexErfFile(getPathIgnoreCase(_path, kPatchFilename));

    fs::path texPacksPath(getPathIgnoreCase(_path, kTexturePackDirectoryName));
    Resources::instance().indexErfFile(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    Resources::instance().indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilename));

    Resources::instance().indexDirectory(getPathIgnoreCase(_path, kMusicDirectoryName));
    Resources::instance().indexDirectory(getPathIgnoreCase(_path, kSoundsDirectoryName));
    Resources::instance().indexDirectory(getPathIgnoreCase(_path, kWavesDirectoryName));

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    for (auto &filename : g_nonTransientLipFiles) {
        Resources::instance().indexErfFile(getPathIgnoreCase(lipsPath, filename));
    }

    Resources::instance().indexExeFile(getPathIgnoreCase(_path, kExeFilename));
    Resources::instance().indexDirectory(getPathIgnoreCase(_path, kOverrideDirectoryName));
}

} // namespace game

} // namespace reone
