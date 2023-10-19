pegasus_add_provider(
        NAME "Conjure"
        CXXID STEAM
        SOURCES
        ConjureGamelist.cpp
        ConjureGamelist.h
        ConjureMetadata.cpp
        ConjureMetadata.h
        ConjureProvider.cpp
        ConjureProvider.h
        USES_JSON_CACHE
        PLATFORMS
        WINDOWS
)
