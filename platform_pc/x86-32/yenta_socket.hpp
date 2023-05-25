/*
 * yenta_socket.hpp
 *
 *  Created on: 2 sty 2023
 *      Author: nyuu
 */

#ifndef PLATFORM_PC_X86_32_YENTA_SOCKET_HPP_
#define PLATFORM_PC_X86_32_YENTA_SOCKET_HPP_

#include "pci.hpp"

class YentaSocket {
private:
	Pci::Pci_driver drv;
	Memory_region base_addr;
	void isr();
public:
	YentaSocket();
};





#endif /* PLATFORM_PC_X86_32_YENTA_SOCKET_HPP_ */
