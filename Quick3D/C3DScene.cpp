
#ifndef WIN32
#include <sys/param.h>
#endif

// Qt
#include <QPainter>
#include <QPaintEngine>
#include <QtOpenGL>
#include <GL/glu.h>

// Fondations
#include "CLogManager.h"

// Application
#include "C3DScene.h"
#include "CView.h"
#include "CRessourcesManager.h"
#include "COBJLoader.h"
#include "CMaterial.h"
#include "CHeightField.h"
#include "CAutoTerrain.h"
#include "CTrajectorable.h"
#include "CController.h"
#include "CStandardController.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class C3DScene
    \brief The base class for a scene that holds 3D objects.
    \inmodule Quick3D
    \sa CComponent
*/

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a C3DScene with its default parameters.
*/
C3DScene::C3DScene(bool bForDisplay)
	: m_pRessourcesManager(NULL)
	, m_pBuildingGenerator(NULL)
	, m_pTreeGenerator(NULL)
	, m_vShaders(NULL)
	, m_bForDisplay(bForDisplay)
	, m_pController(NULL)
	, m_pDefaultController(NULL)
	, m_bEditMode(false)
	, m_DebugMode(false)
	, m_bBoundsOnly(false)
	, m_dShaderQuality(0.5)
	, m_pRain(NULL)
	, m_tTimeOfDay(12, 0, 0)
	, m_dTime(0.0)
	, m_dWindLevel(0.5)
	, m_bRenderingShadows(false)
	, m_bforceWideFOV(false)
	, m_bforceSmallFOV(false)
	, m_bForceIR(false)
	, m_bStreamView(false)
	, m_bDepthComputing(false)
	, m_iNumMeshesDrawn(0)
	, m_iNumPolysDrawn(0)
	, m_iNumChunksDrawn(0)
{
	LOG_DEBUG("C3DScene::C3DScene()");
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a C3DScene.
*/
C3DScene::~C3DScene()
{
	LOG_DEBUG("C3DScene::~C3DScene()");

	clearComponents();
	clearLights();
	clearViewports();

	if (m_pGLExtension != NULL) delete m_pGLExtension;
	if (m_vShaders != NULL) delete m_vShaders;
	if (m_pTreeGenerator != NULL) delete m_pTreeGenerator;
	if (m_pBuildingGenerator != NULL) delete m_pBuildingGenerator;
	if (m_pRessourcesManager != NULL) delete m_pRessourcesManager;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all components, lights and viewports in the scene.
*/
void C3DScene::clear()
{
	clearComponents();
	clearLights();
	clearViewports();
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all components in the scene.
*/
void C3DScene::clearComponents()
{
	// Destruction des composants

	m_vComponents.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all lights in the scene.
*/
void C3DScene::clearLights()
{
	// Destruction des lumières

	m_vLights.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all viewports in the scene.
*/
void C3DScene::clearViewports()
{
	// Destruction des viewports

	foreach (int iIndex, m_pViewports.keys())
	{
		delete m_pViewports[iIndex];
	}

	m_pViewports.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Initializes the scene using components in \a vComponents.
*/
void C3DScene::init(QVector<CComponent*> vComponents)
{
	LOG_DEBUG("C3DScene::init()");

	//-----------------------------------------------
	// Initialisation brume et couleurs du soleil

	m_tFog.enabled() = true;
	m_tFog.level() = 0.0;
	m_tFog.color() = CVector3(0.5, 0.5, 0.5);

	m_vSunColor.clear();
	m_vSunColor.addValue( 0.00, Vector4(0.0, 0.0, 0.0, 1.0));
	m_vSunColor.addValue( 0.50, Vector4(1.1, 0.7, 0.2, 1.0));
	m_vSunColor.addValue( 0.60, Vector4(1.1, 0.7, 0.2, 1.0));
	m_vSunColor.addValue( 0.75, Vector4(1.4, 1.4, 1.2, 1.0));
	m_vSunColor.addValue( 1.00, Vector4(1.4, 1.4, 1.2, 1.0));

	//-----------------------------------------------
	// Ajout soleil

	if (m_vLights.count() == 0 || m_vLights[0]->getName() != "Sun")
	{
		m_vLights.insert(0, QSharedPointer<CLight>(new CLight(this)));

		m_vLights[0]->setName("Sun");
		m_vLights[0]->setCastShadows(true);
		m_vLights[0]->setFOV(10.0);
	}

	//-----------------------------------------------
	// Création des éléments de décor

	// m_pRain = new CRain(this);

	//-----------------------------------------------
	// Chargement des composants

	foreach(CComponent* pComponent, vComponents)
	{
		m_vComponents.append(QSharedPointer<CComponent>(pComponent));
	}

	foreach(QSharedPointer<CComponent> pComponent, m_vComponents)
	{
		pComponent->addItems(this);
	}

	if (m_pController != NULL)
	{
		m_pController->solveLinks(this);
	}

	foreach (QSharedPointer<CComponent> pComponent, m_vComponents)
	{
		pComponent->solveLinks(this);
	}

	autoResolveHeightFields();
}

//-------------------------------------------------------------------------------------------------

/*!
    Initializes shaders.
*/
void C3DScene::initShaders()
{
	LOG_DEBUG("C3DScene::initShaders()");
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the world origin to \a value. \br\br
    This property is used as a center point when rendering, in order to avoid huge geocentric position values in matrices and shaders, which lead to visual artifacts.
*/
void C3DScene::setWorldOrigin(Math::CVector3 value)
{
    m_WorldOrigin = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the active controller of the scene to \a pController.
*/
void C3DScene::setController(CController* pController)
{
	if (pController != NULL)
	{
		m_pController = pController;
	}
	else
	{
		m_pController = m_pDefaultController;
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the default controller of the scene to \a pController.
*/
void C3DScene::setDefaultController(CController* pController)
{
	m_pDefaultController = pController;
	m_pController = pController;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the shader quality of the scene to \a value. \br\br
    This value is used in shaders in order to easily adjust the shader level of complexity.
*/
void C3DScene::setShaderQuality(double value)
{
    m_dShaderQuality = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the time of the scene to \a value, expressed in seconds. \br\br
    The time property is mainly used by animated shaders.
*/
void C3DScene::setTime(double value)
{
	m_dTime = value;

	CMaterial::setTime(value);
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the time of the scene to \a value. \br\br
    The time of day property is mainly used for sunlight computation.
*/
void C3DScene::setTimeOfDay(QTime value)
{
    m_tTimeOfDay = value;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setFogLevel(double value)
{
	m_tFog.level() = value;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setWindLevel(double value)
{
	m_dWindLevel = value;
	if (m_dWindLevel < 0.05) m_dWindLevel = 0.05;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setRainVisible(bool bOnOff)
{
	if (m_pRain) m_pRain->setVisible(bOnOff);
}

//-------------------------------------------------------------------------------------------------

QVector<QSharedPointer<CComponent> > C3DScene::getComponentsByTag(const QString& sTag)
{
	QVector<QSharedPointer<CComponent> > vReturnValue;

	foreach(QSharedPointer<CComponent> pComponent, m_vComponents)
	{
		if (pComponent->getTag() == sTag)
		{
			vReturnValue.append(pComponent);
		}
	}

	return vReturnValue;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::forceWideFOV(bool value)
{
	m_bforceWideFOV = value;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::forceSmallFOV(bool value)
{
	m_bforceSmallFOV = value;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::forceIR(bool value)
{
	m_bForceIR = value;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::updateScene(double dDeltaTimeS)
{
	if (m_pController != NULL)
	{
		m_pController->update(dDeltaTimeS);
	}

	if (m_bEditMode)
	{
		dDeltaTimeS = 0.0;
	}

	foreach (QSharedPointer<CComponent> pComponent, m_vComponents)
	{
		pComponent->update(dDeltaTimeS);
	}

	foreach (QSharedPointer<CComponent> pComponent, m_vComponents)
	{
		pComponent->postUpdate(dDeltaTimeS);
	}

	foreach (QSharedPointer<CLight> pLight, m_vLights)
	{
		pLight->update(dDeltaTimeS);
	}

	foreach (QSharedPointer<CLight> pLight, m_vLights)
	{
		pLight->postUpdate(dDeltaTimeS);
	}

	CPhysicalComponent::computeCollisions(m_vComponents, dDeltaTimeS);
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setupEnvironment(CRenderContext* pContext, QGLShaderProgram* pProgram, bool bBackgroundItem)
{
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setupLights(CRenderContext* pContext)
{
}

//-------------------------------------------------------------------------------------------------

void C3DScene::makeCurrentRenderingContext()
{
}

//-------------------------------------------------------------------------------------------------

void C3DScene::addComponent(QSharedPointer<CComponent> pComponent)
{
	m_vComponents.append(pComponent);
	pComponent->solveLinks(this);
	autoResolveHeightFields();
}

//-------------------------------------------------------------------------------------------------

void C3DScene::autoResolveHeightFields()
{
	CHeightField* pTerrain = NULL;

	foreach (QSharedPointer<CComponent> pComponent, m_vComponents)
	{
		if (dynamic_cast<CAutoTerrain*>(pComponent.data()) != NULL)
		{
			pTerrain = dynamic_cast<CHeightField*>(pComponent.data());
		}
		else
		{
			if (dynamic_cast<CTrajectorable*>(pComponent.data()) != NULL)
			{
				CPhysicalComponent* pPhysical = dynamic_cast<CPhysicalComponent*>(pComponent.data());

				if (pPhysical != NULL && pTerrain != NULL)
				{
					pPhysical->addField(pTerrain);
				}
			}
		}

		pComponent->update(0.0);
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Deletes all components in the scene that have a tag equal to \a sTag.
*/
void C3DScene::deleteComponentsByTag(const QString& sTag)
{
	for (int iIndex = 0; iIndex < m_vComponents.count(); iIndex++)
	{
		if (m_vComponents[iIndex]->getTag() == sTag)
		{
			m_vComponents.remove(iIndex);
			iIndex--;
		}
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects components in the scene.
*/
RayTracingResult C3DScene::intersect(Math::CRay3 aRay) const
{
	RayTracingResult dReturnResult(Q3D_INFINITY);

	foreach (QSharedPointer<CComponent> pComponent, m_vComponents)
	{
		RayTracingResult dNewResult = intersectRecurse(pComponent.data(), aRay);

		if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
		{
			dReturnResult = dNewResult;
		}
	}

	return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects the component specified by \a pComponent, or any of its children.
*/
RayTracingResult C3DScene::intersectComponentHierarchy(CComponent* pComponent, Math::CRay3 aRay) const
{
	RayTracingResult aResult = intersectRecurse(pComponent, aRay);
	return aResult;
}

//-------------------------------------------------------------------------------------------------

RayTracingResult C3DScene::intersectRecurse(CComponent* pComponent, const Math::CRay3& aRay) const
{
	RayTracingResult dReturnResult = pComponent->intersect(aRay);

	foreach (CComponent* pChild, pComponent->getChildren())
	{
		RayTracingResult dChildResult = intersectRecurse(pChild, aRay);

		if (dChildResult.m_dDistance < dReturnResult.m_dDistance)
		{
			dReturnResult = dChildResult;
		}
	}

	return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this scene to \a stream using the indentation value in \a iIdent.
*/
void C3DScene::dump(QTextStream& stream, int iIdent)
{
	dumpIdent(stream, iIdent, QString("[C3DScene]"));
	dumpIdent(stream, iIdent, QString("CComponent::getNumComponents() : %1").arg(CComponent::getNumComponents()));

	foreach (QSharedPointer<CComponent> pComponent, m_vComponents)
	{
		dumpOpenBlock(stream, iIdent);
		pComponent->dump(stream, iIdent + 1);
		dumpCloseBlock(stream, iIdent);
	}
}
