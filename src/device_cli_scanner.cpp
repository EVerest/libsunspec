#include <iostream>
#include <memory>
#include <boost/program_options.hpp>

#include <sunspec/sunspec_device_mapping.hpp>
#include <sunspec/reader.hpp>

namespace po = boost::program_options;
namespace connection = everest::connection;
namespace modbus = everest::modbus;
namespace sunspec = everest::sunspec;

std::unique_ptr<connection::Connection> get_connection(const std::string& ip, const unsigned int& port, const std::string& transport_protocol) {
    if (transport_protocol == "tcp") {
        return std::make_unique<connection::TCPConnection>(ip, port);
    }
    return std::make_unique<connection::UDPConnection>(ip, port);
}

int main(int argc, char *argv[]) {
    po::options_description desc("Sunspec CLI device scanner");
    desc.add_options()("help,h", "Display help message.");
    desc.add_options()("ip", po::value<std::string>(), "IP address of device to be scanned.");
    desc.add_options()("port,p", po::value<unsigned int>()->default_value(502), "Port of device to be scanned. Default 502.");
    desc.add_options()("unit,u", po::value<unsigned int>(), "MODBUS unit of device to be scanned");
    desc.add_options()("transport,t", po::value<std::string>()->default_value("tcp"), "Transport layer protocol to be used. One of 'tcp' or 'udp'. Defaults to 'tcp'.");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    if (vm.count("ip") == 0) {
        std::cout << "Argument 'ip' is required!\n";
        return 1;
    }
    if (vm.count("unit") == 0) {
        std::cout << "Argument 'unit' is required!\n";
        return 1;
    }

    std::string ip_address = vm["ip"].as<std::string>();
    unsigned int port = vm["port"].as<unsigned int>();
    unsigned int unit = vm["unit"].as<unsigned int>();
    std::string transport_protocol = vm["transport"].as<std::string>();

    if (transport_protocol != "tcp" && transport_protocol != "udp") {
        std::cout << "Transport protocol must be 'tcp' or 'udp'\n";
        return 1;
    }

    std::cout << "Initializing reader with IP=" << ip_address << ", port=" << port << ", unit=" << unit <<" using MODBUS " << transport_protocol << ".\n";

    std::unique_ptr<connection::Connection> conn = get_connection(ip_address, port, transport_protocol);
    modbus::ModbusIPClient modbus_client(*conn);
    sunspec::SunspecDeviceMapping sunspec_device_mapping(modbus_client, unit);
    sunspec_device_mapping.scan();
    sunspec_device_mapping.print_summary();
    std::cout << sunspec_device_mapping.get_devices_information().dump(4) << "\n";

}
