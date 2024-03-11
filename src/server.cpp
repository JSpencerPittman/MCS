#include <iostream>
#include <stream_protocol.h>
#include "udp.h"

#define IP_ADDR "127.0.0.1"
#define PORT 8000

using namespace std;

int main() {
	cout << "Starting Server..." << endl;

	udp::Socket sock(IP_ADDR, PORT, true);

	std::cout << "START " << udp::StartSocket(sock) << std::endl;

	std::vector<unsigned char> buffer;
	udp::Receive(sock, buffer, 65000);
	std::cout << "RECEIVED: " << buffer.size() << std::endl;
	std::string sMessage(buffer.begin(), buffer.end());
	std::cout << sMessage << std::endl;

	udp::CloseSocket(sock);
}

/*int main() {
	cout << "Starting Client..." << endl;

	udp::Socket sock(IP_ADDR, PORT, true);

	std::cout << "START " << udp::StartSocket(sock) << std::endl;

	std::vector<unsigned char> buffer;
	streamprotocol::ImageReconstructor reconstruct;
	streamprotocol::ImagePacket packet;
	while (true) {
		udp::Receive(sock, buffer, 65000);
		streamprotocol::DecodeImagePacket(buffer, packet);
		reconstruct.Submit(packet);
		std::cout << "RECEIVED: " << buffer.size() << std::endl;
		std::cout << "READY: " << reconstruct.Ready() << std::endl;
		if(reconstruct.Ready()) break;
	}

	cv::Mat frame;
	bool result = reconstruct.Reconstruct(frame);
	std::cout << "RESULT: " << result << std::endl;

	cv::imwrite("output.jpg", frame);

	udp::CloseSocket(sock);
}*/