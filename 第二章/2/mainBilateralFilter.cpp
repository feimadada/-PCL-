#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
typedef pcl::PointXYZI PointT;

float
 G (float x, float sigma)
 {
return exp (- (x*x)/(2*sigma*sigma));
 }

int
main (int argc, char*argv[])
 {
std::string incloudfile = argv[1];
std::string outcloudfile = argv[2];
float sigma_s = atof (argv[3]);
float sigma_r = atof (argv[4]);
// ��pcd�ļ����ص�������
   pcl::PointCloud<PointT>::Ptr cloud (new pcl::PointCloud<PointT>);
pcl::io::loadPCDFile (incloudfile.c_str (), *cloud);
int pnumber = (int)cloud->size ();
// δ�˲�֮ǰ�������������������
   pcl::PointCloud<PointT> outcloud =*cloud;
// ����kdtree
   pcl::KdTreeFLANN<PointT>::Ptr tree (new pcl::KdTreeFLANN<PointT>);
tree->setInputCloud (cloud);
// ������ش洢
   std::vector<int> k_indices;
   std::vector<float> k_distances;
// �ؼ�����ѭ��
for (int point_id =0; point_id < pnumber; ++point_id)
   {
float BF =0;
float W =0;
tree->radiusSearch (point_id, 2* sigma_s, k_indices, k_distances);
// ����ÿ�����ڽ���һ�¼���
for (size_t n_id =0; n_id < k_indices.size (); ++n_id)
     {
float id = k_indices.at (n_id);
float dist = sqrt (k_distances.at (n_id));
float intensity_dist = abs (cloud->points[point_id].intensity - cloud->points[id].intensity);
float w_a = G (dist, sigma_s);
float w_b = G (intensity_dist, sigma_r);
float weight = w_a * w_b;
       BF += weight * cloud->points[id].intensity;
       W += weight;
     }
outcloud.points[point_id].intensity = BF / W;
   }
// �洢�˲���Ľ�����Ƶ��ļ�
pcl::io::savePCDFile (outcloudfile.c_str (), outcloud);
return (0);
 }
