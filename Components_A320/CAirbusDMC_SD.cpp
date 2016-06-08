
// Qt
#include <QPainter>

// Quick3D
#include "CLogManager.h"
#include "C3DScene.h"

// Application
#include "CAirbusDMC.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

void CAirbusDMC::updateTexture_SD(QPainter* pPainter, CTexture* pTexture, double dDeltaTime)
{
    double X = 0.0 * pTexture->getImage().width();
    double Y = 0.0 * pTexture->getImage().height();
    double W = 1.0 * pTexture->getImage().width();
    double H = 1.0 * pTexture->getImage().height();

    QRectF rWholePart(X, Y, W, H);

    pPainter->resetTransform();

    // Set main font
    pPainter->setPen(A320_Color_White);
    pPainter->setFont(m_fMainFont);

    pPainter->drawText(rWholePart, Qt::AlignCenter, "SD");
}
