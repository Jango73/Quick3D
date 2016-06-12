
#pragma once

#include <QString>

class IProgressListener
{
public:

    //! Notification de la progression d'une op�ration
    virtual void notifyProgress(QString sWhat, double dPercent) = 0;
};
