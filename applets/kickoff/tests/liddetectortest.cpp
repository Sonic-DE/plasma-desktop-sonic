// SPDX-License-Identifier: GPL-2.0-or-later

#include "liddetector.h"

#include <QDBusConnection>
#include <QSignalSpy>
#include <QTest>

class PowerManagementMock : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Solid.PowerManagement")

public Q_SLOTS:
    bool isLidPresent() const
    {
        return lidPresent;
    }

public:
    bool lidPresent = true;
};

class LidDetectorTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void readsLidStateFromPowerDevil()
    {
        PowerManagementMock mock;
        QDBusConnection bus = QDBusConnection::sessionBus();
        QVERIFY(bus.registerObject(QStringLiteral("/org/kde/Solid/PowerManagement"), &mock, QDBusConnection::ExportAllSlots));
        QVERIFY(bus.registerService(QStringLiteral("org.kde.Solid.PowerManagement")));

        LidDetector detector;
        QSignalSpy spy(&detector, &LidDetector::lidPresentChanged);
        QTRY_VERIFY_WITH_TIMEOUT(detector.lidPresent(), 5000);
        QCOMPARE(spy.count(), 1);

        QVERIFY(bus.unregisterService(QStringLiteral("org.kde.Solid.PowerManagement")));
        QTRY_VERIFY_WITH_TIMEOUT(!detector.available(), 5000);
        QVERIFY(!detector.lidPresent());
        bus.unregisterObject(QStringLiteral("/org/kde/Solid/PowerManagement"));
    }

    void recoversWhenPowerDevilStartsAndRestarts()
    {
        PowerManagementMock mock;
        mock.lidPresent = false;
        auto bus = QDBusConnection::sessionBus();

        LidDetector detector;
        QVERIFY(!detector.available());
        QVERIFY(!detector.lidPresent());

        QVERIFY(bus.registerObject(QStringLiteral("/org/kde/Solid/PowerManagement"), &mock, QDBusConnection::ExportAllSlots));
        QVERIFY(bus.registerService(QStringLiteral("org.kde.Solid.PowerManagement")));
        QTRY_VERIFY_WITH_TIMEOUT(detector.available(), 5000);
        QVERIFY(!detector.lidPresent());

        QVERIFY(bus.unregisterService(QStringLiteral("org.kde.Solid.PowerManagement")));
        QTRY_VERIFY_WITH_TIMEOUT(!detector.available(), 5000);
        mock.lidPresent = true;
        QVERIFY(bus.registerService(QStringLiteral("org.kde.Solid.PowerManagement")));
        QTRY_VERIFY_WITH_TIMEOUT(detector.lidPresent(), 5000);
        QVERIFY(detector.available());

        QVERIFY(bus.unregisterService(QStringLiteral("org.kde.Solid.PowerManagement")));
        bus.unregisterObject(QStringLiteral("/org/kde/Solid/PowerManagement"));
    }
};

QTEST_GUILESS_MAIN(LidDetectorTest)

#include "liddetectortest.moc"
