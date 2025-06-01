#include <fstream>
#include <filesystem>
#include <sstream>
#include <cassert>
#include "../headers/ThreadManager.hpp"
#include "../headers/Config.hpp"
#include "../headers/utility.hpp"

namespace fs = std::filesystem;
using namespace mygrep;

namespace tests {
    void processesSingleFileSuccessfully() {
        std::ofstream file("test_single_file.txt");
        file << "foo bar baz\n";
        file.close();

        ThreadManager manager({fs::path("test_single_file.txt")});
        manager.run();

        fs::remove("test_single_file.txt");
    }

    void processesDirectoryWithMultipleFiles() {
        fs::create_directory("test_dir");
        std::ofstream file1("test_dir/file1.txt");
        file1 << "foo bar\n";
        file1.close();
        std::ofstream file2("test_dir/file2.txt");
        file2 << "baz qux\n";
        file2.close();

        ThreadManager manager({fs::path("test_dir")});
        manager.run();

        fs::remove("test_dir/file1.txt");
        fs::remove("test_dir/file2.txt");
        fs::remove("test_dir");
    }

    void handlesEmptyDirectoryGracefully() {
        fs::create_directory("empty_dir");

        ThreadManager manager({fs::path("empty_dir")});
        manager.run();

        fs::remove("empty_dir");
    }

    void skipsNonTextFiles() {
        std::ofstream file("test_binary_file.bin", std::ios::binary);
        char data[100] = {0};
        file.write(data, sizeof(data));
        file.close();

        ThreadManager manager({fs::path("test_binary_file.bin")});
        manager.run();

        fs::remove("test_binary_file.bin");
    }

    void handlesFileAccessError() {
        fs::path path("nonexistent_file.txt");

        ThreadManager manager({path});
        manager.run();
    }



    void processesMultipleFilesList() {
        std::ofstream file1("multi1.txt");
        file1 << "alpha\n";
        file1.close();
        std::ofstream file2("multi2.txt");
        file2 << "beta\n";
        file2.close();

        ThreadManager manager({fs::path("multi1.txt"), fs::path("multi2.txt")});
        manager.run();

        fs::remove("multi1.txt");
        fs::remove("multi2.txt");
    }

    void handlesEmptyFileGracefully() {
        std::ofstream file("empty.txt");
        file.close();

        ThreadManager manager({fs::path("empty.txt")});
        manager.run();

        fs::remove("empty.txt");
    }

    void ignoresUnreadableFile() {
        std::ofstream file("unreadable.txt");
        file << "should not be read\n";
        file.close();
        fs::permissions("unreadable.txt", fs::perms::none);

        ThreadManager manager({fs::path("unreadable.txt")});
        manager.run();

        fs::permissions("unreadable.txt", fs::perms::owner_all);
        fs::remove("unreadable.txt");
    }

    void processesNestedDirectories() {
        fs::create_directory("outer");
        fs::create_directory("outer/inner");
        std::ofstream file1("outer/outerfile.txt");
        file1 << "outer\n";
        file1.close();
        std::ofstream file2("outer/inner/innerfile.txt");
        file2 << "inner\n";
        file2.close();

        ThreadManager manager({fs::path("outer")});
        manager.run();

        fs::remove("outer/outerfile.txt");
        fs::remove("outer/inner/innerfile.txt");
        fs::remove("outer/inner");
        fs::remove("outer");
    }
}