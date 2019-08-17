#include <pcl/point_cloud.h>
#include <pcl/octree/octree.h>
#include <iostream>
#include <vector>
#include <ctime>
	
int
main (intargc, char**argv)
{
srand ((unsignedint) time (NULL));
// �˲����ֱ��� �����صĴ�С
float resolution =32.0f;
// ��ʼ���ռ�仯������
pcl::octree::OctreePointCloudChangeDetector<pcl::PointXYZ>octree (resolution);
pcl::PointCloud<pcl::PointXYZ>::PtrcloudA (newpcl::PointCloud<pcl::PointXYZ> );
//ΪcloudA��������
cloudA->width =128;
cloudA->height =1;
cloudA->points.resize (cloudA->width *cloudA->height);
for (size_ti=0; i<cloudA->points.size (); ++i)
  {
cloudA->points[i].x =64.0f* rand () / (RAND_MAX +1.0f);
cloudA->points[i].y =64.0f* rand () / (RAND_MAX +1.0f);
cloudA->points[i].z =64.0f* rand () / (RAND_MAX +1.0f);
  }
//��ӵ��Ƶ��˲����������˲���
octree.setInputCloud (cloudA);
octree.addPointsFromInputCloud ();
// �����˲������棬����cloudA��Ӧ�İ˲��������ڴ���
octree.switchBuffers ();
pcl::PointCloud<pcl::PointXYZ>::PtrcloudB (new pcl::PointCloud<pcl::PointXYZ> );
// ΪcloudB��������
cloudB->width =128;
cloudB->height =1;
cloudB->points.resize (cloudB->width *cloudB->height);
for (size_ti=0; i<cloudB->points.size (); ++i)
  {
cloudB->points[i].x =64.0f* rand () / (RAND_MAX +1.0f);
cloudB->points[i].y =64.0f* rand () / (RAND_MAX +1.0f);
cloudB->points[i].z =64.0f* rand () / (RAND_MAX +1.0f);
  }
//��� cloudB���˲���
octree.setInputCloud (cloudB);
octree.addPointsFromInputCloud ();
std::vector<int>newPointIdxVector;
//��ȡǰһcloudA��Ӧ�İ˲�����cloudB��Ӧ�˲�����û�е�����
octree.getPointIndicesFromNewVoxels (newPointIdxVector);
//��ӡ�����
std::cout<<"Output from getPointIndicesFromNewVoxels:"<<std::endl;
for (size_ti=0; i<newPointIdxVector.size (); ++i)
std::cout<<i<<"# Index:"<<newPointIdxVector[i]
<<"  Point:"<<cloudB->points[newPointIdxVector[i]].x <<" "
<<cloudB->points[newPointIdxVector[i]].y <<" "
<<cloudB->points[newPointIdxVector[i]].z <<std::endl;
}
