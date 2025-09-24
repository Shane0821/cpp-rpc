#include "logger.h"

#include <gtest/gtest.h>

TEST(LoggerTest, SQPoll) {
    Logger::GetInst().init();
    for (int i = 0; i < (1 << 20); i++) {
        LOG_INFO("SQPoll Test INFO: {}", i);
        LOG_DEBUG("SQPoll Test DEBUG: {}", i);
        LOG_ERROR("SQPoll Test ERROR: {}", i);
    }
}