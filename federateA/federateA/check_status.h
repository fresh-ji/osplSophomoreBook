
#ifndef __CHECKSTATUS_H__
#define __CHECKSTATUS_H__

#include "ccpp_dds_dcps.h"
#include <iostream>

/**
 * Returns the name of an error code.
 **/
std::string getErrorName(DDS::ReturnCode_t status);

/**
 * Check the return status for errors. If there is an error, then terminate.
 **/
bool CheckStatus(DDS::ReturnCode_t status, const char *info);

/**
 * Check whether a valid handle has been returned. If not, then terminate.
 **/
bool CheckHandle(void *handle, std::string info);

#endif
