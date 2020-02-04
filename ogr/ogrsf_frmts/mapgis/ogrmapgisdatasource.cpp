/******************************************************************************
 * $Id: ogrmapgisdatasource.cpp 30003 2012-02-18 08:23:13Z fuxin $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRMapGISDataSource class.
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
#include "cpl_csv.h"

CPL_CVSID("Id: ogrmapgisdatasource.cpp 30003 2012-02-14 08:23:13Z fuxin $");

/************************************************************************/
/*                         OGRMapGISDataSource()                         */
/************************************************************************/

OGRMapGISDataSource::OGRMapGISDataSource()

{
    pszName = NULL;
    papoLayers = NULL;
    nLayers = 0;
    bSingleFileDataSource = FALSE;
}

/************************************************************************/
/*                        ~OGRMapGISDataSource()                        */
/************************************************************************/

OGRMapGISDataSource::~OGRMapGISDataSource()

{
    CPLFree( pszName );

    for( int i = 0; i < nLayers; i++ )
    {
        CPLAssert( NULL != papoLayers[i] );

        delete papoLayers[i];
    }
    
    CPLFree( papoLayers );
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

int OGRMapGISDataSource::Open( const char * pszNewName, int bUpdate/*,
                              int bTestOpen, int bForceSingleFileDataSource*/ )

{    
	if( !EQUAL(CPLGetExtension(pszNewName),"wat") &&
		!EQUAL(CPLGetExtension(pszNewName),"wal") &&
		!EQUAL(CPLGetExtension(pszNewName),"wap") )
		return FALSE;

/* -------------------------------------------------------------------- */
/*      Open the file.                                                  */
/* -------------------------------------------------------------------- */
	fp = VSIFOpenL( pszNewName, "r" );
	if( fp == NULL )
		return FALSE;

/* -------------------------------------------------------------------- */
/*      Confirm we have a header section.                               */
/* -------------------------------------------------------------------- */
	int featureType = -1;
	const char* pszLine = CPLReadLineL( fp );
	if( EQUAL( pszLine, "WMAP9022" ) )
		featureType = 1;
	if( EQUAL( pszLine, "WMAP9021" ) )
		featureType = 2;
	if( EQUAL( pszLine, "WMAP9023" ) )
		featureType = 3;
	if( featureType == -1 )
		return FALSE;

/* -------------------------------------------------------------------- */
/*      Create a layer.                                                 */
/* -------------------------------------------------------------------- */
	nLayers++;
	papoLayers = (OGRMapGISLayer **) CPLRealloc(papoLayers, 
		sizeof(void*) * nLayers);

	CPLString osLayerName = CPLGetBasename(pszNewName);
	papoLayers[nLayers-1] = 
		new OGRMapGISLayer("layer", fp, featureType);

	return TRUE;
}

/************************************************************************/
/*                           TestCapability()                           */
/************************************************************************/

int OGRMapGISDataSource::TestCapability( const char * pszCap )

{
	return FALSE;
}

/************************************************************************/
/*                              OpenFile()                              */
/************************************************************************/

int OGRMapGISDataSource::OpenFile( const char *pszNewName, int bUpdate,
                                  int bTestOpen )
{
    return FALSE;
}

/************************************************************************/
/*                            CreateLayer()                             */
/************************************************************************/

OGRLayer *
OGRMapGISDataSource::CreateLayer( const char * pszLayerName,
                                 OGRSpatialReference *poSRS,
                                 OGRwkbGeometryType eType,
                                 char ** papszOptions )

{

	return FALSE;
}

/************************************************************************/
/*                              GetLayer()                              */
/************************************************************************/

OGRLayer *OGRMapGISDataSource::GetLayer( int iLayer )

{
	if( iLayer < 0 || iLayer >= nLayers )
		return NULL;
	else
		return papoLayers[iLayer];
}

/************************************************************************/
/*                             ExecuteSQL()                             */
/*                                                                      */
/*      We override this to provide special handling of CREATE          */
/*      SPATIAL INDEX commands.  Support forms are:                     */
/*                                                                      */
/*        CREATE SPATIAL INDEX ON layer_name [DEPTH n]                  */
/*        DROP SPATIAL INDEX ON layer_name                              */
/*        REPACK layer_name                                             */
/************************************************************************/

OGRLayer * OGRMapGISDataSource::ExecuteSQL( const char *pszStatement,
                                           OGRGeometry *poSpatialFilter,
                                           const char *pszDialect )

{

    return NULL;
}


/************************************************************************/
/*                            DeleteLayer()                             */
/************************************************************************/

OGRErr OGRMapGISDataSource::DeleteLayer( int iLayer )

{
   
    return OGRERR_NONE;
}
