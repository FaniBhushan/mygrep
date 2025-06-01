#include <fstream>
    #include <filesystem>
    #include <cassert>
    #include <sstream>
    #include <algorithm>
    #include "../../headers/PatternSearcher.hpp"
    #include "../../headers/Config.hpp"
    #include "../../headers/utility.hpp"

    namespace fs = std::filesystem;
    using namespace mygrep;

namespace tests {

    void findsMultipleMatchesInLine() {
        std::ofstream file("test_multiple_matches.txt");
        file << "foo foo foo\n";
        file.close();

        config::SEARCH_PATTERN = "foo";
        config::CASE_SENSITIVE = true;
        config::USE_REGEX = false;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_multiple_matches.txt");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        assert(output.find("foo") != std::string::npos);
        assert(std::count(output.begin(), output.end(), '\n') == 3);

        fs::remove("test_multiple_matches.txt");
    }

    void handlesLargeFileEfficiently() {
        std::ofstream file("test_large_file.txt");
        for (int i = 0; i < 100000; ++i) {
            file << "This is line " << i << "\n";
        }
        file.close();

        config::SEARCH_PATTERN = "line 99999";
        config::CASE_SENSITIVE = true;
        config::USE_REGEX = false;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_large_file.txt");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        assert(output.find("line 99999") != std::string::npos);

        fs::remove("test_large_file.txt");
    }

    void handlesBinaryFileGracefully() {
        std::ofstream file("test_binary_file.bin", std::ios::binary);
        char data[100] = {0};
        file.write(data, sizeof(data));
        file.close();

        config::SEARCH_PATTERN = "foo";
        config::CASE_SENSITIVE = true;
        config::USE_REGEX = false;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_binary_file.bin");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        assert(output.empty());

        fs::remove("test_binary_file.bin");
    }

    void handlesRegexWithSpecialCharacters() {
        std::ofstream file("test_special_regex.txt");
        file << "foo.bar\nfoo?bar\nfoo+bar\n";
        file.close();

        config::SEARCH_PATTERN = R"(foo[\.\?\+])";
        config::CASE_SENSITIVE = true;
        config::USE_REGEX = true;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_special_regex.txt");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        assert(output.find("foo.bar") != std::string::npos);
        assert(output.find("foo?bar") != std::string::npos);
        assert(output.find("foo+bar") != std::string::npos);

        fs::remove("test_special_regex.txt");
    }

    void returnsNoMatchWhenPatternNotFound() {
        std::ofstream file("test_no_match.txt");
        file << "bar baz qux\n";
        file.close();

        config::SEARCH_PATTERN = "foo";
        config::CASE_SENSITIVE = true;
        config::USE_REGEX = false;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_no_match.txt");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        assert(output.empty());

        fs::remove("test_no_match.txt");
    }

    void matchesPatternAtLineStartAndEnd() {
        std::ofstream file("test_line_edges.txt");
        file << "foo at start\nends with foo\n";
        file.close();

        config::SEARCH_PATTERN = "foo";
        config::CASE_SENSITIVE = true;
        config::USE_REGEX = false;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_line_edges.txt");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        assert(output.find("foo at start") != std::string::npos);
        assert(output.find("ends with foo") != std::string::npos);

        fs::remove("test_line_edges.txt");
    }

    void handlesEmptyPatternGracefully() {
        std::ofstream file("test_empty_pattern.txt");
        file << "foo bar\n";
        file.close();

        config::SEARCH_PATTERN = "";
        config::CASE_SENSITIVE = true;
        config::USE_REGEX = false;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_empty_pattern.txt");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        // Depending on implementation, may match all lines or none; here we expect no output
        assert(output.empty());

        fs::remove("test_empty_pattern.txt");
    }

    void matchesPatternWithUnicodeCharacters() {
        std::ofstream file("test_unicode.txt");
        file << "föö bär\nföö again\n";
        file.close();

        config::SEARCH_PATTERN = "föö";
        config::CASE_SENSITIVE = true;
        config::USE_REGEX = false;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_unicode.txt");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        assert(output.find("föö") != std::string::npos);

        fs::remove("test_unicode.txt");
    }

    void matchesPatternWithCaseInsensitiveRegex() {
        std::ofstream file("test_case_insensitive_regex.txt");
        file << "FOO123 foo123 Foo123\n";
        file.close();

        config::SEARCH_PATTERN = R"(foo\d+)";
        config::CASE_SENSITIVE = false;
        config::USE_REGEX = true;

        PatternSearcher searcher;

        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        searcher.searchInFile("test_case_insensitive_regex.txt");

        std::cout.rdbuf(oldCout);
        std::string output = oss.str();
        assert(output.find("FOO123") != std::string::npos);
        assert(output.find("foo123") != std::string::npos);
        assert(output.find("Foo123") != std::string::npos);

        fs::remove("test_case_insensitive_regex.txt");
    }
}