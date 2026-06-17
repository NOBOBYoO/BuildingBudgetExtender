#include "Config.hpp"

#include <fstream>

namespace BBE
{
	namespace
	{
		[[nodiscard]] bool ParseBool(const std::string& a_value)
		{
			return a_value == "1" || a_value == "true" || a_value == "True" || a_value == "yes";
		}

		void ApplyMcmKey(Config& a_config, const std::string& a_key, const std::string& a_value)
		{
			try {
				if (a_key == "fBudgetIncreaseFactor") {
					a_config.budgetIncreaseFactor = std::stof(a_value);
				} else if (a_key == "fExtendThreshold") {
					a_config.extendThreshold = std::stof(a_value);
				} else if (a_key == "fMaxBudgetMultiplier") {
					a_config.maxBudgetMultiplier = std::stof(a_value);
				} else if (a_key == "fDefaultMaxDrawsFallback") {
					a_config.defaultMaxDrawsFallback = std::stof(a_value);
				} else if (a_key == "fDefaultMaxTrianglesFallback") {
					a_config.defaultMaxTrianglesFallback = std::stof(a_value);
				} else if (a_key == "bShowExtendNotification") {
					a_config.showExtendNotification = ParseBool(a_value);
				} else if (a_key == "bShowCapNotification") {
					a_config.showCapNotification = ParseBool(a_value);
				} else if (a_key == "bVerboseLogging") {
					a_config.verboseLogging = ParseBool(a_value);
				}
			} catch (...) {
				// Ignore malformed values and keep the current setting.
			}
		}

		void LoadMcmIni(const std::filesystem::path& a_path, Config& a_config)
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

				auto trim = [](std::string& s) {
					const auto start = s.find_first_not_of(" \t\r\n");
					if (start == std::string::npos) {
						s.clear();
						return;
					}
					const auto end = s.find_last_not_of(" \t\r\n");
					s = s.substr(start, end - start + 1);
				};

				trim(line);
				if (line.empty()) {
					continue;
				}

				if (line.front() == '[' && line.back() == ']') {
					continue;
				}

				const auto eq = line.find('=');
				if (eq == std::string::npos) {
					continue;
				}

				auto key = line.substr(0, eq);
				auto value = line.substr(eq + 1);
				trim(key);
				trim(value);
				if (key.empty() || value.empty()) {
					continue;
				}

				ApplyMcmKey(a_config, key, value);
			}
		}
	}

	Config& Config::Get() noexcept
	{
		static Config config;
		return config;
	}

	void Config::LoadFromFile(const std::filesystem::path& a_path)
	{
		_configPath = a_path;

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
				} else if (key == "MaxBudgetMultiplier") {
					maxBudgetMultiplier = std::stof(value);
				} else if (key == "ShowNotification") {
					const auto enabled = ParseBool(value);
					showExtendNotification = enabled;
					showCapNotification = enabled;
				} else if (key == "ShowExtendNotification") {
					showExtendNotification = ParseBool(value);
				} else if (key == "ShowCapNotification") {
					showCapNotification = ParseBool(value);
				} else if (key == "VerboseLogging") {
					verboseLogging = ParseBool(value);
				}
			} catch (...) {
				// Ignore malformed values and keep defaults.
			}
		}
	}

	void Config::LoadFromMcmIfPresent()
	{
		const auto dataRoot =
			std::filesystem::path{ F4SE::ROOT_DIRECTORY_PATH } / F4SE::DATA_DIRECTORY_PATH / "MCM";

		const auto defaultsPath = dataRoot / "Config" / "BuildingBudgetExtender" / "settings.ini";
		const auto userPath = dataRoot / "Settings" / "BuildingBudgetExtender.ini";

		if (!std::filesystem::exists(defaultsPath) && !std::filesystem::exists(userPath)) {
			return;
		}

		LoadMcmIni(defaultsPath, *this);
		LoadMcmIni(userPath, *this);
	}

	void Config::Reload()
	{
		if (_configPath.empty()) {
			return;
		}

		LoadFromFile(_configPath);
		LoadFromMcmIfPresent();
	}
}
