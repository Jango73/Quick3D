
// Qt
#include <QPainter>

// Application
#include "CTimeSampler.h"
#include "CImageUtilities.h"

//-------------------------------------------------------------------------------------------------

CSphericalTextureSet::CSphericalTextureSet()
: m_iPanCount(0)
, m_iTiltCount(0)
{
}

//-------------------------------------------------------------------------------------------------

CSphericalTextureSet::CSphericalTextureSet(
	Math::CVector2 vStartAngles,
	Math::CVector2 vEndAngles,
	int iPanCount,
	int iTiltCount,
	QSize sTextureSize
	)
: m_vStartAngles(vStartAngles)
, m_vEndAngles(vEndAngles)
, m_iPanCount(iPanCount)
, m_iTiltCount(iTiltCount)
, m_sTextureSize(sTextureSize)
{
}

//-------------------------------------------------------------------------------------------------
/*
createPanoramicTextureSet()

Paramètres d'entrée :
La taille des textures de sortie (tParams.m_sTextureSize) doit être une puissance de deux.

Travail de la méthode :
- Créer un jeu de textures de taille prédéfinie compatible OpenGL qui recouvrent une sphère
- Transférer l'image panoramique d'entrée dans le jeu de textures OpenGL en utilisant les paramètres
  qui définissent son étendue en pan et tilt (degrés)

Ex: Textures de sortie couvrant 180° en tilt et 360° en pan :

  1024    1024    1024    1024
--------------------------------
|      ||      ||      ||      |
|      ||      ||      ||      | 1024
|      ||      ||      ||      |
--------------------------------
|      ||      ||      ||      |
|      ||      ||      ||      | 1024
|      ||      ||      ||      |
--------------------------------

*/

void CImageUtilities::createPanoramicTextureSet(
	const QImage& imgInput,
	CSphericalTextureSet& tParams
	)
{
	// Récupération du nombre de textures en sortie
	int iNumTiltImages = tParams.getTiltCount();
	int iNumPanImages = tParams.getPanCount();

	// Récupération de la taille des textures de sortie
	int iTextureWidth = tParams.getTextureSize().width();
	int iTextureHeight = tParams.getTextureSize().height();

	// Création de la texture de sortie intermédiaire
	QImage imgOutput(
		QSize(iTextureWidth * iNumPanImages, iTextureHeight * iNumTiltImages),
		QImage::Format_RGB888
		);

	Math::CVector2 vInputStartAngles = tParams.getStartAngles();
	Math::CVector2 vInputEndAngles = tParams.getEndAngles();
	Math::CVector2 vInputCenter((vInputStartAngles.X + vInputEndAngles.X) / 2.0, (vInputStartAngles.Y + vInputEndAngles.Y) / 2.0);
	Math::CVector2 vInputSpan(vInputEndAngles.X - vInputStartAngles.X, vInputEndAngles.Y - vInputStartAngles.Y);

	// Resampling sphérique de l'image d'entrée vers l'image de sortie
	for (int iOutputTiltIndex = 0; iOutputTiltIndex < imgOutput.height(); iOutputTiltIndex++)
	{
		for (int iOutputPanIndex = 0; iOutputPanIndex < imgOutput.width(); iOutputPanIndex++)
		{
			// Calcul du pan et tilt dans le repère de sortie
			double dTiltNormalized	= (double) iOutputTiltIndex / (double) imgOutput.height();
			double dPanNormalized	= (double) iOutputPanIndex / (double) imgOutput.width();

			double dOutputTilt = (dTiltNormalized * 180.0) - 90.0;
			double dOutputPan = (dPanNormalized * 360.0) - 180.0;

			double dOuputTiltFactor = imgInput.height() / vInputSpan.X;
			double dOuputPanFactor = imgInput.width() / vInputSpan.Y;

			double dInputTilt = (dOutputTilt - vInputStartAngles.X) * dOuputTiltFactor;
			double dInputPan = (dOutputPan - vInputStartAngles.Y) * dOuputPanFactor;

			int iInputTiltIndex = (int) dInputTilt;
			int iInputPanIndex = (int) dInputPan;

			// Initialisation du pixel courant à zéro
			QRgb rgbPixel = 0;

			// Récupération du pixel s'il est dans les limites de l'image d'entrée
			if (
				iInputPanIndex > 0 && iInputPanIndex < imgInput.width() &&
				iInputTiltIndex > 0 && iInputTiltIndex < imgInput.height()
				)
			{
				rgbPixel = imgInput.pixel(iInputPanIndex, iInputTiltIndex);
			}

			// Ecriture du pixel dans l'image de sortie
			imgOutput.setPixel(iOutputPanIndex, iOutputTiltIndex, rgbPixel);
		}
	}

	// Création des textures de sortie d'après la texture de sortie intermédiaire (imgOutput)
	tParams.getImages().clear();

	// Parcours de chaque ligne
	for (int iTiltIndex = 0; iTiltIndex < iNumTiltImages; iTiltIndex++)
	{
		// Ajout d'un vecteur d'images pour la ligne courante
		tParams.getImages().append(QVector<QImage>());

		// Parcours de chaque colonne
		for (int iPanIndex = 0; iPanIndex < iNumPanImages; iPanIndex++)
		{
			QImage imgImage(QSize(iTextureWidth, iTextureHeight), QImage::Format_RGB888);

			blitPart(
				imgImage, QPoint(0, 0),
				imgOutput, QPoint(iPanIndex * iTextureWidth, iTiltIndex * iTextureHeight),
				tParams.getTextureSize()
				);

			tParams.getImages()[iTiltIndex].append(imgImage);
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CImageUtilities::blitPart(QImage& imgDest, QPoint pDestPosition, const QImage& imgSource, QPoint pSourcePosition, QSize sSize)
{
	START_SAMPLE("CImageUtilities::blitPart");

	for (int y = 0; y < sSize.height(); y++)
	{
		for (int x = 0; x < sSize.width(); x++)
		{
			imgDest.setPixel(
				pDestPosition.x() + x,
				pDestPosition.y() + y,
				imgSource.pixel(pSourcePosition.x() + x, pSourcePosition.y() + y)
				);
		}
	}

	STOP_SAMPLE("CImageUtilities::blitPart");
}

//-------------------------------------------------------------------------------------------------

void CImageUtilities::blitWhole(QImage& imgDest, QPoint pDestPosition, const QImage& imgSource)
{
	START_SAMPLE("CImageUtilities::blitWhole");

	QPainter painter(&imgDest);
	painter.drawImage(pDestPosition, imgSource);
	painter.end();

	STOP_SAMPLE("CImageUtilities::blitWhole");
}
