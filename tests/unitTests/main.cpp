#include <iostream>
#include <exception>
#include "PatternSearcher.test.hpp"
#include "ThreadManager.test.hpp"

using namespace tests;

struct TestCase {
    const char* name;
    void (*func)();
};

int main() {
    TestCase tests[] = {
        {"processesSingleFileSuccessfully", processesSingleFileSuccessfully},
        {"processesDirectoryWithMultipleFiles", processesDirectoryWithMultipleFiles},
        {"handlesEmptyDirectoryGracefully", handlesEmptyDirectoryGracefully},
        {"skipsNonTextFiles", skipsNonTextFiles},
        {"handlesFileAccessError", handlesFileAccessError},
        {"processesMultipleFilesList", processesMultipleFilesList},
        {"handlesEmptyFileGracefully", handlesEmptyFileGracefully},
        {"ignoresUnreadableFile", ignoresUnreadableFile},
        {"processesNestedDirectories", processesNestedDirectories},
        {"findsMultipleMatchesInLine", findsMultipleMatchesInLine},
        {"handlesLargeFileEfficiently", handlesLargeFileEfficiently},
        {"handlesBinaryFileGracefully", handlesBinaryFileGracefully},
        {"handlesRegexWithSpecialCharacters", handlesRegexWithSpecialCharacters},
        {"returnsNoMatchWhenPatternNotFound", returnsNoMatchWhenPatternNotFound},
        {"matchesPatternAtLineStartAndEnd", matchesPatternAtLineStartAndEnd},
        {"handlesEmptyPatternGracefully", handlesEmptyPatternGracefully},
        {"matchesPatternWithUnicodeCharacters", matchesPatternWithUnicodeCharacters},
        {"matchesPatternWithCaseInsensitiveRegex", matchesPatternWithCaseInsensitiveRegex}
    };

    int passed = 0;
    int failed = 0;

    for (const auto& test : tests) {
        try {
            test.func();
            std::cout << "[PASS] " << test.name << std::endl;
            ++passed;
        } catch (const std::exception& e) {
            std::cout << "[FAIL] " << test.name << " (exception: " << e.what() << ")" << std::endl;
            ++failed;
        } catch (...) {
            std::cout << "[FAIL] " << test.name << " (unknown exception)" << std::endl;
            ++failed;
        }
    }

    std::cout << "\nTotal: " << (passed + failed) << ", Passed: " << passed << ", Failed: " << failed << std::endl;
    return failed == 0 ? 0 : 1;
}
