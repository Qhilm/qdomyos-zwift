#ifndef TRAINPROGRAMMUTEXTESTSUITE_H
#define TRAINPROGRAMMUTEXTESTSUITE_H

#include "gtest/gtest.h"

class TrainProgramMutexTestSuite : public testing::Test {
  public:
    void test_schedulerAndClearRowsDoNotDeadlock();
};

TEST_F(TrainProgramMutexTestSuite, SchedulerAndClearRowsDoNotDeadlock) {
    this->test_schedulerAndClearRowsDoNotDeadlock();
}

#endif // TRAINPROGRAMMUTEXTESTSUITE_H
