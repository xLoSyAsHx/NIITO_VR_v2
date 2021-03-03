#include "PatientData.h"

bool UPatientDataLib::EqualEqual_FPatientDataFPatientData(const FPatientData& A, const FPatientData& B)
{
    return A.FirstName == B.FirstName &&
        A.SecondName == B.SecondName &&
        A.Diagnose == B.Diagnose;
}