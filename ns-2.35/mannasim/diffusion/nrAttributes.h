/* nrAttributes.h
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * This file contains definition for Directed Diffusion
 * attributes for using by MannaSIM.
 */
#ifndef _NRATTRIBUTES_H_
#define _NRATTRIBUTES_H_

#include "diffapp.hh"
#include "config.h"

template <> class NRSimpleAttribute<AppData *> : public NRAttribute
{
	public:
		NRSimpleAttribute(int key, int type, int op, AppData * val, int size = 0);
		~NRSimpleAttribute();

		void setVal(AppData * value, int len);
		AppData * getVal();
};

enum keys
{
	APPLICATION_DATA_TYPE = 3000,
	NODE_ID,
	SENSED_DATA_BLOB,
	SENSED_VALUE,
	TIME_STAMP
};

#ifndef _NRATTRIBUTES_CC_
extern NRSimpleAttributeFactory<AppDataType>      AppDataTypeAttr;
extern NRSimpleAttributeFactory<int>              NodeIDAttr;
extern NRSimpleAttributeFactory<AppData *>        SensedDataAttr;
extern NRSimpleAttributeFactory<double>		  SensedValueAttr;
extern NRSimpleAttributeFactory<float>            TimeStampAttr;
#endif

#endif
