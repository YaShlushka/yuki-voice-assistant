#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class Worker {
 public:
	Worker() {
		thread_ = std::thread([this]() {
			while (true) {
				std::function<void()> task;

				{
					std::unique_lock<std::mutex> lock(mtx_);
					cv_.wait(lock, [this] { return stop_ || !queue_.empty(); });
					if (stop_ && queue_.empty()) {
						return;
					}

					task = std::move(queue_.front());
					queue_.pop();
				}

				task();
			}
		});
	}

	~Worker() {
		{
			std::lock_guard<std::mutex> lock(mtx_);
			stop_ = true;
		}

		cv_.notify_one();
		if (thread_.joinable()) {
			thread_.join();
		}
	}

	void AddTask(std::function<void()> func) {
		std::lock_guard<std::mutex> lock(mtx_);
		queue_.push(std::move(func));
		cv_.notify_one();
	}

 private:
	std::queue<std::function<void()>> queue_;
	std::mutex mtx_;
	std::condition_variable cv_;
	std::thread thread_;
	bool stop_ = false;
};
