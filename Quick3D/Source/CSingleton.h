
#ifndef CSINGLETON_H
#define CSINGLETON_H

// QT
#include <QtCore/QMutex>

//-------------------------------------------------------------------------------------------------
//! Classe singleton.
template<class T>
class CSingleton
{
public:

	virtual void initInstance()
	{
		getInstance();
	}

	//! Accesseur a l'instance de la classe.
	static T* getInstance()
	{
		QMutexLocker locker(&s_mutex);
		if (s_pInstance == NULL)
		{
			s_pInstance = new T();
		}
		return s_pInstance;
    }

    static bool isInstanciated()
    {
        return s_pInstance != NULL;
    }

	//! Destructeur de l'instance de la classe.
	static void killInstance()
	{
		QMutexLocker locker(&s_mutex);
		if (s_pInstance) delete s_pInstance;
		s_pInstance = 0;
    }

protected:
	CSingleton() {}; // hide constructor
	virtual ~CSingleton() {}; // hide destructor
	
private:
	static T* s_pInstance;
	static QMutex s_mutex;
};

template<class T> T* CSingleton<T>::s_pInstance = NULL;
template<class T> QMutex CSingleton<T>::s_mutex(QMutex::Recursive);

#endif // CSINGLETON_H
