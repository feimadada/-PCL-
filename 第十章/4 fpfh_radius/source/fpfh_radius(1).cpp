#include <iostream>
#include <pcl/point_types.h>
#include <pcl/features/feature_evaluation/feature_evaluation_framework.h>

int main (int argc, char** argv)
{
  if (argc < 4) {
    std::cout << "Specify the input cloud, ground truth and parameter files:\n";
    std::cout << "   " << argv[0] << " input_cloud.pcd ground_truth.txt parameters.txt\n";
    exit(0);
  }
  pcl::FeatureEvaluationFramework<pcl::PointXYZRGB> test_features;
  test_features.setFeatureTest ("FPFHTest");
  test_features.setGroundTruth (argv[2]);
  //���""��target-cloud�ļ��Ĳ�������ôsource-cloud������ֵ�任����ת���õ�target-cloud
  test_features.setInputClouds (argv[1], "", argv[1]);
  test_features.setThreshold (0.1f,1.0f,0.1f);
  //������������Ҫ���ֱ����ã�����ֵ�����ļ��ж�ȡ���������������Ϊ�����뾶
  //std::string parameters = "searchradius=0.05";
  //test_features.setParameters (parameters);
  test_features.setDownsampling (true);
  test_features.setLeafSize (0.01f);
  test_features.setVerbose (true);
  test_features.setLogFile ("fpfh-radius-variation.txt");
  test_features.runMultipleParameters (argv[3]);
  test_features.clearData ();
  return 0;
}	
