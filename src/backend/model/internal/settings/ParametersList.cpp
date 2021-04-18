#include "ParametersList.h"
#include "Log.h"
#include "Recalbox.h"
#include "audio/AudioController.h"

namespace {

/******************************* section to initial variables used by GetParametersList in same name *************************************/
QStringList ListOfInternalValue;

QStringList GetParametersList(QString Parameter)
{
    QStringList ListOfValue;
    
    //clean global internal values if needed
    ListOfInternalValue.clear();
    
    if (Parameter == "global.ratio")
    {   
        //## Set ratio for all emulators (auto,4/3,16/9,16/10,custom) - default value: auto / index 0
        //global.ratio=auto
        ListOfValue << "none" << "auto" << "4/3" << "16/9" << "16/10" << "16/15" << "21/9" << "1/1" << "2/1" << "3/2" << "3/4" << "4/1" << "9/16" << "5/4" << "6/5" << "7/9" << "8/3" << "8/7" << "19/12" << "19/14" << "30/17" << "32/9" << "squarepixel" << "config" << "custom" << "coreprovided";
    }
    else if (Parameter == "system.kblayout")
    {   
        //## set the keyboard layout (fr,en,de,us,es)
        //system.kblayout=us
        ListOfValue << "fr" << "en" << "de" << "us" << "es";
    }
    else if (Parameter == "global.shaderset")
    {
        //## Shader set
        //## Automatically select shaders for all systems
        //## (none, retro, scanlines)
        //global.shaderset=none
        ListOfValue << "none" << "retro" << "scanline";
    }
    else if ((Parameter == "netplay.password.client") || (Parameter == "netplay.password.viewer"))
    {
        //global.netplay.nickname= ?
        ListOfValue << "|P/4/C-M/4/N|" << "[SpAcE.iNvAdErS]" << ">sUpEr.MaRi0.bRoSs<" << "{SoNiC.tHe.HeDgEhOg}" << "(Q/B/E/R/T-@;&?@#)" << "~AnOtHeR.wOrLd!~" << "(/T\\E/T\\R/I\\S)" << "$m00n.p4tR0I$" << "*M.E.T.A.L.S.L.U.G*" << "OuTruN-hAn60uT" << "[L*E*M*M*I*N*G*S]" << "@-G|a|U|n|L|e|T-@" << "%.BuBBLe.B00Ble.%" << "!.CaStLeVaNiA.!" << "=B@mBeR.J4cK=";
    }
    else if (Parameter == "audio.mode")
    {
        //## 2 values for the moment are really manage for pegasus
        //audio.mode = musicandvideosound -> to activate sound for all using the Display value : "video and game"
        //or
        //audio.mode = none -> to deactivate sound
        
        //##RFU / activated only for 
        //MusicsOnly,
        //VideosSoundOnly,
        //MusicsXorVideosSound,
        
        //pegasus sound layer parameters is not as ES.
        ListOfValue << "sound activated" <<  "mute";
        //use internal values to match with ES modes
        ListOfInternalValue << "musicandvideosound" <<  "none";
    }
    else if (Parameter == "audio.device")
    {
        //example in conf: 
        //audio.device=alsa_card.pci-0000_00_1f.3:hdmi-output-0
        //audio.volume=90
        //audio.bgmusic=1
        //audio.mode=musicandvideosound

        IAudioController::DeviceList playbackList = AudioController::Instance().GetPlaybackList();
        
        for(const auto& playback : playbackList)
           {
               Log::debug(LOGMSG("Audio device DisplayableName : '%1'").arg(QString::fromStdString(playback.DisplayableName)));
               ListOfValue.append(QString::fromStdString(playback.DisplayableName));
               
               Log::debug(LOGMSG("Audio device InternalName : '%1'").arg(QString::fromStdString(playback.InternalName)));
               ListOfInternalValue.append(QString::fromStdString(playback.InternalName));
           }
        if(ListOfValue.isEmpty())
           {
               ListOfValue.append("no device detected");
               ListOfInternalValue.append(""); //to empty parameter
           }            
    }
    else
    {
        ListOfValue << QString("error: Parameters list for '%1' not found").arg(Parameter);
        //not a parameter using list !
        Log::warning(LOGMSG("'%1' parameter is not a parameters list").arg(Parameter));
    }
    Log::debug(LOGMSG("The list of value for '%1' is '%2'.").arg(Parameter,ListOfValue.join(",")));
    return ListOfValue;
}

std::vector<model::ParameterEntry> find_available_parameterslist(const QString& Parameter)
{
    //Log::debug(LOGMSG("Call of std::vector<model::ParameterEntry> find_available_parameterslist(const QString& Parameter)"));
    
    const QStringList ListOfValue = GetParametersList(Parameter);

    std::vector<model::ParameterEntry> parameterslist;

    //Log::debug(LOGMSG("ListOfValue.count():`%1`").arg(ListOfValue.count()));
    
    parameterslist.reserve(static_cast<size_t>(ListOfValue.count()));

    for (const QString& name : qAsConst(ListOfValue)) {
        //Log::debug(LOGMSG("name `%1`").arg(name));
        parameterslist.emplace_back(std::move(name));
        //Log::debug(LOGMSG("Found parameter `%1`").arg(parameterslist.back().name));
    }
    return parameterslist;
}

} // namespace

namespace model {

ParameterEntry::ParameterEntry(QString Name)
    : name(std::move(Name))
{}

ParametersList::ParametersList(QObject* parent)
    : QAbstractListModel(parent)
    , m_role_names({
        { Roles::Name, QByteArrayLiteral("name") },
    })
{
    //empty constructor to be generic
}


void ParametersList::select_preferred_parameter(const QString& Parameter)
{
    //to get first row as default value
    const QString DefaultValue = m_parameterslist.at(0).name; 
    //check in recalbox.conf
    select_parameter(QString::fromStdString(RecalboxConf::Instance().AsString(Parameter.toUtf8().constData(),DefaultValue.toUtf8().constData())));  
}

bool ParametersList::select_parameter(const QString& name)
{
    //Log::debug(LOGMSG("ParametersList::select_parameter(const QString& name) name:`%1`").arg(name));
    if (name.isEmpty())
        return false;

    for (size_t idx = 0; idx < m_parameterslist.size(); idx++) {
        //Log::debug(LOGMSG("idx:`%1`").arg(idx));
        //Log::debug(LOGMSG("at(idx).name:`%1`").arg(m_parameterslist.at(idx).name));
        if (ListOfInternalValue.size() == 0)
        {
            if (m_parameterslist.at(idx).name == name) {
                m_current_idx = idx;
                //Log::debug(LOGMSG("m_current_idx:`%1`").arg(m_current_idx));
                return true;
            }
        }
        else // if internal value to check index from recalbox.conf stored value
        {
            if (ListOfInternalValue.at(idx) == name) {
                m_current_idx = idx;
                //Log::debug(LOGMSG("m_current_idx:`%1`").arg(m_current_idx));
                return true;
            }
        }        
    }
    //Log::debug(LOGMSG("ParametersList::select_parameter(const QString& name) / return false"));
    return false;
}

void ParametersList::load_selected_parameter()
{
    const auto& value = m_parameterslist.at(m_current_idx);
    //Log::debug(LOGMSG("ParametersList::load_selected_parameter() - parameter: `%1`").arg(value.name));
        
    //write parameter in recalbox.conf in all cases
    if (ListOfInternalValue.size() == 0) RecalboxConf::Instance().SetString(m_parameter.toUtf8().constData(), value.name.toUtf8().constData());
    //or internal value
    else RecalboxConf::Instance().SetString(m_parameter.toUtf8().constData(), ListOfInternalValue.at(m_current_idx).toUtf8().constData());
    
    //Check m_parameter to manage specific case with specific management/action
    if(m_parameter == "audio.device")
    {
        //change audio device as selected 
        std::string originalAudioDevice = RecalboxConf::Instance().GetAudioOuput();
        std::string fixedAudioDevice = AudioController::Instance().SetDefaultPlayback(originalAudioDevice);
    }
    else if(m_parameter == "audio.mode")
    {
       //change audio mode as selected 
       if(ListOfInternalValue.at(m_current_idx).toUtf8().constData() == "none")
       {
       //mute audio
       //TO DO
       }
       else
       {
       //unmute audio
       //TO DO
       }
    }
}

int ParametersList::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(m_parameterslist.size());
}

QVariant ParametersList::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || rowCount() <= index.row())
        return {};

    const auto& parameter = m_parameterslist.at(static_cast<size_t>(index.row()));
    switch (role) {
        case Roles::Name:
            return parameter.name;
        default:
            return {};
    }
}

void ParametersList::setCurrentIndex(int idx_int)
{
    const auto idx = static_cast<size_t>(idx_int);

    // verify
    if (idx == m_current_idx)
        return;

    if (m_parameterslist.size() <= idx) {
        Log::warning(LOGMSG("Invalid parameter index #%1").arg(idx));
        return;
    }
    // load
    m_current_idx = idx;
    load_selected_parameter();
    //Log::debug(LOGMSG("emit parameterChanged();"));
    emit parameterChanged();
}


QString ParametersList::currentName(const QString& Parameter) { 
        
        //Log::debug(LOGMSG("QString ParametersList::currentName(const QString& Parameter) - parameter: `%1`").arg(Parameter));
        
        if (m_parameter != Parameter)
        {
            //to signal refresh of model's data
            emit QAbstractItemModel::beginResetModel();
            m_parameter = Parameter;
            m_parameterslist = find_available_parameterslist(Parameter);
            select_preferred_parameter(Parameter);
            load_selected_parameter();
            //to signal end of model's data
            emit QAbstractItemModel::endResetModel();
        }
        return m_parameterslist.at(m_current_idx).name; 
}

} // namespace model
