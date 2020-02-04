/******************************************************************************
 * $Id: ogrmapgislayer.cpp 30004 2012-02-18 08:16:08Z fuxin $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRMapGISLayer class.
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

#if defined(_WIN32_WCE)
#  include <wce_errno.h>
#endif

CPL_CVSID("$Id: ogrmapgislayer.cpp 30004 2012-02-19 08:16:08Z fuxin $");

/************************************************************************/
/*                            CSVSplitLine()                            */
/*                                                                      */
/*      Tokenize a CSV line into fields in the form of a string         */
/*      list.  This is used instead of the CPLTokenizeString()          */
/*      because it provides correct CSV escaping and quoting            */
/*      semantics.                                                      */
/************************************************************************/

static char **MapGISSplitLine( const char *pszString, char chDelimiter )

{
	char        **papszRetList = NULL;
	char        *pszToken;
	int         nTokenMax, nTokenLen;

	pszToken = (char *) CPLCalloc(10,1);
	nTokenMax = 10;

	while( pszString != NULL && *pszString != '\0' )
	{
		int     bInString = FALSE;

		nTokenLen = 0;

		/* Try to find the next delimeter, marking end of token */
		for( ; *pszString != '\0'; pszString++ )
		{

			/* End if this is a delimeter skip it and break. */
			if( !bInString && *pszString == chDelimiter )
			{
				pszString++;
				break;
			}

			if( *pszString == '"' )
			{
				if( !bInString || pszString[1] != '"' )
				{
					bInString = !bInString;
					continue;
				}
				else  /* doubled quotes in string resolve to one quote */
				{
					pszString++;
				}
			}

			if( nTokenLen >= nTokenMax-2 )
			{
				nTokenMax = nTokenMax * 2 + 10;
				pszToken = (char *) CPLRealloc( pszToken, nTokenMax );
			}

			pszToken[nTokenLen] = *pszString;
			nTokenLen++;
		}

		pszToken[nTokenLen] = '\0';
		papszRetList = CSLAddString( papszRetList, pszToken );

		/* If the last token is an empty token, then we have to catch
		* it now, otherwise we won't reenter the loop and it will be lost.
		*/
		if ( *pszString == '\0' && *(pszString-1) == chDelimiter )
		{
			papszRetList = CSLAddString( papszRetList, "" );
		}
	}

	if( papszRetList == NULL )
		papszRetList = (char **) CPLCalloc(sizeof(char *),1);

	CPLFree( pszToken );

	return papszRetList;
}

/************************************************************************/
/*                      OGRCSVReadParseLineL()                          */
/*                                                                      */
/*      Read one line, and return split into fields.  The return        */
/*      result is a stringlist, in the sense of the CSL functions.      */
/************************************************************************/

char **OGRMapGISReadParseLineL( VSILFILE * fp, char chDelimiter, int bDontHonourStrings )

{
	const char  *pszLine;
	char        *pszWorkLine;
	char        **papszReturn;

	pszLine = CPLReadLineL( fp );
	if( pszLine == NULL )
		return( NULL );

	/* Special fix to read NdfcFacilities.xls that has non-balanced double quotes */
	if (chDelimiter == '\t' && bDontHonourStrings)
	{
		return CSLTokenizeStringComplex(pszLine, "\t", FALSE, TRUE);
	}

/* -------------------------------------------------------------------- */
/*      If there are no quotes, then this is the simple case.           */
/*      Parse, and return tokens.                                       */
/* -------------------------------------------------------------------- */
	if( strchr(pszLine,'\"') == NULL )
		return MapGISSplitLine( pszLine, chDelimiter );

/* -------------------------------------------------------------------- */
/*      We must now count the quotes in our working string, and as      */
/*      long as it is odd, keep adding new lines.                       */
/* -------------------------------------------------------------------- */
	pszWorkLine = CPLStrdup( pszLine );

	int i = 0, nCount = 0;
	int nWorkLineLength = strlen(pszWorkLine);

	while( TRUE )
	{
		for( ; pszWorkLine[i] != '\0'; i++ )
		{
			if( pszWorkLine[i] == '\"'
				&& (i == 0 || pszWorkLine[i-1] != '\\') )
				nCount++;
		}

		if( nCount % 2 == 0 )
			break;

		pszLine = CPLReadLineL( fp );
		if( pszLine == NULL )
			break;

		int nLineLen = strlen(pszLine);

		char* pszWorkLineTmp = (char *)
			VSIRealloc(pszWorkLine,
			nWorkLineLength + nLineLen + 2);
		if (pszWorkLineTmp == NULL)
			break;
		pszWorkLine = pszWorkLineTmp;
		strcat( pszWorkLine + nWorkLineLength, "\n" ); // This gets lost in CPLReadLine().
		strcat( pszWorkLine + nWorkLineLength, pszLine );

		nWorkLineLength += nLineLen + 1;
	}

	papszReturn = MapGISSplitLine( pszWorkLine, chDelimiter );

	CPLFree( pszWorkLine );

	return papszReturn;
}

/************************************************************************/
/*                           OGRMapGISLayer()                           */
/************************************************************************/

OGRMapGISLayer::OGRMapGISLayer(	const char *pszLayerNameIn,
							   VSILFILE *fp, int featureType )

{

	this->fp = fp;
	papoLayers = NULL;
	nLayers = 0;
	this->featureType = featureType;
	poFeatureDefn = new OGRFeatureDefn( pszLayerNameIn );
	OGRFieldDefn  oLayerField( "Layer", OFTString );
	poFeatureDefn->AddFieldDefn( &oLayerField );

	poFeatureDefn->Reference();
	poFeatureDefn->SetGeomType( wkbUnknown );
	int featureCount = atoi( CPLReadLineL(fp) );
	idataBufferOffset = 2;

	if( featureType == 3 )
	{
		for( int i = 0; i < featureCount; i++ )
		{
			CPLReadLineL( fp );
			idataBufferOffset++;
			CPLReadLineL( fp );
			idataBufferOffset++;
			CPLReadLineL( fp );
			idataBufferOffset++;
			int pointCount = atoi( CPLReadLineL(fp) );
			idataBufferOffset++;
			OGRFeature *poFeature = new OGRFeature( poFeatureDefn );
			OGRLineString *poLS = new OGRLineString();
			double dfX = 0.0, dfY = 0.0, dfZ = 0.0;
			char **papszTokens = NULL;

			for( int j = 0; j < pointCount; j++ )
			{
				papszTokens = OGRMapGISReadParseLineL( fp, ',', FALSE );
				idataBufferOffset++;
				if( *papszTokens == NULL )
				{
					delete poLS;
					return;
				}
				dfX = CPLAtof(papszTokens[0]);
				if( *( papszTokens+1 ) == NULL )
				{
					delete poLS;
					return;
				}
				dfY = CPLAtof(papszTokens[1]);

				poLS->addPoint( dfX, dfY, dfZ );
			}
			poFeature->SetGeometryDirectly( poLS );

			papszTokens = OGRMapGISReadParseLineL( fp, ',', FALSE );
			idataBufferOffset++;
			if( *papszTokens == NULL )
			{
				delete poLS;
				return;
			}
			long fID = atol(papszTokens[0]);
			arcFeatures[fID] = poFeature;
		}

		int nodeCount = atoi( CPLReadLineL(fp) );
		idataBufferOffset++;
		for( int j = 0; j < nodeCount-1; j++ )
		{
			CPLReadLineL( fp );
			idataBufferOffset++;
			int arcCount = atoi( CPLReadLineL(fp) );
			idataBufferOffset++;
			for( int k = 0; k < arcCount; k++ )
			{
				CPLReadLineL( fp );
				idataBufferOffset++;
			}
		}
		featureCount = atoi( CPLReadLineL(fp) );
		idataBufferOffset++;
	}
}

/************************************************************************/
/*                           ~OGRMapGISLayer()                          */
/************************************************************************/

OGRMapGISLayer::~OGRMapGISLayer()

{
	if( poFeatureDefn )
		poFeatureDefn->Release();
}

/************************************************************************/
/*                            CheckForQIX()                             */
/************************************************************************/

int OGRMapGISLayer::CheckForQIX()

{
    return NULL;
}

/************************************************************************/
/*                            ScanIndices()                             */
/*                                                                      */
/*      Utilize optional spatial and attribute indices if they are      */
/*      available.                                                      */
/************************************************************************/

int OGRMapGISLayer::ScanIndices()

{
    return TRUE;
}

/************************************************************************/
/*                            ResetReading()                            */
/************************************************************************/

void OGRMapGISLayer::ResetReading()

{
	VSIFSeekL( fp, 0, 0);
	for( int i = 0; i < idataBufferOffset; i++ )
		CPLReadLineL(fp);
}

/************************************************************************/
/*                           SetNextByIndex()                           */
/*                                                                      */
/*      If we already have an FID list, we can easily resposition       */
/*      ourselves in it.                                                */
/************************************************************************/

OGRErr OGRMapGISLayer::SetNextByIndex( long nIndex )

{
    return OGRERR_NONE;
}

/************************************************************************/
/*                            FetchMapGIS()                             */
/*                                                                      */
/*     Take a MapGIS id, a geometry, and a feature, and set the feature */
/*      if the MapGISid bbox intersects the geometry.                   */
/************************************************************************/

OGRFeature *OGRMapGISLayer::FetchMapGIS(int iMapGISId)

{
   
    return NULL;
}

/************************************************************************/
/*                      GetNextUnfilteredFeature()                      */
/************************************************************************/

OGRFeature *OGRMapGISLayer::GetNextUnfilteredFeature()
{
	char **papszTokens = NULL;
	OGRFeature *poFeature = new OGRFeature( poFeatureDefn );

	switch(featureType)
	{
	case 1:
		{
			double dfX = 0.0, dfY = 0.0, dfZ = 0.0;
			papszTokens = OGRMapGISReadParseLineL(fp, ',', FALSE);
			if ( *papszTokens == NULL )
				return NULL;
			//int nFieldCount = CSLCount( papszTokens );

			//const char *pszToken = papszTokens[0];
			//int bAllNumeric = TRUE;

			//if (*pszToken != '\0')
			//{
			//	while( *pszToken != '\0' && bAllNumeric )
			//	{
			//		if( *pszToken != '.' && *pszToken != '-'
			//			&& (*pszToken < '0' || *pszToken > '9') )
			//			bAllNumeric = FALSE;
			//		pszToken++;
			//	}
			//	if( !bAllNumeric )
			//		return 
			//}
			if( *( papszTokens + 1 ) == NULL )
				return NULL;
			dfX = CPLAtof(papszTokens[0]);
			dfY = CPLAtof(papszTokens[1]);

			poFeature->SetGeometryDirectly( new OGRPoint( dfX, dfY, dfZ ) );
			poFeature->SetField( "Layer", "WAT_1" );
			poFeature->SetFID( atol(papszTokens[2]) );
			break;
		}
	case 2:
		{
			int nPolylineFlag = 0;
 
			double      dfX = 0.0, dfY = 0.0, dfZ = 0.0;

			OGRLineString *poLS = new OGRLineString();

			const char* pszStr = CPLReadLineL( fp );
			if( *pszStr == NULL )
			{
				delete poLS;
				return NULL;
			}
			int ptCount = atoi( CPLReadLineL( fp ) );

			for( int i = 0; i < ptCount; i++ )
			{
				papszTokens = OGRMapGISReadParseLineL( fp, ',', FALSE );
				if( *papszTokens == NULL )
				{
					delete poLS;
					return NULL;
				}
				dfX = CPLAtof(papszTokens[0]);
				if( *( papszTokens+1 ) == NULL )
				{
					delete poLS;
					return NULL;
				}
				dfY = CPLAtof(papszTokens[1]);

				poLS->addPoint( dfX, dfY, dfZ );
			}
			CPLReadLineL( fp );

			poFeature->SetGeometryDirectly( poLS );
			poFeature->SetField( "Layer", "WAL_1" );
			break;
		}
	case 3:
		{
			OGRLinearRing *poLS = new OGRLinearRing();
			const char* pszStr = CPLReadLineL( fp );
			if( *pszStr == NULL )
			{
				delete poLS;
				return NULL;
			}
			const char* pszLine/* = CPLReadLineL( fp )*/;
			pszLine = CPLReadLineL( fp );
			int numOfArc = atoi( pszLine );
			for ( int i = 0; i < numOfArc - 1; i++ )
			{
				int arcID = atoi( CPLReadLineL( fp ) );
				OGRGeometry *ogrGeometry;
				OGRLineString *ogrLineString;
				if( arcID > 0 )
				{
					ogrGeometry = arcFeatures[arcID]->GetGeometryRef();
					ogrLineString = (OGRLineString *)ogrGeometry;
					int numOfPoints = ogrLineString->getNumPoints();
					for (int j = 0; j < numOfPoints; j++ )
					{
						OGRPoint *tempOGRPoint = new OGRPoint();
						ogrLineString->getPoint(j, tempOGRPoint);
						poLS->addPoint( tempOGRPoint );
					}

				}
				else
				{
					ogrGeometry = arcFeatures[-arcID]->GetGeometryRef();
					ogrLineString = (OGRLineString *)ogrGeometry;
					int numOfPoints = ogrLineString->getNumPoints();
					for (int j = numOfPoints - 1; j >= 0; j-- )
					{
						OGRPoint *tempOGRPoint = new OGRPoint();
						ogrLineString->getPoint(j, tempOGRPoint);
						tempOGRPoint->setZ( 0.0 );
						poLS->addPoint( tempOGRPoint );
					}
				}
			}
			OGRPolygon *ogrPolygon = new OGRPolygon;
			ogrPolygon->addRing( poLS );
			ogrPolygon->setCoordinateDimension( 2 );
			poFeature->SetGeometryDirectly( ogrPolygon );
			poFeature->SetField( "Layer", "WAP_1" );
			CPLReadLineL( fp );
			break;
		}
	}
	
	return poFeature;
}

/************************************************************************/
/*                           GetNextFeature()                           */
/************************************************************************/

OGRFeature *OGRMapGISLayer::GetNextFeature()

{
    OGRFeature  *poFeature = NULL;
/* -------------------------------------------------------------------- */
/*      Read features till we find one that satisfies our current       */
/*      spatial criteria.                                               */
/* -------------------------------------------------------------------- */
	while( TRUE )
	{
		poFeature = GetNextUnfilteredFeature();
		if( poFeature == NULL )
			break;

		if( (m_poFilterGeom == NULL
			|| FilterGeometry( poFeature->GetGeometryRef() ) )
			&& (m_poAttrQuery == NULL
			|| m_poAttrQuery->Evaluate( poFeature )) )
			break;

		delete poFeature;
	}
	return poFeature;
}

/************************************************************************/
/*                             GetFeature()                             */
/************************************************************************/

OGRFeature *OGRMapGISLayer::GetFeature( long nFeatureId )

{
	OGRFeature  *poFeature = NULL;

	return poFeature;
}

/************************************************************************/
/*                             SetFeature()                             */
/************************************************************************/

OGRErr OGRMapGISLayer::SetFeature( OGRFeature *poFeature )

{
	return NULL;
}

/************************************************************************/
/*                           DeleteFeature()                            */
/************************************************************************/

OGRErr OGRMapGISLayer::DeleteFeature( long nFID )

{
    return OGRERR_NONE;
}

/************************************************************************/
/*                           CreateFeature()                            */
/************************************************************************/

OGRErr OGRMapGISLayer::CreateFeature( OGRFeature *poFeature )

{
	return NULL;
}

/************************************************************************/
/*                          GetFeatureCount()                           */
/*                                                                      */
/*      If a spatial filter is in effect, we turn control over to       */
/*      the generic counter.  Otherwise we return the total count.      */
/*      Eventually we should consider implementing a more efficient     */
/*      way of counting features matching a spatial query.              */
/************************************************************************/

int OGRMapGISLayer::GetFeatureCount( int bForce )

{
	return NULL;
}

/************************************************************************/
/*                           TestCapability()                           */
/************************************************************************/

int OGRMapGISLayer::TestCapability( const char * pszCap )

{
        return FALSE;
}

/************************************************************************/
/*                            CreateField()                             */
/************************************************************************/

OGRErr OGRMapGISLayer::CreateField( OGRFieldDefn *poFieldDefn, int bApproxOK )

{
   return OGRERR_FAILURE;
}

/************************************************************************/
/*                           GetSpatialRef()                            */
/************************************************************************/

OGRSpatialReference *OGRMapGISLayer::GetSpatialRef()

{
    return NULL;
}

/************************************************************************/
/*                           ResetGeomType()                            */
/*                                                                      */
/*      Modify the geometry type for this file.  Used to convert to     */
/*      a different geometry type when a layer was created with a       */
/*      type of unknown, and we get to the first feature to             */
/*      establish the type.                                             */
/************************************************************************/

int OGRMapGISLayer::ResetGeomType( int nNewGeomType )

{

    return TRUE;
}

/************************************************************************/
/*                             SyncToDisk()                             */
/************************************************************************/

OGRErr OGRMapGISLayer::SyncToDisk()

{

    return OGRERR_NONE;
}

/************************************************************************/
/*                          DropSpatialIndex()                          */
/************************************************************************/

OGRErr OGRMapGISLayer::DropSpatialIndex()

{

    return OGRERR_NONE;
}

/************************************************************************/
/*                         CreateSpatialIndex()                         */
/************************************************************************/

OGRErr OGRMapGISLayer::CreateSpatialIndex( int nMaxDepth )

{
    return OGRERR_NONE;
}

/************************************************************************/
/*                               Repack()                               */
/*                                                                      */
/*      Repack the MapGIS and dbf file, dropping deleted records.       */
/*      FIDs may change.                                                */
/************************************************************************/

OGRErr OGRMapGISLayer::Repack()

{

    return OGRERR_NONE;
}
