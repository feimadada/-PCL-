#include <pcl/range_image/range_image.h>
int main (int argc, char** argv) {
  pcl::PointCloud<pcl::PointXYZ> pointCloud;
 //��������
  for (float y=-0.5f; y<=0.5f; y+=0.01f) {
    for (float z=-0.5f; z<=0.5f; z+=0.01f) {
      pcl::PointXYZ point;
      point.x = 2.0f - y;
      point.y = y;
      point.z = z;
      pointCloud.points.push_back(point);
    }
  }
  pointCloud.width = (uint32_t) pointCloud.points.size();
  pointCloud.height = 1;
//��1��Ϊ�Ƿֱ��ʣ������洴���ĵ��ƴ������ͼ��
  float angularResolution = (float) (  1.0f * (M_PI/180.0f));  
// 1��ת����
  float maxAngleWidth     = (float) (360.0f * (M_PI/180.0f));  
// 360.0��ת����
  float maxAngleHeight    = (float) (180.0f * (M_PI/180.0f)); 
// 180.0��ת����
  Eigen::Affine3f sensorPose = (Eigen::Affine3f)Eigen::Translation3f(0.0f, 0.0f, 0.0f);
  pcl::RangeImage::CoordinateFrame coordinate_frame = pcl::RangeImage::CAMERA_FRAME;
  float noiseLevel=0.00;
  float minRange = 0.0f;
  int borderSize = 1;
  pcl::RangeImage rangeImage;
  rangeImage.createFromPointCloud(pointCloud, angularResolution, maxAngleWidth, maxAngleHeight, sensorPose, coordinate_frame, noiseLevel, minRange, borderSize);
std::cout << rangeImage << "\n";
}
