/******************************************************************************
 * $Id: ogr_mapgis.h 30001 2011-2-18 08:00:13Z china fuxin $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Private definitions within the MapGISfile driver to implement
 *           integration with OGR.
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

#ifndef _OGRMapGIS_H_INCLUDED
#define _OGRMapGIS_H_INCLUDED

#include "ogrsf_frmts.h"
#include <map>

/************************************************************************/
/*                            OGRMapGISLayer                             */
/************************************************************************/

class OGRMapGISLayer : public OGRLayer
{
	VSILFILE           *fp;
	int                featureType; 
	OGRMapGISLayer       **papoLayers;
	int                 nLayers;
    OGRFeatureDefn     *poFeatureDefn;
	int                 idataBufferOffset; // Êý¾ÝÎ»ÖÃ
    std::map<long, OGRFeature*> arcFeatures;

    OGRSpatialReference *poSRS;

    int                 iNextMapGISId;
    int                 nTotalMapGISCount;

    char                *pszFullName;

    int                 bUpdateAccess;

    OGRwkbGeometryType  eRequestedGeomType;
    int                 ResetGeomType( int nNewType );

    int                 ScanIndices();

    long               *panMatchingFIDs;
    int                 iMatchingFID;

    int                 bHeaderDirty;

    int                 bCheckedForQIX;

    int                 CheckForQIX();

    int                 bSbnSbxDeleted;

  public:
    OGRErr              CreateSpatialIndex( int nMaxDepth );
    OGRErr              DropSpatialIndex();
    OGRErr              Repack();

    const char         *GetFullName() { return pszFullName; }

  public:
                        OGRMapGISLayer(	const char *pszLayerNameIn,
							VSILFILE *fp, int featureType);
                        ~OGRMapGISLayer();

    void                ResetReading();
    OGRFeature *        FetchMapGIS(int iMapGISId);
    OGRFeature *        GetNextFeature();
	OGRFeature *		GetNextUnfilteredFeature();
    virtual OGRErr      SetNextByIndex( long nIndex );

    OGRFeature         *GetFeature( long nFeatureId );
    OGRErr              SetFeature( OGRFeature *poFeature );
    OGRErr              DeleteFeature( long nFID );
    OGRErr              CreateFeature( OGRFeature *poFeature );
    OGRErr              SyncToDisk();
    
    OGRFeatureDefn *    GetLayerDefn() { return poFeatureDefn; }

    int                 GetFeatureCount( int );

    virtual OGRErr      CreateField( OGRFieldDefn *poField,
                                     int bApproxOK = TRUE );

    virtual OGRSpatialReference *GetSpatialRef();
    
    int                 TestCapability( const char * );
};

/************************************************************************/
/*                          OGRMapGISDataSource                         */
/************************************************************************/

class OGRMapGISDataSource : public OGRDataSource
{
	VSILFILE           *fp;

    OGRMapGISLayer     **papoLayers;
    int                 nLayers;
    
    char                *pszName;

    int                 bDSUpdate;

    int                 bSingleFileDataSource;

  public:
                        OGRMapGISDataSource();
                        ~OGRMapGISDataSource();

    int                 Open( const char *, int bUpdate = FALSE/*, int bTestOpen,
                              int bForceSingleFileDataSource = FALSE */);
    int                 OpenFile( const char *, int bUpdate, int bTestOpen );

    const char          *GetName() { return pszName; }
    int                 GetLayerCount() { return nLayers; }
    OGRLayer            *GetLayer( int );

    virtual OGRLayer    *CreateLayer( const char *, 
                                      OGRSpatialReference * = NULL,
                                      OGRwkbGeometryType = wkbUnknown,
                                      char ** = NULL );

    virtual OGRLayer    *ExecuteSQL( const char *pszStatement,
                                     OGRGeometry *poSpatialFilter,
                                     const char *pszDialect );

    virtual int          TestCapability( const char * );
    virtual OGRErr       DeleteLayer( int iLayer );
};

/************************************************************************/
/*                            OGRMapGISDriver                            */
/************************************************************************/

class OGRMapGISDriver : public OGRSFDriver
{
  public:
                ~OGRMapGISDriver();
                
    const char *GetName();
    OGRDataSource *Open( const char *, int );

    virtual OGRDataSource *CreateDataSource( const char *pszName,
                                             char ** = NULL );
    OGRErr              DeleteDataSource( const char *pszDataSource );
    
    int                 TestCapability( const char * );
};


#endif /* ndef _OGRMapGIS_H_INCLUDED */
