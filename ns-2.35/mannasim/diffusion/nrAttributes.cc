/* nrAttributes.cc
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * This file contains implementation for Directed Diffusion
 * attributes for using by MannaSIM.
 */
#define _NRATTRIBUTES_CC_

#include "nrAttributes.h"
#include <stdlib.h>

NRSimpleAttribute<AppData *>::NRSimpleAttribute(int key, int type, int op, AppData * val, int size)
	: NRAttribute(key, BLOB_TYPE, op, 0)
{
	assert(type == BLOB_TYPE);

	val_ = NULL;
	setVal(val, val->size());
}

NRSimpleAttribute<AppData *>::~NRSimpleAttribute()
{
	delete [] (char *) val_;
}

void NRSimpleAttribute<AppData *>::setVal(AppData * value, int len)
{
	delete [] (char *) val_;
	len_ = len;
	val_ = (void *) new char[len_];
	memcpy(val_, value, len_);
}

AppData * NRSimpleAttribute<AppData *>::getVal()
{
	return (AppData *) val_;
}

/***************************************************************************/
/* Atributes */

NRSimpleAttributeFactory<AppDataType> AppDataTypeAttr(
	APPLICATION_DATA_TYPE, NRAttribute::INT32_TYPE);

NRSimpleAttributeFactory<int> NodeIDAttr(
	NODE_ID, NRAttribute::INT32_TYPE);

NRSimpleAttributeFactory<AppData *>   SensedDataAttr(
	SENSED_DATA_BLOB, NRAttribute::BLOB_TYPE);

NRSimpleAttributeFactory<double> SensedValueAttr(SENSED_VALUE, NRAttribute::FLOAT64_TYPE);

NRSimpleAttributeFactory<float> TimeStampAttr(TIME_STAMP, NRAttribute::FLOAT32_TYPE);

#undef _NRATTRIBUTES_CC_
