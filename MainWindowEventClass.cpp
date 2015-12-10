#include "MainWindowEventClass.h"
#include <QQmlContext>
#include "RobotState/robotstate.h"
#include <QDebug>

MainWindowEventClass::MainWindowEventClass(QQmlContext &context, RobotStateHistory &hState) :
    qmlContext(context), history(hState)
{
    QObject::connect(&this->history, SIGNAL(historyChanged()),this, SLOT(graphChanged()));
    qmlContext.setContextProperty(QStringLiteral("graphFrontSensorPos"), QVariant::fromValue(0));
    qmlContext.setContextProperty(QStringLiteral("graphOrientationAngle"), QVariant::fromValue(0));
    qmlContext.setContextProperty(QStringLiteral("graphBatContrV"), QVariant::fromValue(0));
    qmlContext.setContextProperty(QStringLiteral("graphBatMotorV"), QVariant::fromValue(0));
}

void MainWindowEventClass::graphChanged() {

    qmlContext.setContextProperty(QStringLiteral("graphFrontSensorPos"), QVariant::fromValue((qreal)history.currentState->lineSensor.getFrontLinePos()));
    qmlContext.setContextProperty(QStringLiteral("graphOrientationAngle"), QVariant::fromValue((qreal)1));
    qmlContext.setContextProperty(QStringLiteral("graphBatContrV"), QVariant::fromValue((qreal)history.currentState->batVoltage3S));
    qmlContext.setContextProperty(QStringLiteral("graphBatMotorV"), QVariant::fromValue((qreal)history.currentState->batVoltage2S));

    emit graphContextUpdated();
}

void MainWindowEventClass::startCommand() {
    qDebug() << "Start command arrived";
}

void MainWindowEventClass::stopCommand() {
    qDebug() << "Stop comand arrived";
}

QQuickItem* MainWindowEventClass::FindItemByName(QList<QObject *> nodes, const QString &name) {

    for(int i = 0; i < nodes.size(); i++)
    {
        if (nodes.at(i) && nodes.at(i)->objectName() == name)
        {
            return dynamic_cast<QQuickItem*>(nodes.at(i));
        }
        else if (nodes.at(i) && nodes.at(i)->children().size() > 0)
        {
            QQuickItem* item = FindItemByName(nodes.at(i)->children(), name);
            if (item)
                return item;
        }
    }
    return nullptr;
}

QQuickItem* MainWindowEventClass::FindItemByName(QObject *rootObject, const QString &name) {
    return FindItemByName(rootObject->children(), name);
}

void MainWindowEventClass::connectQmlSignals(QObject *rootObject, QString str) {

    QQuickItem *historyGraph = FindItemByName(rootObject,str);
    if (historyGraph)
    {
        QObject::connect(this, SIGNAL(graphContextUpdated()), historyGraph, SLOT(requestPaint()));
    }
    else
    {
        qDebug() << "Error!!! Can't find QML object.";
    }
}

