
// Qt
#include <QFile>

// Library
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CXMLNode
    \inmodule Quick3D
    \brief A simple XML class, based on QDomDocument and QJsonDocument.
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CXMLNode.
*/
CXMLNode::CXMLNode()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CXMLNode.
*/
CXMLNode::~CXMLNode()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the tag of this node to \a value.
*/
void CXMLNode::setTag(const QString& value)
{
    m_sTag = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the value of this node to \a value.
*/
void CXMLNode::setValue(const QString& value)
{
    m_sValue = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if the node's tag is empty.
*/
bool CXMLNode::isEmpty() const
{
    return m_sTag.isEmpty();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the tag of this node.
*/
const QString& CXMLNode::tag() const
{
    return m_sTag;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the value of this node.
*/
const QString& CXMLNode::value() const
{
    return m_sValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a constant map of this node's attributes.
*/
const QMap<QString, QString>& CXMLNode::attributes() const
{
    return m_vAttributes;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a map of this node's attributes.
*/
QMap<QString, QString>& CXMLNode::attributes()
{
    return m_vAttributes;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a constant vector of this node's children.
*/
const QVector<CXMLNode>& CXMLNode::nodes() const
{
    return m_vNodes;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a vector of this node's children.
*/
QVector<CXMLNode>& CXMLNode::nodes()
{
    return m_vNodes;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads a CXMLNode hierarchy from the XML file named \a sFileName.
*/
CXMLNode CXMLNode::loadXMLFromFile(const QString& sFileName)
{
    QFile xmlFile(sFileName);

    if (xmlFile.exists())
    {
        if (xmlFile.open(QIODevice::ReadOnly))
        {
            QString sText = QString(xmlFile.readAll());
            xmlFile.close();

            return parseXML(sText);
        }
    }

    return CXMLNode();
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads a CXMLNode hierarchy from the JSON file named \a sFileName.
*/
CXMLNode CXMLNode::loadJSONFromFile(const QString& sFileName)
{
    QFile jsonFile(sFileName);

    if (jsonFile.exists())
    {
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QString sText = QString(jsonFile.readAll());
            jsonFile.close();

            return parseJSON(sText);
        }
    }

    return CXMLNode();
}

//-------------------------------------------------------------------------------------------------

/*!
    Parses a xml tree from the \a node.
*/
CXMLNode CXMLNode::parseXMLNode(QDomNode node)
{
    CXMLNode tNode;

    tNode.m_sTag = node.nodeName();
    tNode.m_sValue = node.nodeValue();

    for (int Index = 0; Index < node.attributes().length(); Index++)
    {
        QDomNode attrNode = node.attributes().item(Index);

        tNode.m_vAttributes[attrNode.nodeName()] = attrNode.nodeValue();
    }

    if (node.childNodes().length() == 1)
    {
        if (node.childNodes().at(0).nodeName().startsWith("#text"))
        {
            tNode.m_sValue = node.childNodes().at(0).nodeValue();
        }
        else
        {
            tNode.m_vNodes.append(CXMLNode::parseXMLNode(node.childNodes().at(0)));
        }
    }
    else
    {
        for (int Index = 0; Index < node.childNodes().length(); Index++)
        {
            tNode.m_vNodes.append(CXMLNode::parseXMLNode(node.childNodes().at(Index)));
        }
    }

    return tNode;
}

//-------------------------------------------------------------------------------------------------

/*!
    Parses a xml tree from the \a sText string.
*/
CXMLNode CXMLNode::parseXML(QString sText)
{
    CXMLNode tNode;

    if (sText.isEmpty() == false)
    {
        QDomDocument doc("myDocument");

        if (doc.setContent(sText) == true)
        {
            tNode = CXMLNode::parseXMLNode(doc.documentElement());
        }
    }

    return tNode;
}

//-------------------------------------------------------------------------------------------------

/*!
    Parses a JSON node from \a jObject, using \a sTagName as a tag name.
*/
CXMLNode CXMLNode::parseJSONNode(QJsonObject jObject, QString sTagName)
{
    CXMLNode tNode;

    tNode.m_sTag = sTagName;
    tNode.m_sValue = "";

    if (tNode.m_sTag.isEmpty())
    {
        tNode.m_sTag = QString("NOTAG");
    }

    foreach(QString sKey, jObject.keys())
    {
        if (jObject[sKey].isObject())
        {
            tNode.m_vNodes.append(CXMLNode::parseJSONNode(jObject[sKey].toObject(), sKey));
        }
        else if (jObject[sKey].isArray())
        {
            QVector<CXMLNode> vNodes = parseJSONArray(jObject[sKey].toArray(), sKey);

            foreach(CXMLNode xNode, vNodes)
            {
                tNode.m_vNodes.append(xNode);
            }
        }
        else
        {
            tNode.m_vAttributes[sKey] = jObject[sKey].toString();
        }
    }

    return tNode;
}

//-------------------------------------------------------------------------------------------------

QVector<CXMLNode> CXMLNode::parseJSONArray(QJsonArray jArray, QString sTagName)
{
    QVector<CXMLNode> vNodes;

    for (int iIndex = 0; iIndex < jArray.count(); iIndex++)
    {
        vNodes.append(CXMLNode::parseJSONNode(jArray[iIndex].toObject(), sTagName));
    }

    return vNodes;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CXMLNode::parseJSON(QString sText)
{
    CXMLNode tNode;

    if (sText.isEmpty() == false)
    {
        QJsonDocument doc = QJsonDocument::fromJson(sText.toLatin1());

        tNode = CXMLNode::parseJSONNode(doc.object(), "");
    }

    return tNode;
}

//-------------------------------------------------------------------------------------------------

QString CXMLNode::toString() const
{
    return toQDomDocument().toString();
}

//-------------------------------------------------------------------------------------------------

QDomDocument CXMLNode::toQDomDocument() const
{
    QDomDocument doc;

    QDomNode xmlNode = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(xmlNode);
    doc.appendChild(toQDomElement(doc));

    return doc;
}

//-------------------------------------------------------------------------------------------------

QDomElement CXMLNode::toQDomElement(QDomDocument& xDocument) const
{
    QDomElement thisElement = xDocument.createElement(m_sTag);

    if (!thisElement.isNull())
    {
        foreach(QString sAttributeName, m_vAttributes.keys())
        {
            thisElement.setAttribute(sAttributeName, m_vAttributes[sAttributeName]);
        }

        if (m_sValue.isEmpty() == false)
        {
            QDomText textElement = xDocument.createTextNode(m_sValue);
            thisElement.appendChild(textElement);
        }

        foreach(CXMLNode xChild, m_vNodes)
        {
            thisElement.appendChild(xChild.toQDomElement(xDocument));
        }
    }

    return thisElement;
}

//-------------------------------------------------------------------------------------------------

QJsonDocument CXMLNode::toJsonDocument() const
{
    QJsonDocument doc;

    doc.setObject(toJsonObject());

    return doc;
}

//-------------------------------------------------------------------------------------------------

QJsonObject CXMLNode::toJsonObject() const
{
    QJsonObject object;

    foreach (QString sKey, m_vAttributes.keys())
    {
        object[sKey] = m_vAttributes[sKey];
    }

    QStringList sTagList;

    for (int iIndex = 0; iIndex < m_vNodes.count(); iIndex++)
    {
        if (sTagList.contains(m_vNodes[iIndex].tag()) == false)
        {
            sTagList << m_vNodes[iIndex].tag();
        }
    }

    foreach (QString sTag, sTagList)
    {
        QVector<CXMLNode> vNodes = getNodesByTagName(sTag);
        QJsonArray array;

        foreach (CXMLNode xNode, vNodes)
        {
            array << xNode.toJsonObject();
        }

        object[sTag] = array;
    }

    return object;
}

//-------------------------------------------------------------------------------------------------

/*!
    Saves this CXMLNode tree as xml in the file named \a sFileName.
*/
bool CXMLNode::saveXMLToFile(const QString& sFileName)
{
    QFile xmlFile(sFileName);

    if (xmlFile.open(QIODevice::WriteOnly))
    {
        xmlFile.write(toString().toLatin1());
        xmlFile.close();

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Saves this CXMLNode tree as json in the file named \a sFileName.
*/
bool CXMLNode::saveJSONToFile(const QString& sFileName)
{
    QFile xmlFile(sFileName);

    if (xmlFile.open(QIODevice::WriteOnly))
    {
        xmlFile.write(toJsonDocument().toJson());
        xmlFile.close();

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the child node who's tag is \a sTagName.
*/
CXMLNode CXMLNode::getNodeByTagName(QString sTagName)
{
    foreach (CXMLNode tNode, m_vNodes)
    {
        if (tNode.m_sTag == sTagName) return tNode;
    }

    return CXMLNode();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the child node who's tag is \a sTagName.
*/
CXMLNode CXMLNode::getNodeByTagName(QString sTagName) const
{
    foreach(CXMLNode tNode, m_vNodes)
    {
        if (tNode.m_sTag == sTagName) return tNode;
    }

    return CXMLNode();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a list of child nodes who's tag is \a sTagName.
*/
QVector<CXMLNode> CXMLNode::getNodesByTagName(QString sTagName) const
{
    QVector<CXMLNode> vNodes;

    foreach (CXMLNode tNode, m_vNodes)
    {
        if (tNode.m_sTag == sTagName)
        {
            vNodes.append(tNode);
        }
    }

    return vNodes;
}

//-------------------------------------------------------------------------------------------------

/*!
    Merges the child nodes of \a target in this node's children.
*/
void CXMLNode::merge(const CXMLNode& target)
{
    foreach(CXMLNode node, target.m_vNodes)
    {
        m_vNodes.append(node);
    }
}
