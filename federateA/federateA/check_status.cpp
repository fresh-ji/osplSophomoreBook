
#include "check_status.h"

/* Array to hold the names for all ReturnCodes. */
std::string RetCodeName[13] =
{
	"DDS_RETCODE_OK", "DDS_RETCODE_ERROR", "DDS_RETCODE_UNSUPPORTED",
	"DDS_RETCODE_BAD_PARAMETER", "DDS_RETCODE_PRECONDITION_NOT_MET",
	"DDS_RETCODE_OUT_OF_RESOURCES", "DDS_RETCODE_NOT_ENABLED",
	"DDS_RETCODE_IMMUTABLE_POLICY", "DDS_RETCODE_INCONSISTENT_POLICY",
	"DDS_RETCODE_ALREADY_DELETED", "DDS_RETCODE_TIMEOUT", "DDS_RETCODE_NO_DATA",
	"DDS_RETCODE_ILLEGAL_OPERATION"
};

/**
 * Returns the name of an error code.
 **/
std::string getErrorName(DDS::ReturnCode_t status)
{
	return RetCodeName[status];
}

/**
 * Check the return status for errors. If there is an error, then terminate.
 **/
bool CheckStatus(DDS::ReturnCode_t status, const char *info)
{
	if (status != DDS::RETCODE_OK && status != DDS::RETCODE_NO_DATA)
	{
		std::cerr << "Error in " << info << ": " << getErrorName(status).c_str() << std::endl;
		return false;
	}
	return true;
}

/**
 * Check whether a valid handle has been returned. If not, then terminate.
 **/
bool CheckHandle(void *handle, std::string info)
{
	if (!handle)
	{
		std::cerr << "Error in " << info.c_str() << ": Creation failed: invalid handle" << std::endl;
		return false;
	}
	return true;
}
