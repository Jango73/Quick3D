
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

//-------------------------------------------------------------------------------------------------

template <class T>
class CRollingAverager
{
public:

    //! Constructeur par défaut
    CRollingAverager(int numSamples)
        : m_iNumSamples(numSamples)
        , m_tAverage((T) 0)
    {
    }

    //! Destructeur
    virtual ~CRollingAverager()
    {
    }

    //! Remize à zéro des valeurs
    void reset()
    {
        m_tAverage = (T) 0;
    }

    //! Ajoute une valeur
    void append(const T& value)
    {
        m_tAverage -= m_tAverage / m_iNumSamples;
        m_tAverage += value / m_iNumSamples;
    }

    //! Retourne la moyenne de toutes les valeurs
    T getAverage() const
    {
        return m_tAverage;
    }

protected:

    int     m_iNumSamples;
    T       m_tAverage;
};

#endif // CAVERAGER_H
