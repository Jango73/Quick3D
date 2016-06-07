
#include "CAxis.h"

#include "C3DScene.h"
#include "CRessourcesManager.h"
#include "COBJLoader.h"
#include "CVehicle.h"
#include "CMesh.h"

// #define SHOW_AXES

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CVehicle::instanciator(C3DScene* pScene)
{
	return new CVehicle(pScene);
}

//-------------------------------------------------------------------------------------------------

CVehicle::CVehicle(C3DScene* pScene)
: CTrajectorable(pScene)
{
	LOG_DEBUG("CVehicle::CVehicle()");
}

//-------------------------------------------------------------------------------------------------

CVehicle::~CVehicle()
{
	LOG_DEBUG("CVehicle::~CVehicle()");
}

//-------------------------------------------------------------------------------------------------

void CVehicle::loadParameters(CXMLNode xComponent)
{
	CTrajectorable::loadParameters(xComponent);

	// Read physics

	CXMLNode tPhysicsNode = xComponent.getNodeByTagName(ParamName_Physics);

	setSpeedMS(tPhysicsNode.m_vAttributes["SpeedMS"].toDouble());
	setTurnSpeedDS(tPhysicsNode.m_vAttributes["TurnSpeedDS"].toDouble());

	// Read trajectory

	CXMLNode tTrajectoryNode = xComponent.getNodeByTagName("Trajectory");
	QVector<CXMLNode> vPointNodes = tTrajectoryNode.getNodesByTagName(ParamName_Geoloc);

	foreach (CXMLNode tPointNode, vPointNodes)
	{
		m_tTrajectory.addPoint(CGeoloc(
			tPointNode.m_vAttributes[ParamName_Latitude].toDouble(),
			tPointNode.m_vAttributes[ParamName_Longitude].toDouble(),
			tPointNode.m_vAttributes[ParamName_Altitude].toDouble()
		));
	}

	CXMLNode tGeolocNode = xComponent.getNodeByTagName(ParamName_Geoloc);
	CXMLNode tRotationNode = xComponent.getNodeByTagName("Rotation");

	CGeoloc gPosition = CGeoloc(
		tGeolocNode.m_vAttributes[ParamName_Latitude].toDouble(),
		tGeolocNode.m_vAttributes[ParamName_Longitude].toDouble(),
		tGeolocNode.m_vAttributes[ParamName_Altitude].toDouble()
	);

	CVector3 vRotation = CVector3(
		Math::Angles::toRad(tRotationNode.m_vAttributes[ParamName_x].toDouble()),
		Math::Angles::toRad(tRotationNode.m_vAttributes[ParamName_y].toDouble()),
		Math::Angles::toRad(tRotationNode.m_vAttributes[ParamName_z].toDouble())
	);

	m_tTrajectory.setOriginPosition(gPosition);
	m_tTrajectory.setOriginRotation(vRotation);
}

//-------------------------------------------------------------------------------------------------

void CVehicle::update(double dDeltaTime)
{
	CTrajectorable::update(dDeltaTime);

#ifdef SHOW_AXES
	if (m_vAxisMeshes.count() == 0)
	{
		CMesh* pMesh = new CMesh(m_pScene);

		COBJLoader::getInstance()->load(
			m_pScene,
			pMesh,
			CRessourcesManager::getInstance()->getObjByFilePathName(":/Resources/Axis.obj")
			);

		pMesh->updateGeometry();

		for (int iIndex = 0; iIndex < 4; iIndex++)
		{
			m_vAxisMeshes.append(CRessourcesManager::getInstance()->share(pMesh));
		}
	}
#endif

	if (isRootObject())
	{
		if (m_pFields.count() > 0.0)
		{
			CVector3 vRotation;

			if (m_bPhysicsActive == true)
			{
				vRotation = getOriginRotation();
			}
			else
			{
				vRotation = CVector3(0.0, getOriginRotation().Y, 0.0);
			}

			CBoundingBox box = getBounds();

			CMatrix4 mOrientation = CMatrix4().MakeRotation(vRotation);

			CGeoloc gBackLeft = CGeoloc(getGeoloc(), mOrientation * CVector3(box.minimum().X * 0.8, 0.0, box.minimum().Z * 0.8));
			CGeoloc gBackRight = CGeoloc(getGeoloc(), mOrientation * CVector3(box.maximum().X * 0.8, 0.0, box.minimum().Z * 0.8));
			CGeoloc gFrontLeft = CGeoloc(getGeoloc(), mOrientation * CVector3(box.minimum().X * 0.8, 0.0, box.maximum().Z * 0.8));
			CGeoloc gFrontRight = CGeoloc(getGeoloc(), mOrientation * CVector3(box.maximum().X * 0.8, 0.0, box.maximum().Z * 0.8));

			double r1 = 0.0;
			double r2 = 0.0;
			double r3 = 0.0;
			double r4 = 0.0;

			double hBackLeft = m_pFields[0]->getHeightAt(gBackLeft, &r1);
			double hBackRight = m_pFields[0]->getHeightAt(gBackRight, &r2);
			double hFrontLeft = m_pFields[0]->getHeightAt(gFrontLeft, &r3);
			double hFrontRight = m_pFields[0]->getHeightAt(gFrontRight, &r4);

			double dRigidness = (r1 + r2 + r3 + r4) / 4.0;

			double hAverage = (hBackLeft + hBackRight + hFrontLeft + hFrontRight) / 4.0;

			if (hBackLeft != Q3D_INFINITY && hBackRight != Q3D_INFINITY && hFrontLeft != Q3D_INFINITY && hFrontRight != Q3D_INFINITY)
			{
				if (m_bPhysicsActive == true)
				{
					if (fabs(getGeoloc().Altitude - hAverage) < 5.0)
					{
						double hdBackLeft = gBackLeft.Altitude - hBackLeft;
						double hdBackRight = gBackRight.Altitude - hBackRight;
						double hdFrontLeft = gFrontLeft.Altitude - hFrontLeft;
						double hdFrontRight = gFrontRight.Altitude - hFrontRight;

						// X
						CVector3 Front = CVector3(0.0, (hdFrontLeft + hdFrontRight) / 2.0, box.maximum().Z * 0.8);
						CVector3 Back = CVector3(0.0, (hdBackLeft + hdBackRight) / 2.0, box.minimum().Z * 0.8);

						double dDiffX = (Front - Back).AngleX();

						// Z
						CVector3 Right = CVector3(box.maximum().X * 0.8, (hdBackRight + hdFrontRight) / 2.0, 0.0);
						CVector3 Left = CVector3(box.minimum().X * 0.8, (hdBackLeft + hdFrontLeft) / 2.0, 0.0);

						double dDiffZ = (Right - Left).AngleZ();

                        CVector3 torque(-dDiffX * 4.0, 0.0, -dDiffZ);

						addLocalTorque_kg(torque * getTotalMass_kg());
					}
				}
				else
				{
					if (fabs(getGeoloc().Altitude - hAverage) < 2.0)
					{
						// X
						CVector3 Front = CVector3(0.0, (hFrontLeft + hFrontRight) / 2.0, box.maximum().Z * 0.8);
						CVector3 Back = CVector3(0.0, (hBackLeft + hBackRight) / 2.0, box.minimum().Z * 0.8);

						double dDiffX = (Front - Back).AngleX();

						// Z
						CVector3 Right = CVector3(box.maximum().X * 0.8, (hBackRight + hFrontRight) / 2.0, 0.0);
						CVector3 Left = CVector3(box.minimum().X * 0.8, (hBackLeft + hFrontLeft) / 2.0, 0.0);

						double dDiffZ = (Right - Left).AngleZ();

						setOriginRotation(CVector3(
							dDiffX * dRigidness,
							getOriginRotation().Y,
							dDiffZ * dRigidness
							));
					}

#ifdef SHOW_AXES
					m_vAxisMeshes[0]->setGeoloc(Geoloc(gBackLeft.Latitude, gBackLeft.Longitude, hBackLeft));
					m_vAxisMeshes[1]->setGeoloc(Geoloc(gBackRight.Latitude, gBackRight.Longitude, hBackRight));
					m_vAxisMeshes[2]->setGeoloc(Geoloc(gFrontLeft.Latitude, gFrontLeft.Longitude, hFrontLeft));
					m_vAxisMeshes[3]->setGeoloc(Geoloc(gFrontRight.Latitude, gFrontRight.Longitude, hFrontRight));
#endif
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CVehicle::paint(CRenderContext* pContext)
{
	CTrajectorable::paint(pContext);

#ifdef SHOW_AXES
	foreach (CMeshInstance* pMesh, m_vAxisMeshes)
	{
		pMesh->paint(pContext);
	}
#endif
}
