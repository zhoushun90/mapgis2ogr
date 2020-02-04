##### 1. 将ogr文件夹内9个文件拷贝到gdal-1.8.0\ogr\相应文件夹中:  
	本文夹内ogr\ogrsf_frmts\makefile.vc                替换 gdal-1.8.0\ogr\ogrsf_frmts\makefile.vc  
	本文夹内ogr\ogrsf_frmts\ogrsf_frmts.h              替换 gdal-1.8.0\ogr\ogrsf_frmts\ogrsf_frmts.h  
	本文夹内ogr\ogrsf_frmts\generic\makefile.vc        替换 gdal-1.8.0\ogr\ogrsf_frmts\generic\makefile.vc  
	本文夹内ogr\ogrsf_frmts\generic\ogrregisterall.cpp 替换 gdal-1.8.0\ogr\ogrsf_frmts\generic\ogrregisterall.cpp  
	本文夹内ogr\ogrsf_frmts\mapgis文件夹             拷贝到 gdal-1.8.0\ogr\ogrsf_frmts\下;  

##### 2. 重新生成gdal18.dll;

##### 3.修改D3A-ZX-2005中T3DView.cpp中代码：

	CFileDialog fdlg(TRUE,"*.dxf|*.shp|*.dwg","*.dxf",NULL,"*.dxf",NULL);  

	替换为：  

	CFileDialog fdlg(TRUE, NULL, "*.wat;*.wal;*wap",NULL, "*.wat;*.wal;*wap", NULL);  
	fdlg.m_ofn.lpstrFilter = "MapGIS file(*.wat;*.wal;*wap)\0*.wat;*.wal;*wap\0AutoCAD file(*.dxf)\0*.dxf\0Shape file(*shp)\0*.shp\0";  

##### 4.格式转换说明： 将Mapgis点转换为ogr中 wkbPoint25D 类型；
                 将Mapgis线转换为ogr中 wkbLineString25D 类型；
                 将Mapgis面转换为ogr中 wkbPolygon 类型。

##### 5. data文件夹里为实验数据。

##### 6. 本程序只是将mapgis明码数据转换为ogr数据格式。还没有写ogr转换mapgis格式。