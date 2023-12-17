pegasus_add_provider(
        NAME "Conjure"
        CXXID CONJURE
        SOURCES
        ConjureFilter.cpp
        ConjureFilter.h
        ConjureMetadata.cpp
        ConjureMetadata.h
        ConjureProvider.cpp
        ConjureProvider.h
        USES_JSON_CACHE
        PLATFORMS
        WINDOWS
)
