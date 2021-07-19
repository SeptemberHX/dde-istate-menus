//
// Created by septemberhx on 2020/6/22.
//

#include <QWidget>
#include "DDESensorPlugin.h"

#define PLUGIN_STATE_KEY "enable"

DDESensorPlugin::DDESensorPlugin(QObject *parent) : QObject(parent)
{
    this->m_tipWidget = new QLabel();
    this->m_popupWidget = new IstateSensorWidget();
    this->m_pluginWidget = new DDESensorItemWidget();
}


void DDESensorPlugin::init(PluginProxyInterface *proxyInter) {
    m_proxyInter = proxyInter;

    if (pluginIsDisable()) {
        return;
    }
}

// --------------------------- CHANGE NAME --------------------------------

const QString DDESensorPlugin::pluginDisplayName() const {
    return QStringLiteral("DDE Istate Sensor");
}

const QString DDESensorPlugin::pluginName() const {
    return QStringLiteral("dde-istate-sensor");
}

// --------------------------- NO NEED TO CHANGE --------------------------------

int DDESensorPlugin::itemSortKey(const QString &itemKey) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    return m_proxyInter->getValue(this, key, 5).toInt();
}

void DDESensorPlugin::setSortKey(const QString &itemKey, const int order) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    m_proxyInter->saveValue(this, key, order);
}

QWidget *DDESensorPlugin::itemPopupApplet(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_popupWidget;
}

QWidget *DDESensorPlugin::itemTipsWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_tipWidget;
}

QWidget *DDESensorPlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_pluginWidget;
}

void DDESensorPlugin::pluginStateSwitched() {
    m_proxyInter->saveValue(this, PLUGIN_STATE_KEY, pluginIsDisable());

    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    if (m_pluginWidget) {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

void DDESensorPlugin::pluginSettingsChanged() {
    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    if (m_pluginWidget) {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

void DDESensorPlugin::refreshData() {

}

void DDESensorPlugin::setRefreshInterval(int msec) {

}

void DDESensorPlugin::setCpuTemp(qreal temp) {
    this->m_pluginWidget->setTemp(temp);
}

void DDESensorPlugin::updateTempInfos(QList<TempInfo> infoList) {
    this->m_popupWidget->updateTempInfos(infoList);
}

void DDESensorPlugin::updatePowerConsumption(PowerConsumption pc) {
    this->m_popupWidget->updatePowerConsumption(pc);
    this->m_tipWidget->setText(QString("Power: %1 W").arg(QString::number(pc.package * 1000 / pc.timestamp, 'f', 1)));
    this->m_pluginWidget->setPower(pc.package * 1000 / pc.timestamp);
}

void DDESensorPlugin::reloadSettings() {

}

void DDESensorPlugin::themeChanged(DGuiApplicationHelper::ColorType themeType) {
    QColor fontColor = Qt::white;
    switch (themeType) {
        case DGuiApplicationHelper::DarkType:
            this->m_pluginWidget->changeIconTheme(true);
            break;
        case DGuiApplicationHelper::LightType:
            this->m_pluginWidget->changeIconTheme(false);
            fontColor = Qt::black;
            break;
        default:
            break;
    }
    this->m_popupWidget->setStyleSheet(QString("QWidget { color: %1; }").arg(fontColor.name()));
}
