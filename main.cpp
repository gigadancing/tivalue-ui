#include <QApplication>
#include <windows.h>
#include <QDebug>
#include <qapplication.h>
#include <QTranslator>
#include <QThread>
#include <QTextCodec>
#include <QDir>
#include "DbgHelp.h"
#include "tchar.h"
#include "ShlObj.h"

#include "tic.h"
#include "frame.h"
#include "debug_log.h"

bool checkOnly()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    //  创建互斥量
    HANDLE m_hMutex  =  CreateMutex(NULL, FALSE,  L"TVWallet" );
    //  检查错误代码
    if  (GetLastError()  ==  ERROR_ALREADY_EXISTS)  {
      //  如果已有互斥量存在则释放句柄并复位互斥量
     CloseHandle(m_hMutex);
     m_hMutex  =  NULL;
      //  程序退出
      return  false;
    }
    else
        return true;
}

LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    qDebug() << "Enter TopLevelExceptionFilter Function" ;
    HANDLE hFile = CreateFile(  _T(L"project.dmp"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;
    stExceptionParam.ThreadId    = GetCurrentThreadId();
    stExceptionParam.ExceptionPointers = pExceptionInfo;
    stExceptionParam.ClientPointers    = FALSE;
    MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpWithFullMemory,&stExceptionParam,NULL,NULL);
    CloseHandle(hFile);

    qDebug() << "End TopLevelExceptionFilter Function" ;
    return EXCEPTION_EXECUTE_HANDLER;
}

LPWSTR ConvertCharToLPWSTR(const char * szString)
{
    int dwLen = strlen(szString) + 1;
    int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度
    LPWSTR lpszPath = new WCHAR[dwLen];
    MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
    return lpszPath;
}

void refreshIcon()
{
    // 解决windows下自动更新后图标不改变的bug
    SHChangeNotify(0x8000000, 0x1000, 0, 0);
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, NULL, SMTO_ABORTIFHUNG, 100, 0);
}


void removeUpdateDeleteFile()   // 删除更新时应该删除的文件
{
    QDir dir;
    QStringList fileList = dir.entryList();
    foreach ( QString fileName, fileList)
    {
        if( fileName.endsWith(".delete"))
        {
            qDebug() << "remove delete file: " << fileName << QFile::remove(fileName);
        }
    }
}


int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

    QApplication a(argc, argv);

    refreshIcon();

    SetUnhandledExceptionFilter(TopLevelExceptionFilter);

//    QTranslator translator;
//    translator.load(QString(":/qm/qt_zh_cn"));
//    a.installTranslator(&translator);
    Fry::getInstance();  // 在frame创建前先创建实例，读取language

//    qInstallMessageHandler(outputMessage);  // 重定向qebug 到log.txt
    removeUpdateDeleteFile();

    if(checkOnly()==false)  return 0;    // 防止程序多次启动

    Frame frame;
    Fry::getInstance()->mainFrame = &frame;   // 一个全局的指向主窗口的指针
    frame.show();


    a.installEventFilter(&frame);

    a.setStyleSheet("QScrollBar:vertical{width:8px;background:transparent;padding:19px 0px 19px 0px;}"
                    "QScrollBar::handle:vertical{width:8px;background:rgb(44,46,50);border-radius:4px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(64,66,70);}"
                    "QScrollBar::add-line:vertical{height:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;}"
                    "QScrollBar::sub-page:vertical{background: transparent;}"
                    "QScrollBar::add-page:vertical{background: transparent;}"
                    "QScrollBar:horizontal{height:8px;background:rgb(255,255,255);padding:0px 19px 0px 19px;}"
                    "QScrollBar::handle:horizontal{height:8px;background:rgb(44,46,50);border-radius:4px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(64,66,70);}"
                    "QScrollBar::add-line:horizontal{width:0px;}"
                    "QScrollBar::sub-line:horizontal{width:0px;}"
                    );



    int result = a.exec();
    Fry::getInstance()->quit();
    DLOG_QT_WALLET_FUNCTION_END;
    return result;
}
