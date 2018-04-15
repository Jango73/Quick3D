
#include "C3DScene.h"
#include "CAnimator.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    \class CAnimator
    \brief The base class for an animator.
    \inmodule Quick3D
    \sa CComponent
*/

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CAnimator. \br\br
    \a pScene is the scene containing the component.
*/
CComponent* CAnimator::instantiator(C3DScene* pScene)
{
    return new CAnimator(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CAnimator with its default parameters. \br\br
    \a pScene is the scene containing the component.
*/
CAnimator::CAnimator(C3DScene* pScene)
    : CController(pScene)
    , m_bIsPlaying(false)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CAnimator.
*/
CAnimator::~CAnimator()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Starts the animator.
*/
void CAnimator::play()
{
    m_bIsPlaying = true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Stops the animator.
*/
void CAnimator::stop()
{
    m_bIsPlaying = false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this component from \a xComponent. \br\br
    \a sBaseFile is the file name from which it is loaded.
*/
void CAnimator::loadParameters(const QString& sBaseFile, const CXMLNode& xNode)
{
    CController::loadParameters(sBaseFile, xNode);

    if (xNode.attributes()[ParamName_Playing].isEmpty() == false)
    {
        m_bIsPlaying = (bool) xNode.attributes()[ParamName_Playing].toInt();
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates this component using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CAnimator::update(double dDeltaTimeS)
{
    CController::update(dDeltaTimeS);
}
