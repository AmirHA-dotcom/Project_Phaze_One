//
// Created by Arian Sadeghi on 9/3/25.
//
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 5000));

    std::cout << "Waiting for connection...\n";
    tcp::socket socket(io);
    acceptor.accept(socket);
    std::cout << "Client connected\n";

    // دریافت طول فایل
    uint64_t file_size;
    boost::asio::read(socket, boost::asio::buffer(&file_size, sizeof(file_size)));

    // دریافت داده‌ها
    std::vector<char> buffer(file_size);
    boost::asio::read(socket, boost::asio::buffer(buffer.data(), file_size));

    // ذخیره در فایل
    std::ofstream out("received.json", std::ios::binary);
    out.write(buffer.data(), file_size);
    out.close();

    std::cout << "File received and saved as received.json\n";
    return 0;
}
