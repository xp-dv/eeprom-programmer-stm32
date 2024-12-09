# UART-Controlled AT28C16 EEPROM Programmer
This project is a UART-controlled AT28C16 EEPROM programmer based on the STM32 NUCLEO-F303K8 microcontroller.

## Team Members and Responsibilities
As a solo project, I was solely responsible for planning, designing, parts procurement, programming, prototyping, testing, and documenting.

## Command Set
The device is controlled via commands sent through UART via the NUCLEO-F303K8's micro-USB port. These commands are used to call the functions that read and write data to the EEPROM.

|Command|Function|
|:-|:-|
|a \<address\>                | Read memory from \<address\> |
|b \<address\> \<byte\>       | Write \<byte\> to \<address\> |
|r \<start\> \<end\>          | Read memory from address \<start\> to \<end\> |
|w \<start\> \<end\> \<data\> | Write memory from address \<start\> to \<end\> with \<data\> |
<!-- |l                            | Lock EEPROM (enable write protection) | -->
<!-- |u                            | Unlock EEPROM (disable write protection) | -->

> \< \> = Required

## Status Codes
Invalid packets will return one of the following status/error codes.
|Code|Description|
|:-|:-|
| FA | UART_RX_EMPTY |
| FB | UART_RX_INVALID_FORMAT |
| FC | UART_RX_INVALID_DATA |
| FD | UART_RX_INVALID_RANGE |
| FE | UART_RX_INVALID_ADDRESS |
| FF | UART_RX_INVALID_INSTRUCTION |
| 00 | UART_RX_VALID_PACKET |
| 01 | UART_RX_VALID_DATA |

## Packet Format
* The maximum packet size is 256 data bytes + 1 packet terminator byte.
* All packets must end with the line feed (LF) character (`'\n'` or 0x0A) which serves as the packet terminator.
* All bytes (command keywords, addresses, and data) sent over UART must be one of these ASCII characters: `'0'` to `'9'`, `'a'` to `'z'`, or `'A'` to `'Z'`.
* All numbers (addresses and data bytes) must be sent in ASCII-coded hex.
* Addresses (0x000-0x7FF) must be represented using three characters, and EEPROM data (0x00-0xFF) must be represented using two characters.
* Every separate piece of data (individual addresses and individual data bytes) must be separated by spaces (`' '` or 0x20).

## Standard Operating Procedure
1. Commands must be sent as a single ASCII character corresponding to one of the commands in [Command Set](#command-set), followed by the packet terminator. This will put the device in ADDRESS mode, where it will wait to receive a valid address packet.
2. There are two address packet types: _single-address_ for commands `a` and `b` and _multi-address_ for commands `r` and `w`. A single-address packet must consist of a single-address, followed by the packet terminator. A multi-address packet must consist of a start and end address, separated by a space, then followed by the packet terminator. A valid recieved address packet will then put the device in DATA mode where it will wait to receive a valid data packet.
3. A valid data packet can include any number of two-character ASCII-coded hex bytes, as long as they match the address range specified in the address packet, are separated by spaces, and the packet ends with the packet terminator.
4. Upon receipt of a valid data packet, the device will perform the requested operation.

## EEPROM Circuit
The AT28C16 EEPROM Programmer uses two serial shift registers for addressing and an 8-bit parallel data bus to enable full read and write of the AT28C16 EEPROM using only 12 pins. Thus, this design allows the use of the small-footprint, breadboard-compatible NUCLEO-F303K8 microcontroller to operate the device.

### Component List
* STM32 NUCLEO-F303K8 Microcontroller
* Micro-USB Cable (microcontroller to PC connection)
* 2 Shift Registers (SN74HC595N or equivalent)
* 1 Breadboard or Perfboard (830+ tie-points)
* Jumper Wire and/or Ribbon Cable (21-26 AWG)

## Block Diagram
Block diagram of the intercommunication between the various devices required to build the EEPROM programmer.
![Block Diagram](./figs/block_diagram.png)
