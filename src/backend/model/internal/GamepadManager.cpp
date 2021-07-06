// Pegasus Frontend
// Copyright (C) 2017-2019  Mátyás Mustoha
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


#include "GamepadManager.h"

#include "Log.h"
#include "ScriptRunner.h"

#ifdef WITH_SDL_GAMEPAD
#  include "GamepadManagerSDL2.h"
#else
#  include "GamepadManagerQt.h"
#endif


namespace {
void call_gamepad_reconfig_scripts()
{
    ScriptRunner::run(ScriptEvent::CONFIG_CHANGED);
    ScriptRunner::run(ScriptEvent::CONTROLS_CHANGED);
}

QQmlObjectListModel<model::Gamepad>::const_iterator
find_by_deviceid(QQmlObjectListModel<model::Gamepad>& model, int device_id)
{
    return std::find_if(
        model.constBegin(),
        model.constEnd(),
        [device_id](const model::Gamepad* const gp){ return gp->deviceId() == device_id; });
}

inline QString pretty_id(int device_id) {
    return QLatin1String("0x") % QString::number(device_id, 16);
}

QString getIconByName(QString name)
{
	//check if xbox or ps4 pad for surprise ;-)
	QString icon = "";
	if(name.contains("xbox", Qt::CaseInsensitive))
	{
		icon = "\uf2f0";
	}
	else if (name.contains("ps4", Qt::CaseInsensitive))
	{
		icon = "\uf2ca";
	}
	//but also for wheel ;-)
	else if ((name.contains("cockpit", Qt::CaseInsensitive)) || (name.contains("wheel", Qt::CaseInsensitive)))
	{
		icon = "\uf0c7";
	}
	return icon;
}

} // namespace


namespace model {

GamepadManager::GamepadManager(const backend::CliArgs& args, QObject* parent)
    : QObject(parent)
    , m_devices(new QQmlObjectListModel<model::Gamepad>(this))
    , m_log_tag(QStringLiteral("Gamepad"))
#ifdef WITH_SDL_GAMEPAD
    , m_backend(new GamepadManagerSDL2(this))
#else
    , m_backend(new GamepadManagerQt(this))
#endif
{
    connect(m_backend, &GamepadManagerBackend::connected,
            this, &GamepadManager::bkOnConnected);
    connect(m_backend, &GamepadManagerBackend::disconnected,
            this, &GamepadManager::bkOnDisconnected);
    connect(m_backend, &GamepadManagerBackend::nameChanged,
            this, &GamepadManager::bkOnNameChanged);
    connect(m_backend, &GamepadManagerBackend::removed,
            this, &GamepadManager::bkOnRemoved);
            

    connect(m_backend, &GamepadManagerBackend::buttonConfigured,
            this, &GamepadManager::bkOnButtonCfg);
    connect(m_backend, &GamepadManagerBackend::axisConfigured,
            this, &GamepadManager::bkOnAxisCfg);
    connect(m_backend, &GamepadManagerBackend::configurationCanceled,
            this, &GamepadManager::configurationCanceled);

    connect(m_backend, &GamepadManagerBackend::buttonChanged,
            this, &GamepadManager::bkOnButtonChanged);
    connect(m_backend, &GamepadManagerBackend::axisChanged,
            this, &GamepadManager::bkOnAxisChanged);

#ifndef Q_OS_ANDROID
    connect(m_backend, &GamepadManagerBackend::buttonChanged,
            &padbuttonnav, &GamepadButtonNavigation::onButtonChanged);
    connect(m_backend, &GamepadManagerBackend::axisChanged,
            &padaxisnav, &GamepadAxisNavigation::onAxisEvent);

    connect(&padaxisnav, &GamepadAxisNavigation::buttonChanged,
            &padbuttonnav, &GamepadButtonNavigation::onButtonChanged);
#endif // Q_OS_ANDROID

    m_backend->start(args);
}

void GamepadManager::configureButton(int deviceId, GMButton button)
{
    Q_ASSERT(button != GMButton::Invalid);
    m_backend->start_recording(deviceId, static_cast<GamepadButton>(button));
}
void GamepadManager::configureAxis(int deviceId, GMAxis axis)
{
    Q_ASSERT(axis != GMAxis::Invalid);
    m_backend->start_recording(deviceId, static_cast<GamepadAxis>(axis));
}
void GamepadManager::cancelConfiguration() {
    m_backend->cancel_recording();
}

void GamepadManager::bkOnConnected(int device_id, QString name)
{
    if (name.isEmpty())
        name = QLatin1String("generic");

    m_devices->append(new Gamepad(device_id, name, m_devices));

    Log::info(m_log_tag, LOGMSG("Connected device %1 (%2)").arg(pretty_id(device_id), name));
    
    //showpopup for 4 seconds by default
    emit showPopup(QStringLiteral("Device %1 connected").arg(QString::number(device_id)),QStringLiteral("%1").arg(name),QStringLiteral("%1").arg(getIconByName(name)), 4);
    
    emit connected(device_id);
    
}

void GamepadManager::bkOnDisconnected(int device_id)
{
    QString name;
    
    try{
        QString name;
        const auto it = find_by_deviceid(*m_devices, device_id);
        if (it != m_devices->constEnd()) {
            name = (*it)->name();
            //finally, remove device independently in a second time
        }
        Log::info(m_log_tag, LOGMSG("Disconnected device %1 (%2)").arg(pretty_id(device_id), name));

		//showpopup for 4 seconds by default
		emit showPopup(QStringLiteral("Device %1 disconnected").arg(QString::number(device_id)),QStringLiteral("%1").arg(name),QStringLiteral("%1").arg(getIconByName(name)), 4);		
		
        emit disconnected(std::move(name));
    }
    catch ( const std::exception & Exp ) 
    { 
        Log::debug(m_log_tag, LOGMSG("Catched error : %1.\n").arg(Exp.what()));
    } 
}

void GamepadManager::bkOnNameChanged(int device_id, QString name)
{
    //Log::info(m_log_tag, LOGMSG("void GamepadManager::bkOnNameChanged(int device_id, QString name)"));
    try{
        const auto it = find_by_deviceid(*m_devices, device_id);
        if (it != m_devices->constEnd()) {
            Log::debug(m_log_tag, LOGMSG("Set name of device %1 to '%2'").arg(pretty_id(device_id), name));
            (*it)->setName(std::move(name));
        }
    }
    catch ( const std::exception & Exp ) 
    { 
        Log::debug(m_log_tag, LOGMSG("Catched error : %1.\n").arg(Exp.what()));
    } 
}

void GamepadManager::bkOnRemoved(int device_id)
{
    //Log::info(m_log_tag, LOGMSG("void GamepadManager::bkOnRemoveDevice(int device_id)"));
    try{
        const auto it = find_by_deviceid(*m_devices, device_id);
        if (it != m_devices->constEnd()) 
        {
            QString name;
            name = (*it)->name();
            m_devices->remove(*it);
            Log::info(m_log_tag, LOGMSG("Remove device %1 (%2)").arg(pretty_id(device_id), name));
        }
    }
    catch ( const std::exception & Exp ) 
    { 
        Log::debug(m_log_tag, LOGMSG("Catched error : %1.\n").arg(Exp.what()));
    }     
}


void GamepadManager::bkOnButtonCfg(int device_id, GamepadButton button)
{
    call_gamepad_reconfig_scripts();
    emit buttonConfigured(device_id, static_cast<GMButton>(button));
}

void GamepadManager::bkOnAxisCfg(int device_id, GamepadAxis axis)
{
    call_gamepad_reconfig_scripts();
    emit axisConfigured(device_id, static_cast<GMAxis>(axis));
}

void GamepadManager::bkOnButtonChanged(int device_id, GamepadButton button, bool pressed)
{
    const auto it = find_by_deviceid(*m_devices, device_id);
    if (it != m_devices->constEnd())
        (*it)->setButtonState(button, pressed);
}

void GamepadManager::bkOnAxisChanged(int device_id, GamepadAxis axis, double value)
{
    const auto it = find_by_deviceid(*m_devices, device_id);
    if (it != m_devices->constEnd())
        (*it)->setAxisState(axis, value);
}

} // namespace model

