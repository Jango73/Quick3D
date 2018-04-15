
// Application
#include "CImageFilter_Matrix.h"

//-------------------------------------------------------------------------------------------------

CImageFilter_Matrix::CImageFilter_Matrix(const QMatrix3x3& mMatrix)
    : m_mMatrix(mMatrix)
{
}

//-------------------------------------------------------------------------------------------------

CImageFilter_Matrix::~CImageFilter_Matrix()
{
}

//-------------------------------------------------------------------------------------------------

void CImageFilter_Matrix::process(QImage& imgImage)
{
    QImage imgSource = imgImage.copy();

    for (int y = 0; y < imgImage.height(); y++)
    {
        for (int x = 0; x < imgImage.width(); x++)
        {
            double dR = 0.0;
            double dG = 0.0;
            double dB = 0.0;

            for (int subY = -1; subY < 2; subY++)
            {
                for (int subX = -1; subX < 2; subX++)
                {
                    int offsetX = x + subX;
                    int offsetY = y + subY;

                    if (offsetX >= 0 && offsetX < imgImage.width() && offsetY >= 0 && offsetY < imgImage.height())
                    {
                        QRgb iPixel = imgSource.pixel(offsetX, offsetY);

                        double dNR = (double) ((iPixel >>  0) & 0xFF);
                        double dNG = (double) ((iPixel >>  8) & 0xFF);
                        double dNB = (double) ((iPixel >> 16) & 0xFF);

                        dR += dNR * m_mMatrix(subX + 1, subY + 1);
                        dG += dNG * m_mMatrix(subX + 1, subY + 1);
                        dB += dNB * m_mMatrix(subX + 1, subY + 1);
                    }
                }
            }

            QRgb iNewPixel = qRgb((int) dR, (int) dG, (int) dB);

            imgImage.setPixel(x, y, iNewPixel);
        }
    }
}
