#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include "asn1-header/SPATEM.h"
#include "asn1-header/per_encoder.h"
#include "asn1-header/per_decoder.h"

#define PORT 12345
#define MAX_MSG_SIZE 1024

#define STATION_UNIQUE_ID 0x12345454
#define BROADCAST_IP "192.168.1.255"

SPATEM_t spatem_example;
IntersectionReferenceID_t intersection_id = {0};

IntersectionState_t intersection_state_example = {0};
IntersectionState_t* array_of_intersection_state_pointers[5];

IntersectionStatusObject_t intersection_status_example = {0};
uint8_t inter_status_buf = IntersectionStatusObject_off;

MovementState_t movement_state_example = {0};
MovementState_t* array_of_movement_state_pointers[5];

MovementEvent_t movement_event_example = {0};
MovementEvent_t* array_of_movement_event_pointers[5];

DescriptiveName_t intersection_state_example_name = {0};

int main(int ac, char **av)
{
    asn_enc_rval_t ec; /*Encoder return value*/
    asn_dec_rval_t dc; /*Decoder return value*/
    uint16_t index = 0;
    int ret;
    MinuteOfTheYear_t spat_timestamp = 0x0000000000008734;
    SPATEM_t* decoded_spatem = &spatem_example;
    uint8_t out_buffer[1024] = {0};
    unsigned int bytes_enc = 0;
    struct sockaddr_in broadcast_addr;

    int sockfd = createSocket();
    configureBroadcast(&broadcast_addr);

    printf("Starting SPATEM encoding\n");

    printf("Initializing SPATEM structure...\n");
    /*SPATEM ITS PDU Header*/
    spatem_example.header.messageID = 3;
    spatem_example.header.protocolVersion = 4;
    spatem_example.header.stationID = STATION_UNIQUE_ID;
    //spatem_example.header._asn_ctx. -> check _asn_ctx purpose

    /*SPATEM SPAT payload*/
        /*SPAT Timestamp - OPT*/
        spatem_example.spat.timeStamp = &spat_timestamp;
        /*SPAT Name - OPT*/
        /*SPAT Intersections payload*/
            /*Intersections Name payload - OPT*/
            intersection_state_example_name.buf = "Inter1a2b3c"; //0x49 0x6e 0x74 0x65 0x72 0x31
            intersection_state_example_name.size = (strlen(intersection_state_example_name.buf));
        intersection_state_example.name = &intersection_state_example_name;

            /*Intersections ID payload*/
            intersection_id.id = 2;
        intersection_state_example.id = intersection_id;

            /*Intersections Revision payload*/
        intersection_state_example.revision = 1;

            /*Intersections Status payload*/
            intersection_status_example.bits_unused = 4;
            intersection_status_example.buf = &inter_status_buf;
            intersection_status_example.size = 1;
        intersection_state_example.status = intersection_status_example;
            /*Intersections Moy (minutesofyear) payload - OPT*/
            /*Intersections Timestamp payload - OPT*/
            /*Intersections EnabledLanes - OPT*/
            /*Intersections States payload*/
            movement_state_example.signalGroup = 1;
                /*States MovementEvent payload*/
                movement_event_example.eventState = MovementPhaseState_unavailable;
                movement_event_example.regional = NULL;
            movement_state_example.state_time_speed.list.array = array_of_movement_event_pointers;
            movement_state_example.state_time_speed.list.array[0] = &movement_event_example;
            movement_state_example.state_time_speed.list.count = 1;
            movement_state_example.state_time_speed.list.size = 1;
                /*States regional payload*/
            movement_state_example.regional = NULL;
        intersection_state_example.states.list.array = array_of_movement_state_pointers;
        intersection_state_example.states.list.array[0] = &movement_state_example;
        intersection_state_example.states.list.count = 1;
        intersection_state_example.states.list.size = 1;
            /*Intersections ManeuverAssistList payload - OPT*/
            /*Intersections Regional payload*/
        intersection_state_example.regional = NULL;

    spatem_example.spat.intersections.list.array = array_of_intersection_state_pointers;
    spatem_example.spat.intersections.list.array[0] = &intersection_state_example;
    spatem_example.spat.intersections.list.count = 1;
    spatem_example.spat.intersections.list.size = 1;
    spatem_example.spat.regional = NULL;


    printf("Checking for constraints Intersection State List...\n");
    ret = asn_check_constraints(&asn_DEF_IntersectionStateList, &spatem_example.spat.intersections.list, out_buffer, (size_t*)&index);

    if(ret)
    {
        printf("Error checking constraints: %s\n", out_buffer);
        return -1;
    }

    printf("Checking for constraints SPATEM...\n");
    ret = asn_check_constraints(&asn_DEF_SPATEM, &spatem_example, out_buffer, (size_t*)&index);

    if(ret)
    {
        printf("Error checking constraints: %s\n", out_buffer);
        return -1;
    }

    memset(out_buffer, 0, sizeof(out_buffer));

    printf("ITS PDU Header: protocol Version: %ld, messageID: %ld, stationID: %ld\n",
            spatem_example.header.protocolVersion, spatem_example.header.messageID,
            spatem_example.header.stationID);

    if(spatem_example.spat.intersections.list.array)
    {
        printf("SPAT payload: timeStamp: %ld, intersection ID: %ld, intersection Name: %*s\n",
            *(spatem_example.spat.timeStamp), spatem_example.spat.intersections.list.array[0]->id.id,
            (int)spatem_example.spat.intersections.list.array[0]->name->size, 
            spatem_example.spat.intersections.list.array[0]->name->buf);
    }

    printf("\nPreparing to encode the message...\n");

    ec = uper_encode_to_buffer(&asn_DEF_SPATEM, NULL, &spatem_example, out_buffer, 1024);

    bytes_enc = (ec.encoded + 7) / 8;

    if (ec.encoded > 0)
    {
        index = 0;
        printf("Bits enconded: %ld\n", ec.encoded);
        while (index != bytes_enc)
        {
            printf("0x%02x ", out_buffer[index]);
            index++;
        }
        printf("\n");
    }
    else
    {
        printf("Error while encoding: %s\n", ec.failed_type ? ec.failed_type->name : "unknown");
    }

    int count = 0;
    while(count++ < 30) {
        
        sendMessage(sockfd, &broadcast_addr, out_buffer, bytes_enc);

        sleep(1);
    }

    close(sockfd);

    return 0;
}


int createSocket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Habilitando a capacidade de enviar broadcast
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        perror("setsockopt (SO_BROADCAST) failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void sendMessage(int sockfd, struct sockaddr_in *broadcast_addr, char *buffer, unsigned int bytes_enc) {
    if (sendto(sockfd, buffer, bytes_enc, 0, (struct sockaddr *)broadcast_addr, sizeof(*broadcast_addr)) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
}

void configureBroadcast(struct sockaddr_in *broadcast_addr) {
    memset(broadcast_addr, 0, sizeof(*broadcast_addr));
    broadcast_addr->sin_family = AF_INET;
    broadcast_addr->sin_port = htons(PORT);
    //broadcast_addr->sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadcast_addr->sin_addr.s_addr = inet_addr(BROADCAST_IP);

}