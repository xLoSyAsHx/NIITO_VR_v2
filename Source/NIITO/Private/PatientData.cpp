#include "PatientData.h"

bool UPatientDataLib::EqualEqual_FPatientDataFPatientData(const FPatientData& A, const FPatientData& B)
{
    return A.FirstName == B.FirstName &&
        A.SurName      == B.SurName   &&
        A.LastName     == B.LastName  &&
        A.Birthday     == B.Birthday  &&
        A.Diagnose     == B.Diagnose
        ||
        A.ID == B.ID;
}