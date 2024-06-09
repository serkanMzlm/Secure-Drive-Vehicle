#include "obstacle_avoidance.hpp"

void ObstacleAvoidance::updateVelocity(double &linear_x, double &linear_w)
{
    float error = 0.0f;
    for (int phi = 0; phi < HORIZONTAL; phi++)
    {
        int angle = phi % 15;
        if(histogram[phi] > 3.0f)
        {
            // std::cout << -1 << std::endl;
            continue;
        }
        if ((histogram[angle] < 0.3f) || (histogram[359 - angle] < 0.3f))
        {
            // std::cout << "vehicle safety" << angle << " " << histogram[angle] << histogram[angle]<< std::endl;
            linear_x = 0.0;
            int left_force = 0.0f;
            int right_force = 0.0f;
            for (int i = 60; i < 110; i++)
            {
                left_force += histogram[i];
                right_force += histogram[360 - i];
            }
            linear_w = left_force >= right_force ? 0.5 : -0.5;
            linear_x = 0.0;
        }
        if (histogram[phi] <= calculateDistance(vehicle_radius, phi))
        {
            continue;
        }
        if (linear_x * cosf(DEG2RAD * phi) > 0)
        {
            std::cout << error << std::endl;
            error += avoidanceDistance(histogram[phi], phi);
        }
    }
    
    // std::cout << " ERROR: " << error << "\n_______________________" << std::endl;
    if (abs(linear_x) > OFFSET && abs(error) > OFFSET)
    {
        linear_w = error;
    }

    last_point[LINEAR_V].x = linear_x;

    first_point[ANGULAR_V].x = last_point[LINEAR_V].x;
    last_point[ANGULAR_V].x = last_point[LINEAR_V].x;
    last_point[ANGULAR_V].y = linear_w;

    last_point[RESULT_V].x = last_point[LINEAR_V].x;
    last_point[RESULT_V].y = last_point[ANGULAR_V].y;
}

float ObstacleAvoidance::calculateDistance(float distance, int angle)
{
    return distance /* * (abs(cos(DEG2RAD * angle)) + abs(sin(DEG2RAD * angle))) */;
}

float ObstacleAvoidance::avoidanceDistance(float distance, int angle)
{
    float kForce = -0.1f;
    return kForce * cosf(DEG2RAD * angle) * sinf(DEG2RAD * angle) / distance;
}

void ObstacleAvoidance::detectObject(pointXYZMsg &cloud_data)
{
    pointIndicesMsg cluster_indices;
    pointXYZMsg::Ptr cloud_m(new pointXYZMsg);
    pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
    pcl::EuclideanClusterExtraction<pcl::PointXYZ> ec;
    *cloud_m = cloud_data;
    ec.setClusterTolerance(vehicle_dimensions[WIDTH]);
    tree->setInputCloud(cloud_m);
    ec.setMinClusterSize(2);
    ec.setMaxClusterSize(1000);
    ec.setSearchMethod(tree);
    ec.setInputCloud(cloud_m);
    ec.extract(cluster_indices);
    getClusterPoint(cluster_indices, cloud_data);
}

void ObstacleAvoidance::getClusterPoint(pointIndicesMsg &indices_c, pointXYZMsg &cloud_c)
{
    clearHistogram();
    float cartesian[] = {0, 0, 0};
    for (pointIndicesMsg::const_iterator it = indices_c.begin(); it != indices_c.end(); ++it)
    {
        for (std::vector<int>::const_iterator pit = it->indices.begin(); pit != it->indices.end(); ++pit)
        {
            cartesian[X] = cloud_c.points[*pit].x;
            cartesian[Y] = cloud_c.points[*pit].y;
            cartesian[Z] = cloud_c.points[*pit].z;
            polarObstacleDensity(cartesian);
        }
    }
}

void ObstacleAvoidance::polarObstacleDensity(float *cc_data)
{
    Coordinate_t spherical;
    float x = powf(cc_data[X], 2);
    float y = powf(cc_data[Y], 2);
    float z = powf(cc_data[Z], 2);
    float distance = sqrtf(x + y + z);
    cartesian2Spherical(cc_data, spherical.pos); // PHI - THETA - RADIUS

    maskPolarHistogram(spherical, distance);
}

void ObstacleAvoidance::maskPolarHistogram(Coordinate_t spherical, float distance)
{
    if ((distance <= lidar_rules[MIN_DIS]) || (distance >= lidar_rules[MAX_DIS]))
    {
        return;
    }
    std::cout << spherical.pos[PHI] << std::endl;
    if (spherical.pos[THETA] >= MAX_PHI_ANGLE || spherical.pos[THETA] <= MIN_PHI_ANGLE)
    {
        return;
    }
    histogram[spherical.pos[PHI]] = spherical.pos[RADIUS];
}

void ObstacleAvoidance::clearHistogram()
{
    histogram.fill(4.0f);
}