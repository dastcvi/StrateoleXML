/*
 * XMLReader_v5.h
 * Author:  Alex St. Clair
 * Created: August 2019
 *
 * This file declares a class to handle reading and parsing Strateole 2 XML
 * messages from the Zephyr gondola's On-Board Computer.
 *
 * The reader uses the serial buffers internal to the Arduino software core,
 * and this reader is designed for use on the Teensy 3.6, for which it is easy
 * and necessary to modify the core to increase the size of these buffers.
 */

#ifndef XMLREADER_H
#define XMLREADER_H

#include "Arduino.h"
#include <TimeLib.h>
#include <stdint.h>

// Message Types
#define MSG_IM      "IM"
#define MSG_SAck    "SAck"
#define MSG_SW      "SW"
#define MSG_RAAck   "RAAck"
#define MSG_TMAck   "TMAck"
#define MSG_TC      "TC"
#define MSG_GPS     "GPS"

// used to index id string array
enum Instrument_t {
    FLOATS = 0,
    RACHUTS = 1,
    LPC = 2
};

enum ZephyrMessage_t {
    // Main HW
    IM,    // Instrument Mode
    SAck,  // Safety Acknowledge
    SW,    // Shutdown Warning
    RAAck, // Rachuts Acknowledge
    TMAck, // Telemetry Acknowledge
    TC,    // Telecommand
    GPS,   // GPS Data
    NO_MESSAGE,
    UNKNOWN
};

class XMLReader {
public:
    // constructors/destructors
    XMLReader(Stream * rxstream, Instrument_t inst);
    ~XMLReader() { };

    // public interface function
    bool GetNewMessage();

    // message results
    ZephyrMessage_t zephyr_message = NO_MESSAGE;
    uint16_t message_id = 0;
    uint16_t binary_length = 0;

private:
    // parsing functions
    bool ParseMessage();
    bool ParseGPSMessage();

    // get the next character from the stream and update the CRC
    bool ReadNextChar(char * new_char);

    // read different parts of the message into buffers
    bool MessageTypeOpen(uint32_t timeout);
    bool MessageTypeClose(uint32_t timeout);
    bool ReadField(uint32_t timeout);
    bool ReadVerifyCRC(uint32_t timeout);
    bool ReadBinarySection(uint32_t timeout);

    // generic helpers
    bool ReadSpecificChar(uint32_t timeout, char specific_char);
    bool ReadOpeningTag(uint32_t timeout, char * buffer, uint8_t buff_size);
    bool ReadClosingTag(uint32_t timeout, char * buffer, uint8_t buff_size);

    // after every message or error
    void ResetReader();

    // serial port for Strateole on-board computer
    Stream * rx_stream;

    // Zephyr-specified instrument id string
    char inst_ids[3][8] = {"FLOATS", "RACHUTS", "LPC"};
    Instrument_t instrument;

    // CRC-CCITT16 internals
    const uint16_t crc_poly = 0x1021;
    uint16_t working_crc = 0;
    uint16_t crc_result = 0;

    // internal buffers for message parts
    char message_buff[8] = {0};
    char id_buff[8] = {0};
    char fields[8][8] = {{0}};
    char field_values[8][16] = {{0}};
    uint8_t num_fields = 0;

};

#endif /* XMLREADER_H */