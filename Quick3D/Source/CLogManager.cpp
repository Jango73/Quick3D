
// Qt
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

// Application
#include "CPreferencesManager.h"
#include "CLogManager.h"
#include "CQ3DConstants.h"

//-------------------------------------------------------------------------------------------------
// Taille maximum d'un fichier log

#define LOGGER_MAX_FILE_INDEX	5
#define LOGGER_MAX_FILE_SIZE	(10 * 1024 * 1024)	// 4 mb

//-------------------------------------------------------------------------------------------------

CLogManager::CLogManager()
: m_tMutex(QMutex::Recursive)
, m_tTimer(this)
, m_tFlushTimer(this)
, m_pFile(NULL)
, m_iFileSize(0)
, m_eLogLevel(llDebug)
, m_bBackupActive(false)
{
	QString sName = QCoreApplication::applicationFilePath().split("/").last();
	QString sPath = QCoreApplication::applicationDirPath() + "/Logs";
	QString sFinal = sName + ".log";

	initialize(
		sPath,
		sFinal,
		CPreferencesManager::getInstance()->getPreferences().getNodeByTagName("Logger")
	);

	connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	connect(&m_tFlushTimer, SIGNAL(timeout()), this, SLOT(onFlushTimeout()));

	m_tTimer.start(1000);
	m_tFlushTimer.start(2000);
}

//-------------------------------------------------------------------------------------------------

CLogManager::CLogManager(QString sPath, QString sFileName)
: m_tMutex(QMutex::Recursive)
, m_pFile(NULL)
, m_iFileSize(0)
, m_eLogLevel(llDebug)
{
	initialize(sPath, sFileName, CPreferencesManager::getInstance()->getPreferences());

	connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));

	m_tTimer.start(1000);
}

//-------------------------------------------------------------------------------------------------

void CLogManager::registerChainedLogger(ILogger* pLogger)
{
	if (m_vChainedLoggers.contains(pLogger) == false)
	{
		m_vChainedLoggers.append(pLogger);
	}
}

//-------------------------------------------------------------------------------------------------

CLogManager::~CLogManager()
{
	// !!!! NE PAS FAIRE DE LOG DANS LE DESTRUCTEUR DU LOGGER !!!!

	QMutexLocker locker(&m_tMutex);

	if (m_pFile)
	{
		if (m_pFile->isOpen())
		{
			m_pFile->close();
		}

		delete m_pFile;
	}
}

//-------------------------------------------------------------------------------------------------

QString CLogManager::getPathName() const
{
	return m_sPathName;
}

//-------------------------------------------------------------------------------------------------

QString CLogManager::getFileName() const
{
	return m_sFileName;
}

//-------------------------------------------------------------------------------------------------

void CLogManager::initialize(QString sPath, QString sFileName, CXMLNode xParameters)
{
#ifndef NO_LOGGING

	QMutexLocker locker(&m_tMutex);

	CXMLNode xTokensNode = xParameters.getNodeByTagName("Tokens");
	CXMLNode xBackupNode = xParameters.getNodeByTagName("Backup");

	// Lecture des paramètres
	if (xParameters.m_vAttributes[ParamName_Level].isEmpty() == false)
	{
		setLevel(xParameters.m_vAttributes[ParamName_Level]);
	}

	if (xTokensNode.m_vAttributes[ParamName_Display].isEmpty() == false)
	{
		setDisplayTokens(xTokensNode.m_vAttributes[ParamName_Display]);
	}

	if (xTokensNode.m_vAttributes[ParamName_Ignore].isEmpty() == false)
	{
		setIgnoreTokens(xTokensNode.m_vAttributes[ParamName_Ignore]);
	}

	if (xBackupNode.m_vAttributes[ParamName_Active].isEmpty() == false)
	{
		m_bBackupActive = (bool) xBackupNode.m_vAttributes[ParamName_Active].toInt();
	}

	// Assignation du nom de fichier
	m_sPathName = sPath;
	m_sFileName = sPath + "/" + sFileName;

	// Création du répertoire s'il n'existe pas
	if (QDir().exists(sPath) == false)
	{
		QDir().mkpath(sPath);
	}

	// Backups
	manageBackups(sFileName);

	// Destruction du fichier
	if (m_pFile)
	{
		if (m_pFile->isOpen())
		{
			m_pFile->close();
		}
		delete m_pFile;
	}

	// Création du fichier
	m_pFile = new QFile(m_sFileName);
	m_pFile->open(QIODevice::WriteOnly | QIODevice::Text);
	m_iFileSize = 0;

#endif
}

//-------------------------------------------------------------------------------------------------

void CLogManager::manageBackups(const QString& sFileName)
{
	if (m_bBackupActive)
	{
		QDateTime tTime = QDateTime::currentDateTime();

		QString sCopyFileName = QString("%1.%2-%3-%4_%5-%6-%7.bak")
			.arg(sFileName)
			.arg(tTime.date().year())
			.arg(tTime.date().month())
			.arg(tTime.date().day())
			.arg(tTime.time().hour())
			.arg(tTime.time().minute())
			.arg(tTime.time().second());

		QFile tFile(sFileName);

		if (tFile.exists()) tFile.copy(sCopyFileName);
	}
}

//-------------------------------------------------------------------------------------------------

void CLogManager::setLevel(ELogLevel eLevel)
{
	QMutexLocker locker(&m_tMutex);

	m_eLogLevel = eLevel;
}

//-------------------------------------------------------------------------------------------------

void CLogManager::setLevel(const QString& sLevel)
{
	QMutexLocker locker(&m_tMutex);

	if (sLevel.toLower() == "debug")
	{
		setLevel(llDebug);
	}
	else if (sLevel.toLower() == "info")
	{
		setLevel(llInfo);
	}
	else if (sLevel.toLower() == "warning")
	{
		setLevel(llWarning);
	}
	else if (sLevel.toLower() == "error")
	{
		setLevel(llError);
	}
	else if (sLevel.toLower() == "critical")
	{
		setLevel(llCritical);
	}
	else if (sLevel.toLower() == "always")
	{
		setLevel(llAlways);
	}
}

//-------------------------------------------------------------------------------------------------

void CLogManager::setDisplayTokens(const QString& sTokens)
{
	if (sTokens == "")
	{
		m_sDisplayTokens.clear();
	}
	else
	{
		m_sDisplayTokens = sTokens.split("|");
	}
}

//-------------------------------------------------------------------------------------------------

void CLogManager::setIgnoreTokens(const QString& sTokens)
{
	if (sTokens == "")
	{
		m_sIgnoreTokens.clear();
	}
	else
	{
		m_sIgnoreTokens = sTokens.split("|");
	}
}

//-------------------------------------------------------------------------------------------------

QString CLogManager::getShortStringForLevel(ELogLevel eLevel, const QString& sText)
{
	QString sLogLevel;

	switch (eLevel)
	{
		case llDebug	: sLogLevel = "DEBUG"; break;
		case llInfo		: sLogLevel = "INFO"; break;
		case llWarning	: sLogLevel = "WARNING"; break;
		case llError	: sLogLevel = "ERROR"; break;
		case llCritical	: sLogLevel = "CRITICAL"; break;
		case llAlways	: sLogLevel = "ALWAYS"; break;
	}

	QString sFinalText = QString("[%1] - %2\n")
		.arg(sLogLevel)
		.arg(sText);

	return sFinalText;
}

//-------------------------------------------------------------------------------------------------

QString CLogManager::getFinalStringForLevel(ELogLevel eLevel, const QString& sText)
{
	QString sLogLevel;

	switch (eLevel)
	{
		case llDebug	: sLogLevel = "DEBUG"; break;
		case llInfo		: sLogLevel = "INFO"; break;
		case llWarning	: sLogLevel = "WARNING"; break;
		case llError	: sLogLevel = "ERROR"; break;
		case llCritical	: sLogLevel = "CRITICAL"; break;
		case llAlways	: sLogLevel = "ALWAYS"; break;
	}

	QDateTime tNow = QDateTime::currentDateTime();

	QString sFinalText = QString("%1-%2-%3 %4:%5:%6.%7 [%8] - %9\n")
		.arg(tNow.date().year())
		.arg(tNow.date().month())
		.arg(tNow.date().day())
		.arg(tNow.time().hour())
		.arg(tNow.time().minute())
		.arg(tNow.time().second())
		.arg(tNow.time().msec())
		.arg(sLogLevel)
		.arg(sText);

	return sFinalText;
}

//-------------------------------------------------------------------------------------------------

bool CLogManager::filterToken(QString sToken)
{
	if (m_sDisplayTokens.size() > 0)
	{
		bool bFound = false;

		foreach (QString sDisplay, m_sDisplayTokens)
		{
			if (sToken.contains(sDisplay, Qt::CaseInsensitive))
			{
				bFound = true;
				break;
			}
		}

		return bFound;
	}

	if (m_sIgnoreTokens.size() > 0)
	{
		bool bFound = false;

		foreach (QString sDisplay, m_sIgnoreTokens)
		{
			if (sToken.contains(sDisplay, Qt::CaseInsensitive))
			{
				bFound = true;
				break;
			}
		}

		return !bFound;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------

void CLogManager::log(ELogLevel eLevel, const QString& sText, const QString& sToken)
{
	QMutexLocker locker(&m_tMutex);
	QString sFinalText = sText;

	// Est-ce que le token passe?
	if (filterToken(sToken) == false) return;

	// Ajout du token dans le texte
	if (sToken != "")
	{
		sFinalText = "<" + sToken + "> " + sText;
	}

	// Remplacement de noms indésirables
	sFinalText = sFinalText.replace("\"", "'");

	if (eLevel >= llError)
	{
		qDebug() << getShortStringForLevel(eLevel, sFinalText).toLatin1().constData();
	}

	if (m_pFile && m_pFile->isOpen())
	{
		switch (eLevel)
		{
			case llDebug :
			{
				if (m_eLogLevel <= llDebug)
				{
					m_iFileSize += m_pFile->write(getFinalStringForLevel(llDebug, sFinalText).toLatin1().constData());
				}
			}
			break;

			case llInfo :
			{
				if (m_eLogLevel <= llInfo)
				{
					m_iFileSize += m_pFile->write(getFinalStringForLevel(llInfo, sFinalText).toLatin1().constData());
				}
			}
			break;

			case llWarning :
			{
				if (m_eLogLevel <= llWarning)
				{
					m_iFileSize += m_pFile->write(getFinalStringForLevel(llWarning, sFinalText).toLatin1().constData());

					foreach(ILogger *pLogger, m_vChainedLoggers) { pLogger->log(eLevel, sFinalText, sToken); }
				}
			}
			break;

			case llError :
			{
				if (m_eLogLevel <= llError)
				{
					m_iFileSize += m_pFile->write(getFinalStringForLevel(llError, sFinalText).toLatin1().constData());

					foreach(ILogger *pLogger, m_vChainedLoggers) { pLogger->log(eLevel, sFinalText, sToken); }
				}
			}
			break;

			case llCritical :
			{
				if (m_eLogLevel <= llCritical)
				{
					m_iFileSize += m_pFile->write(getFinalStringForLevel(llCritical, sFinalText).toLatin1().constData());

					foreach(ILogger *pLogger, m_vChainedLoggers) { pLogger->log(eLevel, sFinalText, sToken); }
				}
			}
			break;

			case llAlways :
			{
				if (m_eLogLevel <= llAlways)
				{
					m_iFileSize += m_pFile->write(getFinalStringForLevel(llAlways, sFinalText).toLatin1().constData());

					foreach(ILogger *pLogger, m_vChainedLoggers) { pLogger->log(eLevel, sFinalText, sToken); }
				}
			}
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CLogManager::flush()
{
	QMutexLocker locker(&m_tMutex);

	if (m_pFile != NULL)
	{
		m_pFile->flush();
	}
}

//-------------------------------------------------------------------------------------------------

void CLogManager::logBuffer(ELogLevel eLevel, const char* pBuffer, int iSize)
{
	QMutexLocker locker(&m_tMutex);

	const char* pPointer = pBuffer;

	log(eLevel, "Buffer :");

	while (pPointer < pBuffer + iSize)
	{
		QString sText = "";

		for (int iIndex = 0; iIndex < 16 && (pPointer < pBuffer + iSize); iIndex++)
		{
			if (sText != "") sText += " ";
			sText += QString("0x%1").arg(*((const unsigned char*) pPointer), 2 , 16 , QChar('0'));
			pPointer++;
		}

		log(eLevel, sText);
	}
}

//-------------------------------------------------------------------------------------------------

void CLogManager::onTimeout()
{
	QMutexLocker locker(&m_tMutex);

	if (m_pFile != NULL)
	{
		if (m_iFileSize > LOGGER_MAX_FILE_SIZE)
		{
			// Destruction du fichier
			m_pFile->close();
			delete m_pFile;

			handleRollingCopies();

			// Création du fichier
			m_pFile = new QFile(m_sFileName);
			m_pFile->open(QIODevice::WriteOnly | QIODevice::Text);
			m_iFileSize = 0;
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CLogManager::onFlushTimeout()
{
	flush();
}

//-------------------------------------------------------------------------------------------------

void CLogManager::handleRollingCopies()
{
	bool bCopied = false;

	// Recherche d'un nom de fichier libre pour copie
	for (int iIndex = 0; iIndex < LOGGER_MAX_FILE_INDEX; iIndex++)
	{
		QString sCopyFileName(m_sFileName + "." + QString::number(iIndex));

		QFile tCopyFile(sCopyFileName);

		if (tCopyFile.exists() == false)
		{
			bCopied = true;

			QFile tOriginalFile(m_sFileName);
			tOriginalFile.copy(sCopyFileName);

			break;
		}
	}

	if (bCopied == false)
	{
		for (int iIndex = 0; iIndex < LOGGER_MAX_FILE_INDEX - 1; iIndex++)
		{
			QString sSourceFileName(m_sFileName + "." + QString::number(iIndex + 1));
			QString sTargetFileName(m_sFileName + "." + QString::number(iIndex + 0));

			QFile tTargetFile(sTargetFileName);
			tTargetFile.remove();

			QFile tSourceFile(sSourceFileName);
			tSourceFile.copy(sTargetFileName);
		}

		QString sCopyFileName(m_sFileName + "." + QString::number(LOGGER_MAX_FILE_INDEX - 1));

		QFile tOriginalFile(m_sFileName);
		tOriginalFile.copy(sCopyFileName);
	}
}
