/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEVICE_H
#define DEVICE_H

#include <QString>

#include <pulse/volume.h>

#include "volumeobject.h"
#include "port.h"
#include "pulseobject.h"

namespace QPulseAudio
{

class Device : public VolumeObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString formFactor READ formFactor NOTIFY formFactorChanged)
    Q_PROPERTY(quint32 cardIndex READ cardIndex NOTIFY cardIndexChanged)
    Q_PROPERTY(QList<QObject *> ports READ ports NOTIFY portsChanged)
    Q_PROPERTY(quint32 activePortIndex READ activePortIndex WRITE setActivePortIndex NOTIFY activePortIndexChanged)
    Q_PROPERTY(bool default READ isDefault WRITE setDefault NOTIFY defaultChanged)
public:
    enum State {
        InvalidState = 0,
        RunningState,
        IdleState,
        SuspendedState,
        UnknownState
    };
    Q_ENUMS(State);

    virtual ~Device() {}

    template <typename PAInfo>
    void updateDevice(const PAInfo *info)
    {
        updateVolumeObject(info);

        if (m_name != info->name) {
            m_name = info->name;
            emit nameChanged();
        }
        if (m_description != info->description) {
            m_description = info->description;
            emit descriptionChanged();
        }
        const char *form_factor = pa_proplist_gets(info->proplist, PA_PROP_DEVICE_FORM_FACTOR);
        if (form_factor) {
            QString formFactor = QString::fromUtf8(form_factor);
            if (m_formFactor != formFactor) {
                m_formFactor = formFactor;
                emit formFactorChanged();
            }
        }

        m_cardIndex = info->card;
        emit cardIndexChanged();

        // TODO: this rebuilds the entire port list on every update. would be
        // nicer if it actually removed what needs removing updates what needs
        // updating and adds what needs adding. Alas, this is a tad more
        // involved.
        qDeleteAll(m_ports);
        m_ports.clear();
        for (auto **ports = info->ports; ports && *ports != nullptr; ++ports) {
            Port *port = new Port(this);
            port->setInfo(*ports);
            m_ports.append(port);
            if (info->active_port == *ports) {
                m_activePortIndex = m_ports.length() - 1;
            }
        }
        emit portsChanged();
        emit activePortIndexChanged();

        State infoState = stateFromPaState(info->state);
        if (infoState != m_state) {
            m_state = infoState;
            emit stateChanged();
        }
    }

    State state() const;
    QString name() const;
    QString description() const;
    QString formFactor() const;
    quint32 cardIndex() const;
    QList<QObject *> ports() const;
    quint32 activePortIndex() const;
    virtual void setActivePortIndex(quint32 port_index) = 0;
    virtual bool isDefault() const = 0;
    virtual void setDefault(bool enable) = 0;

signals:
    void stateChanged();
    void nameChanged();
    void descriptionChanged();
    void formFactorChanged();
    void cardIndexChanged();
    void portsChanged();
    void activePortIndexChanged();
    void defaultChanged();

protected:
    Device(QObject *parent);

private:
    State stateFromPaState(int value) const;

    QString m_name;
    QString m_description;
    QString m_formFactor;
    quint32 m_cardIndex = -1;
    QList<QObject *> m_ports;
    quint32 m_activePortIndex = -1;
    State m_state = UnknownState;
};

} // QPulseAudio

#endif // DEVICE_H
