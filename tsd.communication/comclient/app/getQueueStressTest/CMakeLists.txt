FILE (GLOB_RECURSE SOURCES *.[c,h]pp *.h)
BUILD_APP(getQueueStressTest ${SOURCES})
