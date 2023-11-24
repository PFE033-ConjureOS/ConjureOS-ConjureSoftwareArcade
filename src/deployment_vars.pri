# This file sets the default values of the deployment variables;
# these control where will your files end up when you call
# `make install` (or similar).
#
# You can change the values by appending KEY=VALUE pairs to the
# `qmake` call, or adding them as `Additional arguments` in
# Qt Creator.


# The base/fallback installation directory
isEmpty(INSTALLDIR) {
    unix:!macx: INSTALLDIR = /opt/conjure-os
    macx: INSTALLDIR = /usr/local/conjure-os
    win32: INSTALLDIR = C:/conjure-os
}

# Linux: installations are portable by default
# Windows and Mac: installations should always be portable (one-dir)
isEmpty(INSTALL_BINDIR): INSTALL_BINDIR = $${INSTALLDIR}
