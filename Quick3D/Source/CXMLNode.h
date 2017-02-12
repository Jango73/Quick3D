
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------
// Includes

// Qt
#include <QString>
#include <QMap>
#include <QVector>
#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//-------------------------------------------------------------------------------------------------

//! D�finit un noeud XML
class QUICK3D_EXPORT CXMLNode
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    //! Default constructor
    CXMLNode();

    //! Destructor
    //! Destructor
    virtual ~CXMLNode();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! D�finit le tag de ce noeud
    //! Defines this node's tag
    void setTag(const QString& value);

    //! D�finit la valeur de ce noeud
    //! Defines this node's value
    void setValue(const QString& value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne vrai si le neoud est vide (pas de tag)
    //! Returns true if the node is empty (no tag)
    bool isEmpty() const;

    //! Retourne le tag de ce noeud
    const QString& tag() const;

    //! Retourne la valeur de ce noeud
    const QString& value() const;

    //! Retourne les attributs de ce noeud
    const QMap<QString, QString>& attributes() const;

    //! Retourne les attributs de ce noeud
    QMap<QString, QString>& attributes();

    //! Retourne les noeuds enfants de ce noeud
    const QVector<CXMLNode>& nodes() const;

    //! Retourne les noeuds enfants de ce noeud
    QVector<CXMLNode>& nodes();

    //! Retourne un noeud selon son tag
    CXMLNode getNodeByTagName(QString sTagName);

    //! Retourne un noeud selon son tag
    CXMLNode getNodeByTagName(QString sTagName) const;

    //! Retourne une liste de noeuds selon leur tag
    QVector<CXMLNode> getNodesByTagName(QString sTagName) const;

    //-------------------------------------------------------------------------------------------------
    // Control methods haut niveau
    // High level control methods
    //-------------------------------------------------------------------------------------------------

    //! Charge un CXMLNode depuis un fichier XML ou JSON selon son extension
    //! Loads a CXMLNode from a XML or JSON file based on the extension
    static CXMLNode load(const QString& sFileName);

    //! Ecrit le contenu dans un fichier XML ou JSON selon son extension
    //! Saves content to a XML or JSON file based on the extension
    bool save(const QString& sFileName);

    //! Lit un fichier XML d'apr�s un nom de fichier
    //! Reads a XML file given a file name
    static CXMLNode loadXMLFromFile(const QString& sFileName);

    //! Lit un fichier XML d'apr�s un nom de fichier
    //! Reads a JSON file given a file name
    static CXMLNode loadJSONFromFile(const QString& sFileName);

    //! Convertit en QString
    //! Converts the document to a string
    QString toString() const;

    //! Ecrit un fichier XML
    //! Saves a XML file
    bool saveXMLToFile(const QString& sFileName);

    //! Ecrit un fichier XML
    //! Saves a XML file
    bool saveJSONToFile(const QString& sFileName);

    //-------------------------------------------------------------------------------------------------
    // Control methods bas niveau
    // Low level control methods
    //-------------------------------------------------------------------------------------------------

    //! Lit un noeud XML depuis un QDomNode
    static CXMLNode parseXMLNode(QDomNode node);

    //! Lit un texte XML depuis un QString
    static CXMLNode parseXML(QString sText);

    //! Lit un objet JSON
    //! Reads a JSON object
    static CXMLNode parseJSONNode(QJsonObject jObject, QString sTagName);

    //! Lit un tableau JSON
    //! Reads a JSON array
    static QVector<CXMLNode> parseJSONArray(QJsonArray jArray, QString sTagName);

    //! Lit un fichier JSON
    //! Reads a JSON file
    static CXMLNode parseJSON(QString sText);

    //! Convertit en QDomDocument
    //! Converts the node to a QDomDocument
    QDomDocument toQDomDocument() const;

    //! Convertit en QDomElement en utilisant 'xDocument'
    //! Converts the node to a QDomElement using 'xDocument'
    QDomElement toQDomElement(QDomDocument& xDocument) const;

    //! Convertit en QJsonDocument
    //! Converts the node to a QJsonDocument
    QJsonDocument toJsonDocument() const;

    //!
    QJsonObject toJsonObject() const;

    //! Fusionne le noeud 'target' avec ce noeud
    //! Merges the 'target' node into this node
    void merge(const CXMLNode& target);

    //-------------------------------------------------------------------------------------------------
    // Properties statiques publiques
    // Static public properties
    //-------------------------------------------------------------------------------------------------

public:

    static const QString sExtension_XML;
    static const QString sExtension_JSON;

    //-------------------------------------------------------------------------------------------------
    // Properties
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString					m_sTag;			// Tag du noeud - Node's tag
    QString					m_sValue;		// Valeur du noeud - Node's value
    QMap<QString, QString>	m_vAttributes;	// Attributs du noeuds - Node's attributes
    QVector<CXMLNode>		m_vNodes;		// Noeuds enfants - Child nodes
};
