
#ifndef CXMLNODE_H
#define CXMLNODE_H

#include "quick3d_global.h"

// QT
#include <QString>
#include <QMap>
#include <QVector>
#include <QDomDocument>

class QUICK3D_EXPORT CXMLNode
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur par défaut
	CXMLNode();

	//! Destructeur
	virtual ~CXMLNode();

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	bool isEmpty() const { return m_sTag.isEmpty(); }

	//-------------------------------------------------------------------------------------------------
	// Méthode de contrôle
	//-------------------------------------------------------------------------------------------------

	//! Lit un noeud XML
	static CXMLNode parseXMLNode(QDomNode node);

	//! Lit un texte XML
	static CXMLNode parseXML(QString sText);

	//! Lit un fichier XML
	static CXMLNode loadFromFile(QString sFileName);

	//! Convertit en QString
	QString toString() const;

	//! Convertit en QDomDocument
	QDomDocument toQDomDocument() const;

	//! Convertit en QDomElement
	QDomElement toQDomElement(QDomDocument& xDocument) const;

	//! Retourne un noeud selon son tag
	CXMLNode getNodeByTagName(QString sTagName);

	//! Retourne une liste de noeuds selon leur tag
	QVector<CXMLNode> getNodesByTagName(QString sTagName);

	//-------------------------------------------------------------------------------------------------
	// Propriétés publiques
	//-------------------------------------------------------------------------------------------------

	QString					m_sTag;
	QString					m_sValue;
	QMap<QString, QString>	m_vAttributes;
	QVector<CXMLNode>		m_vNodes;
};

#endif // CXMLNODE_H
