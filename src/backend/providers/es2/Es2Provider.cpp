// Pegasus Frontend
// Copyright (C) 2017-2020  Mátyás Mustoha
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#include "Es2Provider.h"

#include "Log.h"
#include "Paths.h"
#include "providers/es2/Es2Games.h"
#include "providers/es2/Es2Metadata.h"
#include "providers/es2/Es2Systems.h"



#include <QDir>
#include <QStringBuilder>


namespace {
std::vector<QString> default_config_paths()
{
    return {
        paths::homePath() % QStringLiteral("/.emulationstation/"),
        QStringLiteral("/etc/emulationstation/"),
    };
}

} // namespace


namespace providers {
namespace es2 {

Es2Provider::Es2Provider(QObject* parent)
    : Provider(QLatin1String("es2"), QStringLiteral("EmulationStation"), parent)
{}

Provider& Es2Provider::run(SearchContext& sctx)
{
    
    std::vector<QString> possible_config_dirs = [this]{
        const auto option_it = options().find(QStringLiteral("installdir"));
        return (option_it != options().cend())
            ? std::vector<QString>{ QDir::cleanPath(option_it->second.front()) + QLatin1Char('/') }
            : default_config_paths();
    }();
    
    for (int i = 0; i < possible_config_dirs.size(); ++i) {
        Log::info(display_name(), LOGMSG("ES2 Default config path : %1").arg(possible_config_dirs.at(i)));
    }

    // Find systems
    const std::vector<SystemEntry> systems = find_systems(display_name(), possible_config_dirs);
    if (systems.empty())
        return *this;
    Log::info(display_name(), LOGMSG("Found %1 systems").arg(QString::number(systems.size())));

    const float progress_step = 1.f / (systems.size() * 2);
    float progress = 0.f;

    // Load MAME blacklist, if exists
    const std::vector<QString> mame_blacklist = read_mame_blacklists(display_name(), possible_config_dirs);

    // Find games
    for (const SystemEntry& sysentry : systems) {
        const size_t found_games = find_games_for(sysentry, sctx, mame_blacklist);
        Log::info(display_name(), LOGMSG("System `%1` provided %2 games")
            .arg(sysentry.name, QString::number(found_games)));

        progress += progress_step;
        emit progressChanged(progress);
    }

    // Find assets
    const Metadata metahelper(display_name(), std::move(possible_config_dirs));
    for (const SystemEntry& sysentry : systems) {
        metahelper.find_metadata_for(sysentry, sctx);

        progress += progress_step;
        emit progressChanged(progress);
    }

    return *this;
}

inputConfigEntry Es2Provider::load_input_data(const QString& DeviceName, const QString& DeviceGUID)
{   
    std::vector<QString> possible_config_dirs = [this]{
        const auto option_it = options().find(QStringLiteral("installdir"));
        return (option_it != options().cend())
            ? std::vector<QString>{ QDir::cleanPath(option_it->second.front()) + QLatin1Char('/') }
            : default_config_paths();
    }();

    // Find input
    const inputConfigEntry input = find_input(display_name(), possible_config_dirs,DeviceName, DeviceGUID);
    if (input.inputConfigAttributs.deviceName != DeviceName)
    {
        Log::error(display_name(), LOGMSG("%1 input configuration not found from es_input.cfg").arg(DeviceName));
    }
    else
    {
        Log::info(display_name(), LOGMSG("%1 input configuration found from es_input.cfg").arg(DeviceName));
    }
    return input;
}

} // namespace es2
} // namespace providers
