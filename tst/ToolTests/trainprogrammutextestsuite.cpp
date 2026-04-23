#include "trainprogrammutextestsuite.h"

#include "Tools/testsettings.h"
#include "bluetooth.h"
#include "qzsettings.h"
#include "trainprogram.h"

#include <QElapsedTimer>
#include <QThread>
#include <atomic>
#include <thread>

void TrainProgramMutexTestSuite::test_schedulerAndClearRowsDoNotDeadlock() {
    TestSettings testSettings(QStringLiteral("qdomyos-zwift-tests"),
                              QStringLiteral("trainprogram-mutex-tests"));
    testSettings.activate();
    testSettings.qsettings.clear();
    testSettings.qsettings.setValue(QZSettings::zwift_username, QString());
    testSettings.qsettings.setValue(QZSettings::peloton_companion_workout_ocr, false);
    testSettings.qsettings.setValue(QZSettings::zwift_ocr, false);
    testSettings.qsettings.setValue(QZSettings::zwift_ocr_climb_portal, false);
    testSettings.qsettings.setValue(QZSettings::zwift_workout_ocr, false);
    testSettings.qsettings.setValue(QZSettings::peloton_workout_ocr, false);

    bluetooth *bt = new bluetooth(false, QString(), false, false, 200, false, false, 4, 1.0, false);
    trainprogram *program = new trainprogram(QList<trainrow>(), bt);

    std::atomic<bool> start(false);
    std::atomic<bool> schedulerDone(false);
    std::atomic<bool> clearRowsDone(false);

    std::thread schedulerThread([&]() {
        while (!start.load()) {
            std::this_thread::yield();
        }

        for (int i = 0; i < 1000; ++i) {
            program->scheduler();
        }
        schedulerDone.store(true);
    });

    std::thread clearRowsThread([&]() {
        while (!start.load()) {
            std::this_thread::yield();
        }

        for (int i = 0; i < 1000; ++i) {
            program->clearRows();
        }
        clearRowsDone.store(true);
    });

    start.store(true);

    QElapsedTimer timer;
    timer.start();
    while ((!schedulerDone.load() || !clearRowsDone.load()) && timer.elapsed() < 5000) {
        QThread::msleep(10);
    }

    if (!schedulerDone.load() || !clearRowsDone.load()) {
        schedulerThread.detach();
        clearRowsThread.detach();
        ADD_FAILURE() << "scheduler() and clearRows() did not finish within the timeout; possible deadlock";
        return;
    }

    schedulerThread.join();
    clearRowsThread.join();

    delete program;
    delete bt;
}
