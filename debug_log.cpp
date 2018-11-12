#include "debug_log.h"
#include "tic.h"

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
	mutex.lock();

	QString text;
	switch (type)
	{
	case QtDebugMsg:
		text = QString("Debug:");
		break;

	case QtWarningMsg:
		text = QString("Warning:");
		break;

	case QtCriticalMsg:
		text = QString("Critical:");
		break;

	case QtFatalMsg:
		text = QString("Fatal:");
	}

	//QString context_info = QString("File:(%1) Line:(%2) FUN:(%3)").arg(QString(context.file)).arg(context.line).arg(context.function);
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
	QString current_date = QString("(%1)").arg(current_date_time);
	QString message = QString("%1 %2 %3").arg(text).arg(current_date).arg(msg);

    QFile file( Fry::getInstance()->walletConfigPath + "/log.txt");

    if(file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        if( file.size() > 1000000)
        {
            file.resize(0);
        }

        QTextStream text_stream(&file);
        text_stream << message << "\r\n";
        file.flush();
        file.close();
    }

	mutex.unlock();
}
