#include "logger.h"

#include <gtest/gtest.h>

TEST(LoggerTest, SQPoll) {
    Logger::GetInst().init();
    for (int i = 0; i < 65536; i++) {
        LOG_INFO("SQPoll Test INFO: %d", i);
        LOG_DEBUG("SQPoll Test DEBUG: %d", i);
        LOG_ERROR("SQPoll Test ERROR: %d", i);
    }
}