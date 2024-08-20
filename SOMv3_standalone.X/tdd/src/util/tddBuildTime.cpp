#include "tddBuildTime.h"

tddBuildTime::tddBuildTime()
: dateTime(__DATE__ " " __TIME__)
{
}

tddBuildTime::~tddBuildTime()
{
}

const char* tddBuildTime::GetDateTime()
{
    return dateTime;
}

