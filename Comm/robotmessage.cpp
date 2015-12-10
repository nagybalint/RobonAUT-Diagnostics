#include "robotmessage.h"

RobotMessage::RobotMessage()
{ }

RobotMessage::Type RobotMessage::getType(quint8 modeID) {
    switch (modeID) {
    case (quint8) RobotMessage::Type::Terminal:
        return RobotMessage::Type::Terminal;
    case (quint8) RobotMessage::Type::Config:
        return RobotMessage::Type::Config;
    case (quint8) RobotMessage::Type::Error:
        return RobotMessage::Type::Error;
    case (quint8) RobotMessage::Type::Status:
        return RobotMessage::Type::Status;
    case (quint8) RobotMessage::Type::StatusRequest:
        return RobotMessage::Type::StatusRequest;
    case (quint8) RobotMessage::Type::SetSpeed:
        return RobotMessage::Type::SetSpeed;
    case (quint8) RobotMessage::Type::SetServo:
        return RobotMessage::Type::SetServo;
    default:
        break;
    }
}
