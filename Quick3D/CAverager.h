//---------------------------------------------------------------------------------------------------------
// 
//
// Logiciel 	: BARIER
// Module		: AtsUtilities
// Classe    	: CAverager
// Fichier		: CAverager.h
// 
// Auteur		: Guillaume DARIER
// Date			: 03/11/2011
//
// Description	:
//
//---------------------------------------------------------------------------------------------------------
// Révisions :
// NumRevision Date_Révision Auteur_Révision Description_Révision
//
//---------------------------------------------------------------------------------------------------------

#ifndef CAVERAGER_H
#define CAVERAGER_H

// QT
#include <QObject>
#include <QDateTime>
#include <QDir>
#include <QFile>

//-------------------------------------------------------------------------------------------------

template <class T>
class CAverager
{
public:

	//! Constructeur par défaut
	CAverager(int iMaxValues = 5)
	: m_iMaxValues(iMaxValues)
	{
	}

	//! Destructeur
	virtual ~CAverager()
	{
	}

	//! Remize à zéro des valeurs
	void reset()
	{
		m_mValues.clear();
	}

	//! Ajoute une valeur
	void append(const T& value)
	{
		m_mValues.append(value);

		if (m_mValues.count() > m_iMaxValues)
		{
			m_mValues.remove(0);
		}
	}

	//! Retourne la moyenne de toutes les valeurs
	T getAverage() const
	{
		if (m_mValues.count() == 0)
		{
			return (T) 0;
		}

		T returnValue = (T) 0;

		foreach (T iterValue, m_mValues)
		{
			returnValue += iterValue;
		}

		return returnValue / (T) m_mValues.count();
	}

protected:

	int			m_iMaxValues;			// Nombre maximum de valeurs
	QVector<T>	m_mValues;				// Les valeurs à moyenner
};

#endif // CAVERAGER_H
