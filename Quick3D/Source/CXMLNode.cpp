
// Qt
#include <QFile>

// Application
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------

CXMLNode::CXMLNode()
{
}

//-------------------------------------------------------------------------------------------------

CXMLNode::~CXMLNode()
{
}

//-------------------------------------------------------------------------------------------------

CXMLNode CXMLNode::parseXMLNode(QDomNode node)
{
	CXMLNode tNode;

	tNode.m_sTag = node.nodeName();
	tNode.m_sValue	= node.nodeValue();

	for (int Index = 0; Index < node.attributes().length(); Index++)
	{
		QDomNode attrNode = node.attributes().item(Index);
		tNode.m_vAttributes[attrNode.nodeName()] = attrNode.nodeValue();
	}

	for (int Index = 0; Index < node.childNodes().length(); Index++)
	{
		tNode.m_vNodes.append(CXMLNode::parseXMLNode(node.childNodes().at(Index)));
	}

	return tNode;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CXMLNode::parseXML(QString sText)
{
	CXMLNode tNode;

	if (sText.isEmpty() == false)
	{
		QDomDocument doc("myDocument");
		doc.setContent(sText);

		tNode = CXMLNode::parseXMLNode(doc.documentElement());
	}

	return tNode;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CXMLNode::loadFromFile(QString sFileName)
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

		foreach(CXMLNode xChild, m_vNodes)
		{
			thisElement.appendChild(xChild.toQDomElement(xDocument));
		}
	}

	return thisElement;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CXMLNode::getNodeByTagName(QString sTagName)
{
	foreach (CXMLNode tNode, m_vNodes)
	{
		if (tNode.m_sTag == sTagName) return tNode;
	}

	return CXMLNode();
}

//-------------------------------------------------------------------------------------------------

QVector<CXMLNode> CXMLNode::getNodesByTagName(QString sTagName)
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
