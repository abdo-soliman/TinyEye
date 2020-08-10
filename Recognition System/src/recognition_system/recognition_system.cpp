#include "recognition_system.h"

#include <unistd.h>
#include <iostream>

#include "utils.h"
#include "logger/logger.h"

namespace tinyeye
{
	RecognitionSystem RecognitionSystem::recognition_system;

	RecognitionSystem::RecognitionSystem()
	{
	}

	void RecognitionSystem::intialize(std::string mtcnn_models_dir, std::string mfn_model_path,
		int in_features, int out_features, std::string classifier_model_path,
		std::string classifier_map_path, std::string camera_ip, std::string temp_dir_path, tinyeye::socket *sio)
	{
		recognition_system.sio_server = sio;
		recognition_system.updated_classifier = false;
		recognition_system.in_features = in_features;
		recognition_system.out_features = out_features;
		recognition_system.classifier_model_path = classifier_model_path;
		recognition_system.classifier_map_path = classifier_map_path;

		try
		{
			recognition_system.detector.reset();
			recognition_system.detector = std::make_unique<mtcnn::MTCNN>(mtcnn_models_dir);
		}
		catch (const std::exception &e)
		{
			std::string error_msg = utils::replace_all(e.what(), "\n", " ");
			error_msg = utils::replace_all(e.what(), "\0", " ");
			error_msg = utils::replace_all(error_msg, "\"");
			logger::LOG_ERROR(FAILED_TO_INTIALIZE_MTCNN, error_msg);
			exit(-1);
		}

		try
		{
			recognition_system.mfn_net.reset();
			recognition_system.mfn_net = std::make_unique<mobilefacenet::mfn>(mfn_model_path);
		}
		catch (const std::exception &e)
		{
			std::string error_msg = utils::replace_all(e.what(), "\n", " ");
			error_msg = utils::replace_all(e.what(), "\0", " ");
			error_msg = utils::replace_all(error_msg, "\"");
			logger::LOG_ERROR(FAILED_TO_INTIALIZE_MOBILE_FACENET, error_msg);
			exit(-1);
		}

		sio->send_get_model_signal();
		while (!recognition_system.updated_classifier)
		{
			sleep(1);
			recognition_system.updated_classifier = recognition_system.sio_server->get_updated_model();
		}
		std::cout << "Start processing" << std::endl;

		recognition_system.load_classifier(in_features, out_features, classifier_model_path, classifier_map_path);

		recognition_system.ip = camera_ip;
		recognition_system.temp_dir = temp_dir_path;
		recognition_system.camera_temp_dir = temp_dir_path + "/camera";
		recognition_system.recognition_temp_dir = temp_dir_path + "/recognition";

		std::string command = "mkdir -p " + recognition_system.camera_temp_dir;
		system(command.c_str());
		if (!utils::is_dir(recognition_system.camera_temp_dir))
		{
			std::string msg = "can't create camera temp directory at: " + recognition_system.camera_temp_dir + " please make sure correct path and privded and write permissions are set";
			logger::LOG_ERROR(FAILED_TO_CREATE_TEMP_DIR, msg);
			throw std::runtime_error(msg);
		}

		command = "mkdir -p " + recognition_system.recognition_temp_dir;
		system(command.c_str());
		if (!utils::is_dir(recognition_system.recognition_temp_dir))
		{
			std::string msg = "can't create camera temp directory at: " + recognition_system.recognition_temp_dir + " please make sure correct path and privded and write permissions are set";
			logger::LOG_ERROR(FAILED_TO_CREATE_TEMP_DIR, msg);
			throw std::runtime_error(msg);
		}
	}

	void RecognitionSystem::load_classifier(int in_features, int out_features, std::string classifier_model_path,
		std::string classifier_map_path)
	{
		try
		{
			recognition_system.classifier.reset();
			recognition_system.classifier = std::make_unique<ArcFace>();
			recognition_system.classifier->construct_map(classifier_map_path);
			recognition_system.classifier->load(classifier_model_path);
		}
		catch (const std::exception &e)
		{
			std::string error_msg = utils::replace_all(e.what(), "\n", " ");
			error_msg = utils::replace_all(e.what(), "\0", " ");
			error_msg = utils::replace_all(error_msg, "\"");
			logger::LOG_ERROR(FAILED_TO_INTIALIZE_CLASSIFIER, error_msg);
			exit(-1);
		}
	}

	RecognitionSystem &RecognitionSystem::get()
	{
		return recognition_system;
	}

	void RecognitionSystem::set_frame_rate(int fr)
	{
		get().frame_rate = fr;
	}

	void RecognitionSystem::set_max_imgs_per_temp(int max_per_temp)
	{
		get().max_imgs_per_temp = max_per_temp;
	}

	void RecognitionSystem::recognition_loop()
	{
		get()._recognition_loop();
	}

	void RecognitionSystem::detection_loop()
	{
		get()._detection_loop();
	}

	void RecognitionSystem::camera_loop()
	{
		get()._camera_loop();
	}

	void RecognitionSystem::_recognition_loop()
	{
		cv::Mat ref_frame;

		bool empty_queue = true;
		std::string name;
		std::string command;
		std::string dir_name = "";
		std::vector<std::string> img_paths;
		std::vector<cv::Mat> imgs;
		while (true)
		{
			{
				const std::lock_guard<std::mutex> lock(processing_dirs_mutex);
				empty_queue = processing_dirs.empty();
			}

			if (empty_queue)
				usleep(200);
			else
			{
				{
					const std::lock_guard<std::mutex> lock(processing_dirs_mutex);
					dir_name = processing_dirs.front();
					processing_dirs.pop();
				}

				if (dir_name == "")
					continue;

				img_paths = utils::list_dir(dir_name);
				for (const auto &img_path : img_paths)
				{
					if (img_path.substr(img_path.size() - 7) == "ref.jpg")
						ref_frame = cv::imread(img_path);
					else
						imgs.push_back(cv::imread(img_path));
				}

				clock_t start = clock();
				name = classifier->predict_block(mfn_net->get_embeddings(imgs));
				std::cout << "[PREDICTION] recognition_time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
				std::cout << "[PREDICTION] " << name << std::endl;

				std::string base64_ref = utils::img2base64(ref_frame);
				if (name == "unknown")
				{
					sio_server->send_log(name, base64_ref, name);
					logger::LOG_INFO(DETECTED_UNKNOWN, "unknown personal detected", base64_ref);
				}
				else
				{
					sio_server->send_log(name, base64_ref, "known");
					logger::LOG_INFO(DETECTED_KNOWN, name + " was detected", base64_ref);
				}

				imgs.clear();
				command = "rm -rf " + dir_name;
				system(command.c_str());
			}
		}
	}

	void RecognitionSystem::_detection_loop()
	{
		bool empty_queue;
		std::string dir_name, command;
		std::vector<std::string> img_paths;
		std::vector<cv::Mat> frames;

		cv::Mat frame, face;
		cv::Rect2d bbox;
		cv::Rect2d *bbox_ptr;

		std::list<cv::Rect2d> correct_boxes;

		while (true)
		{
			{
				const std::lock_guard<std::mutex> lock(unprocessed_dirs_mutex);
				empty_queue = unprocessed_dirs.empty();
			}

			if (empty_queue)
				usleep(1000 / frame_rate);
			else
			{
				{
					const std::lock_guard<std::mutex> lock(unprocessed_dirs_mutex);
					dir_name = unprocessed_dirs.front();
					unprocessed_dirs.pop();
				}

				if (dir_name == "")
					continue;

				img_paths = utils::list_dir(dir_name);
				for (const auto &img_path : img_paths)
					frames.push_back(cv::imread(img_path));

				for (size_t i = 0; i < frames.size(); i++)
				{
					if (i == 0)
					{
						clock_t start = clock();
						correct_boxes = detector->detect_rects(frames[i], 0.97);
						std::cout << "[PROCESSING] detection_time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;

						for (auto &tracker : trackers)
						{
							int overlaps = 0;
							for (auto &box : correct_boxes)
							{
								if (overlaping(box, trackers_current_box[tracker]))
								{
									bbox_ptr = &box;
									overlaps++;
								}
							}

							if (overlaps != 1)
							{
								tracker_done[tracker] = true;
								to_delete.push_back(tracker);
								std::cout << "[PROCESSING] done overlapping" << std::endl;
							}
							else
								correct_boxes.remove(*bbox_ptr);
						}

						for (auto &box : correct_boxes)
							allocate_tracker(frames[i], box);
					}
					else
						update_trackers(frames[i]);

					for (auto &tracker : trackers)
					{
						if (tracker_done[tracker] && !tracker_processed[tracker])
						{
							const std::lock_guard<std::mutex> lock(processing_dirs_mutex);
							processing_dirs.push(trackers_dirs_map[tracker].first);
							tracker_processed[tracker] = true;
						}
					}

					for (auto &tracker : to_delete)
					{
						trackers_current_box.erase(tracker);
						trackers_dirs_map.erase(tracker);
						tracker_done.erase(tracker);
						tracker_processed.erase(tracker);
						trackers.remove(tracker);
						std::cout << "[PROCESSING] deleting useless trackers" << std::endl;
					}
					to_delete.clear();
				}

				frames.clear();
				command = "rm -rf " + dir_name;
				system(command.c_str());
			}
		}
	}

	void RecognitionSystem::_camera_loop()
	{
		// Read video
		cv::VideoCapture cap;
		std::string url = "http://" + ip + ":4747/video?x.mjpeg";

		cap.open(url);
		if (!cap.isOpened())
		{
			std::cout << "[CAMERA] can't read camera feed please make sure the ip is correct" << std::endl;
			return;
		}

		cv::Mat frame;

		std::string dir_name, command;
		int count = 0;
		while (true)
		{
			updated_classifier = sio_server->get_updated_model();
			while (!updated_classifier)
			{
				while (!processing_dirs.empty())
				{
					sleep(1);
				}
				updated_classifier = sio_server->get_updated_model();
				sleep(1);
				if (updated_classifier)
				{
					// reload the classifier model
					std::cout << "Continue processing ..." << std::endl;
					load_classifier(in_features, out_features, classifier_model_path, classifier_map_path);
				}
			}

			if (!cap.read(frame))
			{
				std::cout << "[CAMERA] frame droped!!!" << std::endl;
				continue;
			}

			// cv::rotate(frame, frame, cv::ROTATE_90_COUNTERCLOCKWISE);

			if (++count > frame_rate)
				count = 1;

			if (count == 1)
			{
				dir_name = camera_temp_dir + "/" + utils::random_string(camera_dir_name_length);
				while (utils::is_dir(dir_name))
					dir_name = camera_temp_dir + "/" + utils::random_string(camera_dir_name_length);

				command = "mkdir " + dir_name;
				system(command.c_str());
			}

			cv::imwrite(dir_name + "/" + std::to_string(count) + ".jpg", frame);

			if (count == frame_rate)
			{
				const std::lock_guard<std::mutex> lock(unprocessed_dirs_mutex);
				unprocessed_dirs.push(dir_name);
			}

			cv::waitKey(1000 / frame_rate);
		}
	}

	bool RecognitionSystem::overlaping(const cv::Rect2d &b1, const cv::Rect2d &b2)
	{
		cv::Point b1_corners[4] ={ b1.tl(), b1.br() };
		b1_corners[2] = cv::Point(b1.x + b1.width, b1.y);
		b1_corners[2] = cv::Point(b1.x, b1.y + b1.height);

		for (int i = 0; i < 4; i++)
		{
			if ((b1_corners[i].x >= b2.x) && (b1_corners[i].x <= (b2.x + b2.width)))
			{
				if ((b1_corners[i].y >= b2.y) && (b1_corners[i].y <= (b2.y + b2.height)))
					return true;
			}
		}

		cv::Point b1_center = cv::Point(b1.x + b1.width / 2, b1.y + b1.height / 2);
		cv::Point b2_center = cv::Point(b2.x + b2.width / 2, b2.y + b2.height / 2);
		return sqrt(pow(b2_center.y - b1_center.y, 2) + pow(b2_center.x - b1_center.x, 2)) < 30;
	}

	void RecognitionSystem::allocate_tracker(const cv::Mat &frame, const cv::Rect2d &box)
	{
		tracker_ptr tracker = cv::TrackerMedianFlow::create();
		tracker->init(frame, cv::Rect2d(box.x, box.y, box.width, box.height));
		trackers.push_back(tracker);
		trackers_current_box.insert(std::make_pair(tracker, cv::Rect2d(box.x, box.y, box.width, box.height)));
		tracker_done.insert(std::make_pair(tracker, false));
		tracker_processed.insert(std::make_pair(tracker, false));

		std::string dir_name = recognition_temp_dir + "/" + utils::random_string(recognition_dir_name_length);
		while (utils::is_dir(dir_name))
			dir_name = recognition_temp_dir + "/" + utils::random_string(recognition_dir_name_length);

		std::string command = "mkdir " + dir_name;
		system(command.c_str());

		auto face = cv::Mat(frame, cv::Rect(box.x, box.y, box.width, box.height));
		cv::imwrite(dir_name + "/ref.jpg", frame);
		cv::imwrite(dir_name + "/0.jpg", face);

		trackers_dirs_map.insert(std::make_pair(tracker, std::make_pair(dir_name, 1)));
	}

	void RecognitionSystem::update_trackers(const cv::Mat &frame)
	{
		int num_files;
		std::string dir_name;

		cv::Mat face;
		cv::Rect2d bbox;
		for (auto &tracker : trackers)
		{
			clock_t start = clock();
			if (tracker->update(frame, bbox))
			{
				std::cout << "[PROCESSING] tracking_time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
				trackers_current_box[tracker] = bbox;
				if (!tracker_done[tracker])
				{
					dir_name = trackers_dirs_map[tracker].first;
					num_files = trackers_dirs_map[tracker].second;

					face = cv::Mat(frame, cv::Rect(bbox.x, bbox.y, bbox.width, bbox.height));
					cv::imwrite(dir_name + "/" + std::to_string(num_files) + ".jpg", face);

					trackers_dirs_map[tracker] = std::make_pair(dir_name, ++num_files);

					if (num_files >= max_imgs_per_temp)
					{
						tracker_done[tracker] = true;
						std::cout << "[PROCESSING] done max imgs count" << std::endl;
					}
				}
			}
			else
			{
				tracker_done[tracker] = true;
				to_delete.push_back(tracker);
				std::cout << "[PROCESSING] done out of frame" << std::endl;
			}
		}
	}
} // namespace tinyeye
