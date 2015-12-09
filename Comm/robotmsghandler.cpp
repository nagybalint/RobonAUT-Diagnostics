#include "robotmsghandler.h"
#include "robotmessage.h"
#include "robotterminalmessage.h"
#include "roboterrormessage.h"
#include <QDataStream>
#include <QTimer>
#include <QIODevice>
#include <memory>


RobotMsgHandler::RobotMsgHandler(CommSerial& serial) :
    currentParseState(RobotMsgHandler::ParseState::StartByte),
    messageIn(nullptr)
{
    listenOn(serial);
}

void RobotMsgHandler::listenOn(CommSerial& serial) {
    this->serial = &serial;
    QObject::connect(this->serial, SIGNAL(dataAvailable(QDataStream&)), this, SLOT(dataAvailable(QDataStream&)));
}

void RobotMsgHandler::dataAvailable(QDataStream &inStream) {

    quint8 byteIn;
    QIODevice* socket = inStream.device();
    while(socket->bytesAvailable()) {
        switch (currentParseState) {

        case RobotMsgHandler::ParseState::StartByte:
            inStream >> byteIn;
            if(byteIn == DEBUG_MSG_START_CODE)
                this->currentParseState = ParseState::MsgType;
            break;

        case RobotMsgHandler::ParseState::MsgType:
            inStream >> byteIn;
            currentParseState = RobotMsgHandler::ParseState::Parsing;
            switch (RobotMessage::getType(byteIn)) {
            case RobotMessage::Type::Terminal:
                messageIn = std::make_unique<RobotTerminalMessage>();
                break;
            case (RobotMessage::Type::Error):
                messageIn = std::make_unique<RobotErrorMessage>();
                break;
            default:
                // TODO emit error message
                // Unknown message type, prepare for new message
                currentParseState = RobotMsgHandler::ParseState::StartByte;
                break;
            }
            break;

        case RobotMsgHandler::ParseState::Parsing:
            if(!messageIn->parseMessage(inStream)) {
                emitSignal(messageIn.get());
                currentParseState = RobotMsgHandler::ParseState::EndByte;
            }

        case RobotMsgHandler::ParseState::EndByte:
            inStream >> byteIn;
            if(byteIn != DEBUG_MSG_END_CODE) {
                ; // TODO emit error
            }
            currentParseState = RobotMsgHandler::ParseState::StartByte;

        default:
            break;
        }
    }
}

void RobotMsgHandler::emitSignal(RobotMessage* msg) {

    switch (msg->type) {
    case RobotMessage::Type::Terminal:
        emit this->terminalMessageReceived(((RobotTerminalMessage*) msg)->getMessage());
        break;
    default:
        break;
    }

}

void RobotMsgHandler::sendTerminalMsg(QString& command) {

    RobotTerminalMessage msg(command);
    this->serial->send(msg);
}
