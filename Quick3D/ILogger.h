
#ifndef ILOGGER_H
#define ILOGGER_H

enum ELogLevel
{
	llDebug,
	llInfo,
	llWarning,
	llError,
	llCritical,
	llAlways
};

//! Définit une interface de log
class ILogger
{
public:

	virtual void log(ELogLevel eLevel, const QString& sText, const QString& sToken = "") = 0;
	virtual void logBuffer(ELogLevel eLevel, const char* pBuffer, int iSize) = 0;
};

#endif // ILOGGER_H
