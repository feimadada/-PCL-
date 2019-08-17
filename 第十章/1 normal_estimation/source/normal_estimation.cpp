#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/features/integral_image_normal.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/point_types.h>
#include <pcl/features/normal_3d.h>

int
main ()
 {
//���ص���
pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
pcl::io::loadPCDFile ("table_scene_lms400.pcd", *cloud);
//���Ʒ���
pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> ne;
ne.setInputCloud (cloud);
//����һ���յ�kdtree���󣬲��������ݸ����߹��ƶ���
//���ڸ������������ݼ���kdtree��������
pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ> ());
ne.setSearchMethod (tree);
//������ݼ�
pcl::PointCloud<pcl::Normal>::Ptr cloud_normals (new pcl::PointCloud<pcl::Normal>);
//ʹ�ð뾶�ڲ�ѯ����Χ3���׷�Χ�ڵ�������Ԫ��
ne.setRadiusSearch (0.03);
//��������ֵ
ne.compute (*cloud_normals);
// cloud_normals->points.size ()Ӧ����input cloud_downsampled->points.size ()����ͬ�ߴ�
//���߿��ӻ�
pcl::visualization::PCLVisualizer viewer("PCL Viewer");
viewer.setBackgroundColor (0.0, 0.0, 0.0);
viewer.addPointCloudNormals<pcl::PointXYZ,pcl::Normal>(cloud, cloud_normals);

while (!viewer.wasStopped ())
{
     viewer.spinOnce ();
}

return 0;
}
