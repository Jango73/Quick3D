
#pragma once

// Qt
#include <QString>

//-------------------------------------------------------------------------------------------------

class ISnapshotable
{
public:

    //! Returns a JSON document with the object's state for the current frame
    virtual QString snapshotJson() = 0;
};
