#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/console/time.h>
#include <iostream>
#include <ostream>
#include <pcl/filters/voxel_grid.h>
#include <pcl/features/normal_3d.h>
#include <pcl/segmentation/conditional_euclidean_clustering.h>
#include <pcl/console/parse.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/point_cloud_color_handlers.h>
typedef pcl::PointXYZI PointTypeIO;
typedef pcl::PointXYZINormal PointTypeFull;
typedef pcl::visualization::PointCloudColorHandler<pcl::PCLPointCloud2> ColorHandler;
typedef ColorHandler::Ptr ColorHandlerPtr;
typedef ColorHandler::ConstPtr ColorHandlerConstPtr;
using namespace pcl::console;
//������������1
bool
	enforceIntensitySimilarity (const PointTypeFull& point_a, const PointTypeFull& point_b, float squared_distance)
{
	if (fabs ((float)point_a.intensity - (float)point_b.intensity) < 5.0f)
		return (true);
	else
		return (false);
}
//������������2
bool
	enforceCurvatureOrIntensitySimilarity (const PointTypeFull& point_a, const PointTypeFull& point_b, float squared_distance)
{
	Eigen::Map<const Eigen::Vector3f> point_a_normal = point_a.normal, point_b_normal = point_b.normal;
	if (fabs ((float)point_a.intensity - (float)point_b.intensity) < 5.0f)
		return (true);
	if (fabs (point_a_normal.dot (point_b_normal)) < 0.05)
		return (true);
	return (false);
}
//������������3
bool
	customRegionGrowing (const PointTypeFull& point_a, const PointTypeFull& point_b, float squared_distance)
{
	Eigen::Map<const Eigen::Vector3f> point_a_normal = point_a.normal, point_b_normal = point_b.normal;
	if (squared_distance < 10000)
	{
		if (fabs ((float)point_a.intensity - (float)point_b.intensity) < 8.0f)
			return (true);
		if (fabs (point_a_normal.dot (point_b_normal)) < 0.06)
			return (true);
	}
	else
	{
		if (fabs ((float)point_a.intensity - (float)point_b.intensity) < 3.0f)
			return (true);
	}
	return (false);
}

int
	main (int argc, char** argv)
{

	if(argc<2)
	{
		std::cout<<".exe xx.pcd -l 40 -r 300.0 -v 1 -m 1/2/3"<<std::endl;

		return 0;
	}//����������С��1���������ʾ
	bool Visual=true;
	//����Ĭ���������
	float Leaf=40,Radius=300;
	int Method=1;
	//�������������ʽ
	parse_argument (argc, argv, "-l", Leaf);
	parse_argument (argc, argv, "-r", Radius);
	parse_argument (argc, argv, "-v", Visual);
	parse_argument (argc, argv, "-m", Method);
	// Data containers used
	pcl::PointCloud<PointTypeIO>::Ptr cloud_in (new pcl::PointCloud<PointTypeIO>), cloud_out (new pcl::PointCloud<PointTypeIO>);//����PointCloud <pcl::PointXYZI>����ָ�벢����ʵ����
	pcl::PointCloud<PointTypeFull>::Ptr cloud_with_normals (new pcl::PointCloud<PointTypeFull>);//����PointCloud <pcl::PointXYZINormal>����ָ�벢����ʵ����
	pcl::IndicesClustersPtr clusters (new pcl::IndicesClusters), small_clusters (new pcl::IndicesClusters), large_clusters (new pcl::IndicesClusters);
	pcl::search::KdTree<PointTypeIO>::Ptr search_tree (new pcl::search::KdTree<PointTypeIO>);
	pcl::console::TicToc tt;

	// Load the input point cloud
	std::cerr << "Loading...\n", tt.tic ();
	pcl::io::loadPCDFile (argv[1], *cloud_in);//����������ļ�
	std::cerr << ">> Done: " << tt.toc () << " ms, " << cloud_in->points.size () << " points\n";

	// Downsample the cloud using a Voxel Grid class
	std::cerr << "Downsampling...\n", tt.tic ();
	pcl::VoxelGrid<PointTypeIO> vg;//�����˲�����
	vg.setInputCloud (cloud_in);//������Ҫ���˵ĵ��Ƹ��˲�����
	vg.setLeafSize (Leaf,Leaf,Leaf);//�����˲�ʱ������դ��߳�
	vg.setDownsampleAllData (true);//�������е���ֵ����Ҫ���²���
	vg.filter (*cloud_out);//ִ���˲������洢���cloud_out
	std::cerr << ">> Done: " << tt.toc () << " ms, " << cloud_out->points.size () << " points\n";

	// Set up a Normal Estimation class and merge data in cloud_with_normals
	std::cerr << "Computing normals...\n", tt.tic ();
	pcl::copyPointCloud (*cloud_out, *cloud_with_normals);//���Ƶ���
	pcl::NormalEstimation<PointTypeIO, PointTypeFull> ne;//�������߹��ƶ���
	ne.setInputCloud (cloud_out);//���÷��߹��ƶ�������㼯
	ne.setSearchMethod (search_tree);//������������
	ne.setRadiusSearch (Radius);// ���������뾶
	ne.compute (*cloud_with_normals);//���㲢���������
	std::cerr << ">> Done: " << tt.toc () << " ms\n";

	// Set up a Conditional Euclidean Clustering class
	std::cerr << "Segmenting to clusters...\n", tt.tic ();
	pcl::ConditionalEuclideanClustering<PointTypeFull> cec (true);//������������ָ���󣬲����г�ʼ����
	cec.setInputCloud (cloud_with_normals);//��������㼯
	//����ѡ��ͬ��������
	switch(Method)
	{
	case 1:
		cec.setConditionFunction (&enforceIntensitySimilarity);
		break;
	case 2:
		cec.setConditionFunction (&enforceCurvatureOrIntensitySimilarity);
		break;
	case 3:
		cec.setConditionFunction (&customRegionGrowing);
		break;
	default:
		cec.setConditionFunction (&customRegionGrowing);
		break;
	}

	cec.setClusterTolerance (500.0);//���þ���ο������������
	cec.setMinClusterSize (cloud_with_normals->points.size () / 1000);//���ù�С����ı�׼
	cec.setMaxClusterSize (cloud_with_normals->points.size () / 5);//���ù������ı�׼
	cec.segment (*clusters);//��ȡ����Ľ�����ָ��������ڵ���������������
	cec.getRemovedClusters (small_clusters, large_clusters);//��ȡ��Ч�ߴ�ľ���
	std::cerr << ">> Done: " << tt.toc () << " ms\n";
	
	for (int i = 0; i < small_clusters->size (); ++i)
		for (int j = 0; j < (*small_clusters)[i].indices.size (); ++j)
			cloud_out->points[(*small_clusters)[i].indices[j]].intensity = -2.0;

	for (int i = 0; i < large_clusters->size (); ++i)
		for (int j = 0; j < (*large_clusters)[i].indices.size (); ++j)
			cloud_out->points[(*large_clusters)[i].indices[j]].intensity = +10.0;
	
	for (int i = 0; i < clusters->size (); ++i)
	{
		int label = rand () % 8;
		for (int j = 0; j < (*clusters)[i].indices.size (); ++j)
			cloud_out->points[(*clusters)[i].indices[j]].intensity = label;
	}
	// Save the output point cloud
	if(0)
	{//���ӻ����ְ����д��󡣴��޸ģ�
		
		boost::shared_ptr<pcl::visualization::PCLVisualizer> MView (new pcl::visualization::PCLVisualizer ("���ƿ�PCLѧϰ�̵̳ڶ���-�����ָ��")); 
		//View-Port1 
		int v1(0); 
		MView->createViewPort (0.0, 0.0, 0.5, 1.0, v1); //�����ӿ�1�ļ��β���
		MView->setBackgroundColor (1, 0.2, 1); //�����ӿ�1�ı���
		MView->addText ("Before segmentation", 10, 10, "Before segmentation", v1); //Ϊ�ӿ�1��ӱ�ǩ
		int v2(0); 
		MView->createViewPort (0.5, 0.0, 1.0, 1.0, v2); 
		MView->setBackgroundColor (0.5, 0.5,0.5, v2); 
		MView->addText ("After segmentation", 10, 10, "After segmentation", v2); 
		//pcl::visualization::PointCloudColorHandlerLabelField<pcl::PointXYZI>::Ptr color_handler(new pcl::visualization::PointCloudColorHandlerLabelField<pcl::PointXYZI>());
		/*pcl::PCLPointCloud2::Ptr cloud;
		ColorHandlerPtr color_handler;
		pcl::fromPCLPointCloud2 (*cloud, *cloud_out);
		Eigen::Vector4f origin=Eigen::Vector4f::Identity();
		Eigen::Quaternionf orientation=Eigen::Quaternionf::Identity();

		color_handler.reset (new pcl::visualization::PointCloudColorHandlerGenericField<pcl::PCLPointCloud2> (cloud,"intensity"));*/
		MView->addPointCloud<pcl::PointXYZI>(cloud_in,"input",v1);//�����ӿ�1���������
		//MView->addPointCloud(cloud,color_handler,origin, orientation,"output",v2);
		MView->addPointCloud<pcl::PointXYZI>(cloud_out,"output",v2);
		MView->spin();
	}
	else
	{
		std::cerr << "Saving...\n", tt.tic ();
		pcl::io::savePCDFile ("output.pcd", *cloud_out);
		std::cerr << ">> Done: " << tt.toc () << " ms\n";
	}


	return (0);
}