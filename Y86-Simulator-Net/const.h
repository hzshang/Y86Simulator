#ifndef CONST_H
#define CONST_H
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QRunnable>
#include <QThreadPool>
#include <QString>
#include <QJsonArray>
#include <QThread>
#include <QMap>

#define Y86PORT 42313
#define DECODE_FOR_FETCH_PORT 42317
#define EXECUTE_FOR_DECODE_PORT 42319
#define EXECUTE_FOR_FETCH_PORT 42333
#define MEMORY_FOR_EXECUTE_PORT 42321
#define MEMORY_FOR_DECODE_PORT 42323
#define MEMORY_FOR_FETCH_PORT 42325

#define WRITEBACK_FOR_FETCH_PORT 42327
#define WRITEBACK_FOR_DECODE_PORT 42331
#define WRITEBACK_FOR_MEMORY_PORT 42329

#define CLOCK_PORT 42333


#endif // CONST_H
