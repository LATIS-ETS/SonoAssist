import time

import cv2
import imutils
import numpy as np
import pyrealsense2 as rs

from .video import VideoManager
from .config import ConfigurationManager


class SonoTracker:

    ''' 
    Enables the tracking of a custom marker in 3D space 
    The tracker uses the (CSRT) tracker from opencv
    
    User interface : 
        - "s" key : select target object, via bounding rectangle
        - "space bar" : next frame (when detection fails)
    '''

    display_delay = 0.1

    # defining color filter parameters
    color_filter_margin = 10
    filter_lower_red = np.array([150,50,50]) 
    filter_upper_red = np.array([200,255,255])

    def __init__(self, config_path, video_file_path):

        '''
        Parameters
        ----------
        config_path (str) : path to configuration file
        video_file_path (str) : path to the video file to be loaded
        '''

        # loading configurations
        self.config_path = config_path
        self.config_manager = ConfigurationManager(config_path)
        self.debug = self.config_manager.config_data["debug_mode"]
        
        self.tracker = cv2.TrackerCSRT_create()
        self.video_manager = VideoManager(config_path, video_file_path)


    def launch_tracking(self):

        ''' Launches object tracking on the frames of the video source '''

        object_positions = []

        frame_count = 0
        completion_percentage = 0

        initial_selection = True
        detection_failed = False
        target_object_rect = None

        # checking video information
        if self.video_manager.video_source is None : return
        n_total_frames = self.video_manager.count_video_frames()

        while(True):

            try :

                # getting the next vide frame
                frame_count += 1
                (color_frame, depth_frame) = self.video_manager.get_vide_frame()
                if color_frame is None : break

                # selection of new target object is required
                if initial_selection or detection_failed :

                    # display is necessary for object selection
                    time.sleep(self.display_delay)
                    cv2.imshow("Video", color_frame)
                    input_key = cv2.waitKey(1) & 0xFF
                    
                    # handling the initial selection
                    if initial_selection and input_key == ord("s"):

                        target_object = cv2.selectROI("Video", color_frame, fromCenter=False, showCrosshair=True)    
                        if not target_object == (0,0,0,0):
                            self.tracker.init(color_frame, target_object)
                            initial_selection = False
                            cv2.destroyAllWindows()

                    # handling failed detections
                    elif detection_failed:

                        target_object = cv2.selectROI("Video", color_frame, fromCenter=False, showCrosshair=True)
                        if not target_object == (0,0,0,0):
                            self.tracker = cv2.TrackerCSRT_create()
                            self.tracker.init(color_frame, target_object)
                            detection_failed = False
                            cv2.destroyAllWindows()

                # performing the tracking on the current frame
                else :

                    (success, bounding_box) = self.tracker.update(color_frame)
                    detection_failed = not success
                    
                    if success :
                        
                        (x, y, w, h) = [int(v) for v in bounding_box]
                        # acquire depth info here ...
                        
                        # updating the display (in debug mode)
                        if self.debug : 
                            
                            color_frame = cv2.rectangle(color_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                            time.sleep(self.display_delay)
                            cv2.imshow("Video", color_frame)
                            cv2.waitKey(1)

                    # displaying processing progress       
                    if frame_count >= (completion_percentage * n_total_frames):
                        print("completion percentage : ", int(completion_percentage * 100), " %")
                        completion_percentage += 0.1

            # stop processing on failure
            except Exception as e:
                print(f"error occured during tracking : {e}")
                raise
        
        # clean up
        cv2.destroyAllWindows()

        return object_positions