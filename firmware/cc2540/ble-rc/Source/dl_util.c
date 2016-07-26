#include "bcomdef.h"
#include "gatt.h"
#include "dl_util.h"

bStatus_t ExtractUuid16(gattAttribute_t* attribute, uint16* uuid)
{
	bStatus_t status = SUCCESS;

	if (attribute->type.len == ATT_BT_UUID_SIZE)
	{
		// 16-bit UUID direct
		*uuid = BUILD_UINT16(attribute->type.uuid[0], attribute->type.uuid[1]);
	}
	else if (attribute->type.len == ATT_UUID_SIZE)
	{
		 // 16-bit UUID extracted bytes 12 and 13
		*uuid = BUILD_UINT16(attribute->type.uuid[12], attribute->type.uuid[13]);
	}
	else
	{
		*uuid = 0xFFFF;
		status = FAILURE;
	}

	return status;
};
