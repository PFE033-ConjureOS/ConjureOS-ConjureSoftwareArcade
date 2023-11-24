pegasus_cxx_test(test_ConjureProvider)

qtquick_compiler_add_resources(TEST_RESOURCES data/data.qrc)
target_sources(test_ConjureProvider PRIVATE ${TEST_RESOURCES})
