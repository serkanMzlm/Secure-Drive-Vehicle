#!/usr/bin/python3

import os
import cv2

import rclpy
from rclpy.node import Node
from cv_bridge import CvBridge
from sensor_msgs.msg import Image

from ament_index_python.packages import get_package_share_directory

parameters_path =  get_package_share_directory('parameters')


class Recorder(Node):
    def __init__(self):
        super().__init__("recorder_node")
        self.camera_sub = self.create_subscription(Image, "camera", self.cameraCallback, 10)
        video_path = os.path.join(parameters_path + "/video", "front_camera.avi")
        self.out_video = cv2.VideoWriter(video_path, cv2.VideoWriter_fourcc('M','J','P','G'),
                                                                                15, (360, 360))
        self.bridge = CvBridge()
    
    def cameraCallback(self, img):
        frame = self.bridge.imgmsg_to_cv2(img, 'bgr8')
        self.out_video.write(frame)
        cv2.imshow("Front Camera", frame)
        cv2.waitKey(1)
   
def main(args = None):
  rclpy.init(args=args)
  image_sub = Recorder()
  rclpy.spin(image_sub)    
  rclpy.shutdown()

if __name__ == '__main__':
  main()