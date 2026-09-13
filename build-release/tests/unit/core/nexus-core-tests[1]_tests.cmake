add_test([=[BootstrapSmokeTest.ReturnsTrue]=]  D:/NumeroNexus/ChaosNexus/build-release/tests/unit/core/Debug/nexus-core-tests.exe [==[--gtest_filter=BootstrapSmokeTest.ReturnsTrue]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[BootstrapSmokeTest.ReturnsTrue]=]  PROPERTIES DEF_SOURCE_LINE [==[D:\NumeroNexus\ChaosNexus\tests\unit\core\bootstrap_test.cpp:12]==] WORKING_DIRECTORY D:/NumeroNexus/ChaosNexus/build-release/tests/unit/core SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  nexus-core-tests_TESTS BootstrapSmokeTest.ReturnsTrue)
