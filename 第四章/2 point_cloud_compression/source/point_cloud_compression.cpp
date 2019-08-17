#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/openni_grabber.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/compression/octree_pointcloud_compression.h>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>

#ifdef WIN32
# define sleep(x) Sleep((x)*1000)
#endif

class SimpleOpenNIViewer
{
public:
SimpleOpenNIViewer () :
viewer (" Point Cloud Compression Example")
  {
  }

void
cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr& cloud)
  {
if (!viewer.wasStopped ())
    {
// �洢ѹ�����Ƶ��ֽ���
std::stringstream compressedData;
// �������
pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloudOut (new pcl::PointCloud<pcl::PointXYZRGBA> ());
// ѹ������
PointCloudEncoder->encodePointCloud (cloud, compressedData);
// ��ѹ������
PointCloudDecoder->decodePointCloud (compressedData, cloudOut);
//���ӻ���ѹ������
viewer.showCloud (cloudOut);
    }
  }

void
run ()
  {
bool showStatistics=true;
// ѹ��ѡ����� /io/include/pcl/compression/compression_profiles.h
pcl::octree::compression_Profiles_e compressionProfile=pcl::octree::MED_RES_ONLINE_COMPRESSION_WITH_COLOR;
// ��ʼ��ѹ�����ѹ������
PointCloudEncoder=new pcl::octree::PointCloudCompression<pcl::PointXYZRGBA> (compressionProfile, showStatistics);
PointCloudDecoder=new pcl::octree::PointCloudCompression<pcl::PointXYZRGBA> ();
//������ OpenNI��ȡ���ƵĶ���
pcl::Grabber* interface =new pcl::OpenNIGrabber ();
//�����ص�����
    boost::function<void
   (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr&)> f = boost::bind (&SimpleOpenNIViewer::cloud_cb_, this, _1);
// �����ص�������ص��ź�֮����ϵ
boost::signals2::connection c = interface->registerCallback (f);
// ��ʼ���յ���������
interface->start ();
while (!viewer.wasStopped ())
    {
sleep (1);
    }
interface->stop ();
// ɾ������ѹ�����ѹ������ʵ��
delete (PointCloudEncoder);
delete (PointCloudDecoder);
  }
pcl::visualization::CloudViewer viewer;
pcl::octree::PointCloudCompression<pcl::PointXYZRGBA>*PointCloudEncoder;
pcl::octree::PointCloudCompression<pcl::PointXYZRGBA>*PointCloudDecoder;
};

int
main (int argc, char**argv)
{
SimpleOpenNIViewer v;
v.run ();
return (0);
}	
