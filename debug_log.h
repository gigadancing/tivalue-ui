#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <stdlib.h>
#include <QtDebug>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

/*
#if _MSC_VER < 1400                // 版本较低时，不支持...类型的宏，屏蔽代码LogPrintf (vc60, vc70)
#define LogPrintf //
#else                              // 1400之后的版本支持...类型的宏，此时来区分_Debug和_Release模式
#ifdef _DEBUG
#define DLOG(msg)\
		{\
			qDebug() << msg;\
		}
#define DLOG_QT_WALLET(msg, value)\
		{\
			qDebug() << msg << value;\
		}
#define WLOG_QT_WALLET(msg, ...)\
		{\
			qWarning() << format << ##__VA_ARGS__;\
		}
#define DLOG_QT_WALLET_FUNCTION_BEGIN \
		{\
			qDebug("Function:%s Begin", __FUNCTION__);\
		}
#define DLOG_QT_WALLET_FUNCTION_END \
		{\
			qDebug("Function:%s End",__FUNCTION__);\
		}
#else
#define DLOG_QT_WALLET //
#define DLOG_QT_WALLET_FUNCTION_BEGIN //
#define DLOG_QT_WALLET_FUNCTION_END //
#endif
#endif
*/

//#define RELEASE1

#if _MSC_VER < 1400                // 版本较低时，不支持...类型的宏，屏蔽代码LogPrintf (vc60, vc70)
#define LogPrintf //
#else                              // 1400之后的版本支持...类型的宏，此时来区分_Debug和_Release模式
#ifdef RELEASE1
#define DLOG_QT_WALLET(format, ...)\
                {\
        qDebug("File:%s, Line:%d, Function:%s " format, __FILE__,  __LINE__, __FUNCTION__, ##__VA_ARGS__);\
                }
#define DLOG_QT_WALLET_FUNCTION_BEGIN \
                {\
        qDebug("File:%s, Line:%d, Function:%s Begin", __FILE__,  __LINE__, __FUNCTION__);\
                }
#define DLOG_QT_WALLET_FUNCTION_END \
                {\
        qDebug("File:%s, Line:%d, Function:%s End", __FILE__,  __LINE__, __FUNCTION__);\
                }
#else
#define DLOG_QT_WALLET //
#define DLOG_QT_WALLET_FUNCTION_BEGIN //
#define DLOG_QT_WALLET_FUNCTION_END //
#endif
#endif



void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // DEBUG_H

