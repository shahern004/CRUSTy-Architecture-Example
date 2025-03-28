#ifndef COMMANDMESSAGE_H
#define COMMANDMESSAGE_H

struct CommandMessage {
    int32_t command_id;
    uint8_t data[64];
};

#endif
