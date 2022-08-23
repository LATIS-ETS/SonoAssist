#ifndef SONOASSIST_H
#define SONOASSIST_H

#include "ui_SonoAssist.h"

#include <tuple>
#include <memory>
#include <vector>
#include <windows.h>
#include <Lmcons.h>

#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QtXML/QDomDocument>
#include <QGraphicsPixmapItem>
#include <QtWidgets/QMainWindow>

#include "GazeTracker.h"
#include "OSKeyDetector.h"
#include "ScreenRecorder.h"
#include "RGBDCameraClient.h"
#include "ClariusProbeClient.h"
#include "MetaWearBluetoothClient.h"

#include "SensorDevice.h"
#include "process_management.h"

#define RED_TEXT "#cc0000"
#define GREEN_TEXT "#008000"
#define IMG_PLACE_HOLDER_COLOR "#000000"
#define ACTIVE_SENSOR_FIELD_COLOR "#D3D3D3"
#define INACTIVE_SENSOR_FIELD_COLOR "#ffffff"

// US probe display (normal)
#define US_DISPLAY_DEFAULT_WIDTH 1260
#define US_DISPLAY_DEFAULT_HEIGHT 720

// US probe display (preview)
#define PREVIEW_US_DISPLAY_WIDTH 640
#define PREVIEW_US_DISPLAY_HEIGHT 360
#define PREVIEW_US_DISPLAY_X_OFFSET 650
#define PREVIEW_US_DISPLAY_Y_OFFSET 0

// RGB D camera display (preview)
#define CAMERA_DISPLAY_WIDTH 640
#define CAMERA_DISPLAY_HEIGHT 360
#define CAMERA_DISPLAY_X_OFFSET 0
#define CAMERA_DISPLAY_Y_OFFSET 0

// Eyetracker crosshairs dimensions
#define EYETRACKER_N_ACC_TARGETS 4
#define EYETRACKER_ACC_TARGET_WIDTH 20
#define EYETRACKER_ACC_TARGET_HEIGHT 20
#define EYETRACKER_CROSSHAIRS_WIDTH 50
#define EYETRACKER_CROSSHAIRS_HEIGHT 50

// defining default config file path
#define DEFAULT_LOG_PATH "C:/Users/<username>/AppData/SonoAssist/"
#define DEFAULT_CONFIG_PATH "C:/Program Files (x86)/SonoAssist/resources/params.xml"

typedef std::map<std::string, std::string> config_map;

class ClariusProbeClient;

class SonoAssist : public QMainWindow {
	
	Q_OBJECT

	public:

		SonoAssist(QWidget *parent = Q_NULLPTR);
		~SonoAssist();

	public slots:
		
		// device connection
		void on_sensor_connect_button_clicked(void);
		void set_device_status(int device_id, bool device_status);

		// data streaming
		void on_pass_through_box_clicked(void);
		void on_eye_t_targets_box_clicked(void);
		void on_acquisition_preview_box_clicked(void);
		void on_start_acquisition_button_clicked(void);
		void on_stop_acquisition_button_clicked(void);

		// data acquisition slots
		void on_new_camera_image(QImage);
		void on_new_clarius_image(QImage);
		void on_clarius_no_imu_data(void);
		void on_new_gaze_point(float, float);
		void on_new_us_screen_capture(QImage);
		void on_new_os_key_detected(int);

		// loading file slots 
		void on_param_file_browse_clicked(void);
		void on_output_folder_browse_clicked(void);
		void on_param_file_apply_clicked();

		// other slots
		void on_udp_port_input_editingFinished(void);
		void sensor_panel_selection_handler(int row, int column);

	private:

		Ui::MainWindow ui;

		// main display vars
		std::unique_ptr<QGraphicsScene> m_main_scene_p;
		int m_main_us_img_width = US_DISPLAY_DEFAULT_WIDTH;
		int m_main_us_img_height = US_DISPLAY_DEFAULT_HEIGHT;
		
		// US image display var (normal and preview)
		std::unique_ptr<QGraphicsPixmapItem> m_us_pixmap_p;
		// US normal placeholder display vars (normal)
		std::unique_ptr<QPixmap> m_us_bg_i_p;
		std::unique_ptr<QGraphicsPixmapItem> m_us_bg_p;
		// eye tracker placeholder display vars (preview)
		std::unique_ptr<QPixmap> m_eye_tracker_bg_i_p;
		std::unique_ptr<QGraphicsPixmapItem> m_eye_tracker_bg_p;
		std::unique_ptr<QGraphicsPixmapItem> m_eyetracker_crosshair_p;

		// RGB D camera display vars (preview)
		std::unique_ptr<QPixmap> m_camera_bg_i_p;
		std::unique_ptr<QGraphicsPixmapItem> m_camera_bg_p;
		std::unique_ptr<QGraphicsPixmapItem> m_camera_pixmap_p;

		// state check vars
		bool m_stream_is_active = false;
		bool m_preview_is_active = false;
		bool m_config_is_loaded = false;
		bool m_eye_tracker_targets = false;

		// config and output vars
		QJsonObject m_output_params;
		std::string m_output_folder_path = "";
		std::shared_ptr<config_map> m_app_params;

		// time marker handling vars
		QJsonArray m_time_markers_json;

		// sensor devices

		std::shared_ptr<RGBDCameraClient> m_camera_client_p;
		std::shared_ptr<GazeTracker> m_gaze_tracker_client_p;
		std::shared_ptr<OSKeyDetector> m_key_detector_client_p;
		std::shared_ptr<ClariusProbeClient> m_us_probe_client_p;
		std::shared_ptr<ScreenRecorder> m_screen_recorder_client_p;
		std::shared_ptr<MetaWearBluetoothClient> m_metawear_client_p;
		
		std::vector<int> m_sensor_conn_updates;
		std::vector<std::shared_ptr<SensorDevice>> m_sensor_devices;

		// redis process info
		PROCESS_INFORMATION m_redis_process;

		// graphical functions
		void add_debug_text(QString);
		void build_sensor_panel(void);
		void set_acquisition_label(bool active);
		void display_warning_message(QString title, QString message);

		// graphics scene functions
		void configure_normal_display(void);
		void clean_preview_display(void);
		void remove_preview_display(void);
		void generate_preview_display(void);
		void clean_normal_display(void);
		void remove_normal_display(void);
		void generate_normal_display(void);
		void generate_eye_tracker_targets(void);

		// utility functions
		bool check_devices_streaming(void);
		bool check_device_connections(void);
		void configure_device_clients(void);

		// param write/load functions
		void write_output_params(void);
		bool create_output_folder(void);
		std::string create_log_folder(void);
		bool load_config_file(QString param_file_path);

		// time marker funcions
		void clear_time_markers(void);
};

#endif