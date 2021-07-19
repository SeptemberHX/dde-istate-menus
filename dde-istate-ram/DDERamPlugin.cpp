//
// Created by septemberhx on 2020/6/13.
//

#include "DDERamPlugin.h"

#define PLUGIN_STATE_KEY "enable"

DDERamPlugin::DDERamPlugin(QObject *parent)
    : QObject(parent)
{
    this->m_tipWidget = new QLabel();
    this->m_popupWidget = new IstateRamWidget();
    this->m_pluginWidget = new DDERamItemWidget();
}


void DDERamPlugin::init(PluginProxyInterface *proxyInter) {
    m_proxyInter = proxyInter;

    if (pluginIsDisable()) {
        return;
    }

    // todo
}

void DDERamPlugin::updateRamInfo(qreal usedPercent, mem_stat memStat) {
    this->m_pluginWidget->setPercent(usedPercent);
    this->m_popupWidget->updateMemStat(memStat);

    if (memStat.swap_total_kb == 0) {
        this->m_tipWidget->setText(QString("RAM: %1/%2")
            .arg(this->engLocale.formattedDataSize(memStat.mem_total_kb * (usedPercent / 100) * 1024, 2, QLocale::DataSizeTraditionalFormat))
            .arg(this->engLocale.formattedDataSize(memStat.mem_total_kb * 1024, 2, QLocale::DataSizeTraditionalFormat))
        );
    } else {
        this->m_tipWidget->setText(QString(" RAM: %1/%2\nSWAP: %3/%4")
           .arg(this->engLocale.formattedDataSize(memStat.mem_total_kb * (usedPercent / 100) * 1024, 2, QLocale::DataSizeTraditionalFormat))
           .arg(this->engLocale.formattedDataSize(memStat.mem_total_kb * 1024, 2, QLocale::DataSizeTraditionalFormat))
           .arg(this->engLocale.formattedDataSize((memStat.swap_total_kb - memStat.swap_free_kb) * 1024, 2, QLocale::DataSizeTraditionalFormat))
           .arg(this->engLocale.formattedDataSize(memStat.swap_total_kb * 1024, 2, QLocale::DataSizeTraditionalFormat))
        );
    }
}


// --------------------------- CHANGE NAME --------------------------------

const QString DDERamPlugin::pluginDisplayName() const {
    return QStringLiteral("DDE Istate RAM");
}

const QString DDERamPlugin::pluginName() const {
    return QStringLiteral("dde-istate-ram");
}

// --------------------------- NO NEED TO CHANGE --------------------------------

int DDERamPlugin::itemSortKey(const QString &itemKey) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    return m_proxyInter->getValue(this, key, 5).toInt();
}

void DDERamPlugin::setSortKey(const QString &itemKey, const int order) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    m_proxyInter->saveValue(this, key, order);
}

QWidget *DDERamPlugin::itemPopupApplet(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_popupWidget;
}

QWidget *DDERamPlugin::itemTipsWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_tipWidget;
}

QWidget *DDERamPlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_pluginWidget;
}

void DDERamPlugin::pluginStateSwitched() {
    m_proxyInter->saveValue(this, PLUGIN_STATE_KEY, pluginIsDisable());

    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    if (m_pluginWidget) {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

void DDERamPlugin::pluginSettingsChanged() {
    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    if (m_pluginWidget) {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

void DDERamPlugin::refreshData() {

}

void DDERamPlugin::setRefreshInterval(int msec) {

}

void DDERamPlugin::updateProcesses(QList<ProcessEntry> processEntries) {
    this->m_popupWidget->updateProcessList(processEntries);
}

void DDERamPlugin::reloadSettings() {

}

void DDERamPlugin::themeChanged(DGuiApplicationHelper::ColorType themeType) {
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
