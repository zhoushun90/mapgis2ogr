/******************************************************************************
 * $Id: ogrmapgisdriver.cpp 30002 2011-2-18 14:23:13Z china fuxin $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRMapGISDriver class.
 * Author:   ZhouShun, shunzhou@foxmail.com
 *
 ******************************************************************************
 * Copyright (c) 1999,  Les Technologies SoftMap Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "ogr_mapgis.h"
#include "cpl_conv.h"
#include "cpl_string.h"

CPL_CVSID("$Id: ogrmapgisdriver.cpp 30002 2011-2-18 14:23:13Z china fuxin $");

/************************************************************************/
/*                          ~OGRMapGISDriver()                           */
/************************************************************************/

OGRMapGISDriver::~OGRMapGISDriver()

{
}

/************************************************************************/
/*                              GetName()                               */
/************************************************************************/

const char *OGRMapGISDriver::GetName()

{
    return "MapGISfile";
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

OGRDataSource *OGRMapGISDriver::Open( const char * pszFilename,
                                     int bUpdate )
{
	OGRMapGISDataSource   *poDS = new OGRMapGISDataSource();

	if( !poDS->Open( pszFilename ) )
	{
		delete poDS;
		poDS = NULL;
	}

	return poDS;
}

/************************************************************************/
/*                          CreateDataSource()                          */
/************************************************************************/

OGRDataSource *OGRMapGISDriver::CreateDataSource( const char * pszName,
                                                 char **papszOptions )

{
   
	return NULL;
}

/************************************************************************/
/*                          DeleteDataSource()                          */
/************************************************************************/

OGRErr OGRMapGISDriver::DeleteDataSource( const char *pszDataSource )

{
    
    return OGRERR_NONE;
}


/************************************************************************/
/*                           TestCapability()                           */
/************************************************************************/

int OGRMapGISDriver::TestCapability( const char * pszCap )

{
        return FALSE;
}

/************************************************************************/
/*                          RegisterOGRMapGIS()                         */
/************************************************************************/

void RegisterOGRMapGIS()
{
    OGRSFDriverRegistrar::GetRegistrar()->RegisterDriver( new OGRMapGISDriver );
}

