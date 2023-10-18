TEMPLATE = subdirs

SUBDIRS += \
    pegasus \
    pegasus_media \
    emulationstation \
    favorites \
    logiqx \
    playtime \
    conjure \

win32: SUBDIRS += \
    launchbox \
    playnite \

