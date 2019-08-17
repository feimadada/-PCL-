#include <pcl/console/parse.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/segmentation/supervoxel_clustering.h>

// Types
typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloudT;
typedef pcl::PointNormal PointNT;
typedef pcl::PointCloud<PointNT> PointNCloudT;
typedef pcl::PointXYZL PointLT;
typedef pcl::PointCloud<PointLT> PointLCloudT;

void addSupervoxelConnectionsToViewer (PointT &supervoxel_center,
	PointCloudT &adjacent_supervoxel_centers,
	std::string supervoxel_name,
	boost::shared_ptr<pcl::visualization::PCLVisualizer> & viewer);


int
	main (int argc, char ** argv)
{
	if (argc < 2)
	{
		pcl::console::print_error ("Syntax is: %s <pcd-file> \n "
			"--NT Dsables the single cloud transform \n"
			"-v <voxel resolution>\n-s <seed resolution>\n"
			"-c <color weight> \n-z <spatial weight> \n"
			"-n <normal_weight>\n", argv[0]);
		return (1);
	}


	PointCloudT::Ptr cloud = boost::make_shared <PointCloudT> ();
	pcl::console::print_highlight ("Loading point cloud...\n");
	if (pcl::io::loadPCDFile<PointT> (argv[1], *cloud))
	{
		pcl::console::print_error ("Error loading cloud file!\n");
		return (1);
	}
	cout<<"point size of input: "<<cloud->size()<<endl;

	bool disable_transform = pcl::console::find_switch (argc, argv, "--NT");

	float voxel_resolution = 0.008f;
	bool voxel_res_specified = pcl::console::find_switch (argc, argv, "-v");
	if (voxel_res_specified)
		pcl::console::parse (argc, argv, "-v", voxel_resolution);

	float seed_resolution = 0.1f;
	bool seed_res_specified = pcl::console::find_switch (argc, argv, "-s");
	if (seed_res_specified)
		pcl::console::parse (argc, argv, "-s", seed_resolution);

	float color_importance = 0.2f;
	if (pcl::console::find_switch (argc, argv, "-c"))
		pcl::console::parse (argc, argv, "-c", color_importance);

	float spatial_importance = 0.4f;
	if (pcl::console::find_switch (argc, argv, "-z"))
		pcl::console::parse (argc, argv, "-z", spatial_importance);

	float normal_importance = 1.0f;
	if (pcl::console::find_switch (argc, argv, "-n"))
		pcl::console::parse (argc, argv, "-n", normal_importance);

	//////////////////////////////  //////////////////////////////
	////// This is how to use supervoxels
	//////////////////////////////  //////////////////////////////

	pcl::SupervoxelClustering<PointT> super (voxel_resolution, seed_resolution);
	if (disable_transform)
		super.setUseSingleCameraTransform (false);
	super.setInputCloud (cloud);
	super.setColorImportance (color_importance);
	super.setSpatialImportance (spatial_importance);
	super.setNormalImportance (normal_importance);

	std::map <uint32_t, pcl::Supervoxel<PointT>::Ptr > supervoxel_clusters;
	//�õ�ӳ�������Ա�ǩΪ��ֵ�洢���г�����
	pcl::console::print_highlight ("Extracting supervoxels!\n");
	super.extract (supervoxel_clusters);
	pcl::console::print_info ("Found %d supervoxels\n", supervoxel_clusters.size ());

	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer (new pcl::visualization::PCLVisualizer ("���ƿ�PCLѧϰ�̵̳ڶ���-�����طָ�"));
	viewer->setBackgroundColor (1,1,1);

	PointCloudT::Ptr voxel_centroid_cloud = super.getVoxelCentroidCloud ();
	cout<<"voxel centroids: "<<voxel_centroid_cloud->size()<<endl;
	if(0)
	{//�����������ĵĿ��ӻ��ͱ��棬�������Ƕ�ԭʼ���ݵĿռ�����²���
		viewer->addPointCloud<PointT>(voxel_centroid_cloud,"voxel centroids");
		pcl::io::savePCDFile("voxel_centroids.pcd",*voxel_centroid_cloud);
		viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE,4, "voxel centroids");
		viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_OPACITY,0.5, "voxel centroids");
	}


	PointLCloudT::Ptr labeled_voxel_cloud = super.getLabeledVoxelCloud ();
	if(1)
	{//�����طָ�����ʾ�뱣��
		pcl::io::savePCDFile("labeled_voxels.pcd",*labeled_voxel_cloud);
		viewer->addPointCloud (labeled_voxel_cloud, "labeled voxels");
		cout<<"labeled voxels: "<<labeled_voxel_cloud->size()<<endl;
		viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE,3, "labeled voxels");
		// viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_OPACITY,0.8, "labeled voxels");
	}

	PointNCloudT::Ptr sv_normal_cloud = super.makeSupervoxelNormalCloud (supervoxel_clusters);
	//
	if(0)//�����ض�Ӧ�ķ����������ӻ�
		viewer->addPointCloudNormals<pcl::PointNormal> (sv_normal_cloud,1,0.05f, "supervoxel_normals");

	pcl::console::print_highlight ("Getting supervoxel adjacency\n");
	std::multimap<uint32_t, uint32_t> supervoxel_adjacency;
	super.getSupervoxelAdjacency (supervoxel_adjacency);
	cout<<"size of supervoxel_adjacency: "<<supervoxel_adjacency.size()<<endl;

	//��������ӳ�����������ڽ�ͼ
	std::multimap<uint32_t,uint32_t>::iterator label_itr = supervoxel_adjacency.begin ();
	for ( ; label_itr != supervoxel_adjacency.end (); )
	{
		//��ȡ��ǩֵ
		uint32_t supervoxel_label = label_itr->first;
		//���ݱ�ǩ�������ó�����
		pcl::Supervoxel<PointT>::Ptr supervoxel = supervoxel_clusters.at (supervoxel_label);

		//�����ó��������ڳ����ز��������ڳ���������Ϊ�㼯������ƣ����ں������ӻ�����������ڳ������ڶ���ӳ�������о�����ͬ�ļ�ֵ
		PointCloudT adjacent_supervoxel_centers;
		std::multimap<uint32_t,uint32_t>::iterator adjacent_itr = supervoxel_adjacency.equal_range (supervoxel_label).first;
		for ( ; adjacent_itr!=supervoxel_adjacency.equal_range (supervoxel_label).second; ++adjacent_itr)
		{
			pcl::Supervoxel<PointT>::Ptr neighbor_supervoxel = supervoxel_clusters.at (adjacent_itr->second);
			adjacent_supervoxel_centers.push_back (neighbor_supervoxel->centroid_);
		}
		//
		std::stringstream ss;
		ss << "supervoxel_" << supervoxel_label;
		//cout<<ss.str()<<endl;
		//���Ƹó������������ڳ�������ͼ
		addSupervoxelConnectionsToViewer (supervoxel->centroid_, adjacent_supervoxel_centers, ss.str (), viewer);
		//ʹ������ָ����һ����ǩ��
		label_itr = supervoxel_adjacency.upper_bound (supervoxel_label);
	}

	while (!viewer->wasStopped ())
	{
		viewer->spinOnce();
	}
	return (0);
}

void
	addSupervoxelConnectionsToViewer (PointT &supervoxel_center,
	PointCloudT &adjacent_supervoxel_centers,
	std::string supervoxel_name,
	boost::shared_ptr<pcl::visualization::PCLVisualizer> & viewer)
{

	int i=0;
	//Iterate through all adjacent points, and add a center point to adjacent point pair
	PointCloudT::iterator adjacent_itr = adjacent_supervoxel_centers.begin ();
	for ( ; adjacent_itr != adjacent_supervoxel_centers.end (); ++adjacent_itr)
	{
		std::stringstream ss;
		ss<<supervoxel_name<<i;
		viewer->addLine(supervoxel_center,*adjacent_itr,ss.str());

		viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_LINE_WIDTH,3,ss.str());
		viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR,0,255,0,ss.str());
		ss<<supervoxel_name<<i;
		viewer->addSphere(supervoxel_center,0.008,0,0,255,ss.str());
		viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_SHADING,pcl::visualization::PCL_VISUALIZER_SHADING_GOURAUD,ss.str());
		//viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY,0.9,ss.str());
		i++;
	}

}


