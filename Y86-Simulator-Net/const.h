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
#include <QMutex>
#include <QWaitCondition>
#include <QFuture>
#include <QtConcurrent>

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

#define CLOCK_FOR_FETCH 42335
#define CLOCK_FOR_Decode 42337
#define CLOCK_FOR_Execute 42339
#define CLOCK_FOR_Memory 42341
#define CLOCK_FOR_Writeabck 42343


#endif // CONST_H
