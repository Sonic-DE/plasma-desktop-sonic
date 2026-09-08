/*
 *  SPDX-FileCopyrightText: 2025 SonicDE
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "liddetector.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusServiceWatcher>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_KICKOFF_LID, "org.kde.plasma.kickoff.lid")

static const QString s_service = QStringLiteral("org.kde.Solid.PowerManagement");
static const QString s_path = QStringLiteral("/org/kde/Solid/PowerManagement");
static const QString s_interface = QStringLiteral("org.kde.Solid.PowerManagement");

LidDetector::LidDetector(QObject *parent)
    : QObject(parent)
{
    auto bus = QDBusConnection::sessionBus();
    m_serviceOwner = bus.interface()->serviceOwner(s_service).value();
    auto *serviceWatcher = new QDBusServiceWatcher(s_service, bus, QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &LidDetector::onServiceOwnerChanged);
    queryLidPresent();
}

LidDetector::~LidDetector() = default;

bool LidDetector::lidPresent() const
{
    return m_lidPresent;
}

bool LidDetector::available() const
{
    return m_available;
}

void LidDetector::queryLidPresent()
{
    if (m_serviceOwner.isEmpty()) {
        setAvailable(false);
        setLidPresent(false);
        return;
    }

    const quint64 generation = m_generation;
    const quint64 serial = ++m_querySerial;
    const QString owner = m_serviceOwner;
    QDBusMessage msg = QDBusMessage::createMethodCall(owner, s_path, s_interface, QStringLiteral("isLidPresent"));
    auto *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(msg), this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, generation, serial, owner](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<bool> reply(*w);
        w->deleteLater();
        if (generation != m_generation || serial != m_querySerial || owner != m_serviceOwner) {
            return;
        }
        if (reply.isError()) {
            qCWarning(LOG_KICKOFF_LID).nospace().noquote() << "Failed to query lid state: "
                                                           << "org.kde.Solid.PowerManagement"
                                                           << " /org/kde/Solid/PowerManagement"
                                                           << " isLidPresent: " << reply.error().name() << ": " << reply.error().message();
            setAvailable(false);
            setLidPresent(false);
        } else {
            setAvailable(true);
            setLidPresent(reply.value());
        }
    });
}

void LidDetector::onServiceOwnerChanged(const QString &, const QString &, const QString &newOwner)
{
    ++m_generation;
    ++m_querySerial;
    m_serviceOwner = newOwner;
    if (newOwner.isEmpty()) {
        setAvailable(false);
        setLidPresent(false);
        return;
    }
    queryLidPresent();
}

void LidDetector::setLidPresent(bool present)
{
    if (m_lidPresent == present) {
        return;
    }
    m_lidPresent = present;
    Q_EMIT lidPresentChanged();
}

void LidDetector::setAvailable(bool available)
{
    if (m_available == available) {
        return;
    }
    m_available = available;
    Q_EMIT availableChanged();
}
