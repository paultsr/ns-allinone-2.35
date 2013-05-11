#ifndef _ATTRSEXPORTER_H_
#define _ATTRSEXPORTER_H_

#include <nr.hh>
#include "nrAttributes.h"

/* AppDataAttrsExporter
 * -- J�lio C�sar e Melo <julio.melo@gmail.com>
 *
 * This is a interface for any sensed data sent
 * by CommonNodeDiffApp.
 *
 * First idea was to implements a interface called
 * AttrsExporter and to multiple-inherits the sensed
 * data (AppData and AttrsExporter).
 *
 * As multiple-inheritance may cause calling virtual
 * function problems, AppDataAttrsExporter inherits
 * from AppData.
 */
class AppDataAttrsExporter : public AppData
{
	public:
		AppDataAttrsExporter(AppDataType type) : AppData(type)
		{ }

		/* Set attributes on attributes vector */
		virtual void setAttributes(NRAttrVec * attrs) = 0;
};

typedef AppDataAttrsExporter AttrsExporter;

#endif
