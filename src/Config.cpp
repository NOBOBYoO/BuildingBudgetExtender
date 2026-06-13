#include "Config.hpp"

#include <fstream>

namespace BBE
{
	Config& Config::Get() noexcept
	{
		static Config config;
		return config;
	}

	void Config::LoadFromFile(const std::filesystem::path& a_path)
	{
		std::ifstream file{ a_path };
		if (!file.is_open()) {
			return;
		}

		std::string line;
		while (std::getline(file, line)) {
			const auto comment = line.find_first_of("#;");
			if (comment != std::string::npos) {
				line.erase(comment);
			}

			const auto eq = line.find('=');
			if (eq == std::string::npos) {
				continue;
			}

			auto key = line.substr(0, eq);
			auto value = line.substr(eq + 1);

			auto trim = [](std::string& s) {
				const auto start = s.find_first_not_of(" \t\r\n");
				if (start == std::string::npos) {
					s.clear();
					return;
				}
				const auto end = s.find_last_not_of(" \t\r\n");
				s = s.substr(start, end - start + 1);
			};

			trim(key);
			trim(value);
			if (key.empty() || value.empty()) {
				continue;
			}

			try {
				if (key == "BudgetIncreaseFactor") {
					budgetIncreaseFactor = std::stof(value);
				} else if (key == "DefaultMaxDrawsFallback") {
					defaultMaxDrawsFallback = std::stof(value);
				} else if (key == "DefaultMaxTrianglesFallback") {
					defaultMaxTrianglesFallback = std::stof(value);
				} else if (key == "ExtendThreshold") {
					extendThreshold = std::stof(value);
				} else if (key == "ShowNotification") {
					showNotification = (value == "1" || value == "true" || value == "True" || value == "yes");
				} else if (key == "VerboseLogging") {
					verboseLogging = (value == "1" || value == "true" || value == "True" || value == "yes");
				}
			} catch (...) {
				// Ignore malformed values and keep defaults.
			}
		}
	}
}
