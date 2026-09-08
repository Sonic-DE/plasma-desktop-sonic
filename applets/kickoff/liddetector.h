/*
 *  SPDX-FileCopyrightText: 2025 SonicDE
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LIDDETECTOR_H
#define LIDDETECTOR_H

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

class LidDetector : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool lidPresent READ lidPresent NOTIFY lidPresentChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)

public:
    explicit LidDetector(QObject *parent = nullptr);
    ~LidDetector() override;

    bool lidPresent() const;
    bool available() const;

Q_SIGNALS:
    void lidPresentChanged();
    void availableChanged();

private:
    void queryLidPresent();
    void onServiceOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner);
    void setLidPresent(bool present);
    void setAvailable(bool available);

    bool m_lidPresent = false;
    bool m_available = false;
    QString m_serviceOwner;
    quint64 m_generation = 0;
    quint64 m_querySerial = 0;
};

#endif // LIDDETECTOR_H
