/**
 * @file ini_parse.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief This is an on-for ini ASCII Parser
 * @version 0.1
 * @date 2025-12-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace cxx_utils {
namespace ini_parser {

	/**
	 * @brief   Ini parser accept parse a
	 *          simple ini file, which contains
	 *          only plain kv pairs
	 *
	 */
	class IniParser {
	public:
		IniParser() = default;

		IniParser(const IniParser&) = delete;
		const IniParser& operator=(const IniParser&) = delete;

		/**
		 * @brief parse a view-ini string
		 *
		 * @param view
		 * @return true
		 * @return false
		 */
		bool parse(const std::string_view view);

		/**
		 * @brief get the value from parser map
		 *
		 * @param section
		 * @param key
		 * @return std::optional<std::string>
		 */
		std::optional<std::string>
		get(const std::string& section,
		    const std::string& key);

		/**
		 * @brief get the value from ini file
		 *
		 * @param section
		 * @param key
		 * @param default_value
		 * @return std::string
		 */
		std::string
		get(const std::string& section,
		    const std::string& key,
		    const std::string& default_value);

		/**
		 * @brief   check if the mappings owns value,
		 *          briefly recommend when need to know if owns value
		 *
		 * @param section
		 * @param key
		 * @return true
		 * @return false
		 */
		bool
		has(const std::string& section,
		    const std::string& key);

		using ini_section_t = std::unordered_map<std::string, std::string>;
		using ini_data_t = std::unordered_map<std::string, ini_section_t>;

		/**
		 * @brief get the datas directly
		 *
		 * @return const ini_data_t
		 */
		inline const ini_data_t data() const noexcept { return m_data; }

		/**
		 * @brief clear the datas, reset the ini parser
		 *
		 */
		void clear() noexcept { return m_data.clear(); }

	private:
		ini_data_t m_data; ///< data_stores

	private:
		bool consume_line(const std::string_view& line,
		                  std::string& current_section);
	};

}
}
