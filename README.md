##### 1. ��ogr�ļ�����9���ļ�������gdal-1.8.0\ogr\��Ӧ�ļ�����:  
	���ļ���ogr\ogrsf_frmts\makefile.vc                �滻 gdal-1.8.0\ogr\ogrsf_frmts\makefile.vc  
	���ļ���ogr\ogrsf_frmts\ogrsf_frmts.h              �滻 gdal-1.8.0\ogr\ogrsf_frmts\ogrsf_frmts.h  
	���ļ���ogr\ogrsf_frmts\generic\makefile.vc        �滻 gdal-1.8.0\ogr\ogrsf_frmts\generic\makefile.vc  
	���ļ���ogr\ogrsf_frmts\generic\ogrregisterall.cpp �滻 gdal-1.8.0\ogr\ogrsf_frmts\generic\ogrregisterall.cpp  
	���ļ���ogr\ogrsf_frmts\mapgis�ļ���             ������ gdal-1.8.0\ogr\ogrsf_frmts\��;  

##### 2. ��������gdal18.dll;

##### 3.�޸�D3A-ZX-2005��T3DView.cpp�д��룺

	CFileDialog fdlg(TRUE,"*.dxf|*.shp|*.dwg","*.dxf",NULL,"*.dxf",NULL);  

	�滻Ϊ��  

	CFileDialog fdlg(TRUE, NULL, "*.wat;*.wal;*wap",NULL, "*.wat;*.wal;*wap", NULL);  
	fdlg.m_ofn.lpstrFilter = "MapGIS file(*.wat;*.wal;*wap)\0*.wat;*.wal;*wap\0AutoCAD file(*.dxf)\0*.dxf\0Shape file(*shp)\0*.shp\0";  

##### 4.��ʽת��˵���� ��Mapgis��ת��Ϊogr�� wkbPoint25D ���ͣ�
                 ��Mapgis��ת��Ϊogr�� wkbLineString25D ���ͣ�
                 ��Mapgis��ת��Ϊogr�� wkbPolygon ���͡�

##### 5. data�ļ�����Ϊʵ�����ݡ�

##### 6. ������ֻ�ǽ�mapgis��������ת��Ϊogr���ݸ�ʽ����û��дogrת��mapgis��ʽ��