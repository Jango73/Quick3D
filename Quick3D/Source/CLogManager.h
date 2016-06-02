
#ifndef CLOGMANAGER_H
#define CLOGMANAGER_H

#include "quick3d_global.h"

// QT
#include <QObject>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTimer>
#include <QVector>

// Application
#include "ILogger.h"
#include "CSingleton.h"
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------
// Utiliser les macros suivantes pour logger

#ifdef LINUX
	#define __FILE_NOPATH__		(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
	#define __FILE_NOPATH__		(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#ifndef NO_LOGGING
	#define LOG_DEBUG(a)			CLogManager::getInstance()->log(llDebug,    a,__FILE_NOPATH__)
	#define LOG_INFO(a)				CLogManager::getInstance()->log(llInfo,     a,__FILE_NOPATH__)
	#define LOG_WARNING(a)			CLogManager::getInstance()->log(llWarning,  a,__FILE_NOPATH__)
	#define LOG_ERROR(a)			CLogManager::getInstance()->log(llError,    a,__FILE_NOPATH__)
	#define LOG_CRITICAL(a)			CLogManager::getInstance()->log(llCritical, a,__FILE_NOPATH__)
	#define LOG_ALWAYS(a)			CLogManager::getInstance()->log(llAlways,   a,__FILE_NOPATH__)
	#define LOG_BUFFER(l,b,s)		CLogManager::getInstance()->logBuffer (l,b,s)

	#define LOG_DEBUG_TOKEN(a,b)	CLogManager::getInstance()->log(llDebug,a,b)
	#define LOG_INFO_TOKEN(a,b)		CLogManager::getInstance()->log(llInfo,a,b)
	#define LOG_WARNING_TOKEN(a,b)	CLogManager::getInstance()->log(llWarning,a,b)
	#define LOG_ERROR_TOKEN(a,b)	CLogManager::getInstance()->log(llError,a,b)
	#define LOG_CRITICAL_TOKEN(a)	CLogManager::getInstance()->log(llCritical,a,b)
	#define LOG_ALWAYS_TOKEN(a,b)	CLogManager::getInstance()->log(llAlways,a,b)
#else
	#define LOG_DEBUG(a)			
	#define LOG_INFO(a)				
	#define LOG_WARNING(a)			
	#define LOG_ERROR(a)			
	#define LOG_CRITICAL(a)			
	#define LOG_ALWAYS(a)
	#define LOG_BUFFER(l,b,s)		

	#define LOG_DEBUG_TOKEN(a,b)	
	#define LOG_INFO_TOKEN(a,b)		
	#define LOG_WARNING_TOKEN(a,b)	
	#define LOG_ERROR_TOKEN(a,b)	
	#define LOG_CRITICAL_TOKEN(a,b)			
	#define LOG_ALWAYS_TOKEN(a,b)
	#define LOG_CRITICAL_TOKEN(a,b)	
#endif

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CLogManager : public QObject, public CSingleton<CLogManager>
{
	Q_OBJECT

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur par d�faut
	CLogManager();

	//! Constructeur avec nom de fichier
	CLogManager(QString sPath, QString sFileName);

	//! Destructeur
	virtual ~CLogManager();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//! D�finit le niveau de log
	virtual void setLevel(const QString& sLevel);

	//! D�finit le niveau de log
	virtual void setLevel(ELogLevel eLevel);

	//! D�finit la liste de tokens � logger
	void setDisplayTokens(const QString& sTokens);

	//! D�finit la liste de tokens � ignorer
	void setIgnoreTokens(const QString& sTokens);

	//! Teste si le token doit �tre logg� ou pas
	bool filterToken(QString sToken);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne le chemin du fichier log
	QString getPathName() const;

	//! Retourne le nom de fichier log
	QString getFileName() const;

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//! Initialise le logger
	virtual void initialize(QString sPath, QString sFileName, CXMLNode xParameters);

	//! Cr��e les backups si n�cessaire
	void manageBackups(const QString& sFileName);

	//! D�finit un logger cha�n�
	void registerChainedLogger(ILogger *logger);

	//! Log d'une ligne de texte
	virtual void log(ELogLevel eLevel, const QString& sText, const QString& sToken = "");
	virtual void logBuffer(ELogLevel eLevel, const char* pBuffer, int iSize);

	virtual void flush(); // Permet de forcer l'�criture du log sur le disque (� ne faire que dans des cas exceptionnels)

	//-------------------------------------------------------------------------------------------------
	// M�thodes prot�g�es
	//-------------------------------------------------------------------------------------------------

protected:

	//! Retourne une cha�ne de caract�re finale courte � �crire en fichier
	QString getShortStringForLevel(ELogLevel eLevel, const QString& sText);

	//! Retourne une cha�ne de caract�re finale � �crire en fichier
	QString getFinalStringForLevel(ELogLevel eLevel, const QString& sText);

	//!
	void handleRollingCopies();

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	QMutex					m_tMutex;
	QTimer					m_tTimer;
	QTimer					m_tFlushTimer;
	QString					m_sPathName;
	QString					m_sFileName;
	QFile*					m_pFile;
	qint64					m_iFileSize;
	ELogLevel				m_eLogLevel;
	QStringList				m_sDisplayTokens;
	QStringList				m_sIgnoreTokens;
	QVector<ILogger *>		m_vChainedLoggers;
	bool					m_bBackupActive;

	//-------------------------------------------------------------------------------------------------
	// Slots prot�g�s
	//-------------------------------------------------------------------------------------------------

protected slots:

	//!
	void onTimeout();

	//!
	void onFlushTimeout();
};

#endif // CLOGMANAGER_H
