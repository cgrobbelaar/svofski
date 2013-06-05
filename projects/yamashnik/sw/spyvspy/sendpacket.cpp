#include <stdio.h>
#include <inttypes.h>
#include "commands.h"
#include "sendpacket.h"
#include "diags.h"

uint8_t PacketUtil::ReadEscapedByte(unsigned char *p) {
    return (p [1] + (p [0] ? 128 : 0));
}

uint16_t PacketUtil::ReadEscapedWord(unsigned char *p) {
    return ((ReadEscapedByte (p) << 8) + ReadEscapedByte (p + 2));
}

void PacketSender::SendEscapedByte(uint8_t b) const {
    SendByte((b & 128) >> 7);
    SendByte(b & 127);
}

void PacketSender::SendEscapedWord(uint16_t w) const {
    SendEscapedByte(w >> 8);
    SendEscapedByte(w & 255);
}

void PacketSender::SendHeader(const uint8_t *h) const {
    for (int i = 0; i < 5; i++) {
        SendByte(h[i]);
    }
}

void PacketSender::SendEscapedBlockWithChecksum(const uint8_t *buf, int len) {
    uint16_t checksum = 0;
    
    for (int i = 0; i < len; i++) {
        SendEscapedByte(buf[i]);
        checksum += buf[i];
    }
    SendEscapedWord(checksum);
}

/*
 * SendPacket
 */
void PacketSender::SendPacket(int srcAddr, int dstAddr, int cmdType, const uint8_t *buf, uint16_t len) {
    uint8_t Header[] = { 0xf0, 0x00, 0x00, 0x01, 0x00 };
    int l;

    verbose("\nSendPacket: from %d to %d, cmd: 0x%.2x, %d bytes",
        srcAddr, dstAddr, cmdType, len);

    // Header fields, common for all packets
    Header[2] = dstAddr;
    Header[4] = srcAddr;

    switch (cmdType) {
    case PCMD_PING:
        Header[3] = PCMD_PING;
        SendHeader(Header);
        SendByte(LAST);                             // TERMINATOR
        break;

    case NET_CREATE_FILE:
        SendHeader(Header);                         // HEADER
        SendEscapedByte(NET_CREATE_FILE);           // COMMAND
        SendEscapedWord(len);                       // LENGTH: FCB (37 bytes) + H, F, A (3 bytes)
        SendEscapedBlockWithChecksum(buf, len);     // PAYLOAD + CHECHKSUM
        SendByte(LAST);                             // TERMINATOR
        break;

    case NET_CLOSE_FILE:
        SendHeader(Header);                        // HEADER
        SendEscapedByte(NET_CLOSE_FILE);           // COMMAND
        SendEscapedWord(len);                      // LENGTH: FCB (37 bytes) + H, F, A (3 bytes)
        SendEscapedBlockWithChecksum(buf, len);    // PAYLOAD + CHECHKSUM
        SendByte(LAST);                            // TERMINATOR
        break;

    case NET_MASTER_DATA:
        l = len;
        do {
            pos = 0;

            SendHeader(Header);
            SendEscapedByte((l == len) ? NET_MASTER_DATA : NET_MASTER_DATA2);

            SendEscapedWord((l > 56) ? 56 : l);
            SendEscapedBlockWithChecksum(buf + len - l, (l > 56) ? 56 : l);
            SendByte((l > 56) ? INTERMEDIATE : LAST);

            l -= 56;

            serial->waitRx();
        } while (l > 0);

        break;

    case NET_WRITE_FILE:
        SendHeader(Header);
        SendEscapedByte(NET_WRITE_FILE);
        SendEscapedWord(len);
        SendEscapedBlockWithChecksum(buf, len);
        SendByte(LAST);
        break;

    case PCMD_SNDCMD:
        SendHeader(Header);
        SendEscapedByte(PCMD_SNDCMD);
        SendEscapedWord(len);
        SendEscapedBlockWithChecksum(buf, len);
        SendByte(LAST);
        break;
    }

    // Debug
    verbose("\n");
}

void PacketSender::SendPacket(GenericPacket* packet) 
{
    SendPacket(packet->GetSrcAddr(), packet->GetDstAddr(), packet->GetCmd(),
        packet->GetData(), packet->GetLength());
}

int PacketSender::ReceivePacket() {
    pos = 0;
    return serial->waitRx();
}

void PacketSender::CheckPacket() {
    int src, dst;

    // Header
    if ((buf[0] == 0xf0) || (buf [0] == 0x78) // sorry :)
        && (buf[1] == 00))
    {
        dst = buf[2];
        src = buf[4];
        switch (buf[3]) {
            int readEscWord;

            case PCMD_BASE:
                switch (buf [6]) {
                    case RE_NET_CREATE_FILE:
                        verbose("\n *** RE_NET_CREATE_FILE from %d to %d", src, dst);
                        readEscWord = PacketUtil::ReadEscapedWord (&(buf [7]));
                        verbose("\n *** Payload: %d bytes", readEscWord);
                        GetRxData(buf);
                        break;
                    case RE_NET_CLOSE_FILE:
                        verbose("\n *** RE_NET_CLOSE_FILE from %d to %d", src, dst);
                        readEscWord = PacketUtil::ReadEscapedWord (&(buf [7]));
                        verbose("\n *** Payload: %d bytes", readEscWord);
                        GetRxData(buf);
                        break;
                    case RE_NET_WRITE_FILE:
                        verbose("\n *** RE_NET_WRITE_FILE from %d to %d", src, dst);
                        readEscWord = PacketUtil::ReadEscapedWord (&(buf [7]));
                        verbose("\n *** Payload: %d bytes", readEscWord);
                        GetRxData(buf);
                        break;
                    default:
                        verbose("\n *** Unknown BASE packet 0x%.2x.", buf [6]);
                        break;
                }
                break;
            case PCMD_PING:
                break;
            case PCMD_ACK:
                verbose("\n *** ACK from %d to %d", src, dst);
                break;
            case PCMD_PONG:
                verbose("\nPONG from %d to %d STATUS=%02x.", src, dst, buf[5]);
                break;
        }
        // printf ("\n");
    }
}

void PacketSender::GetRxData(uint8_t *p) {
    m_RxData.H = PacketUtil::ReadEscapedByte(&p[11]);
    m_RxData.F = PacketUtil::ReadEscapedByte(&p[13]);
    m_RxData.A = PacketUtil::ReadEscapedByte(&p[15]);
    verbose("\n *** H: %.2x F: %.2x A: %.2x FCB: ", m_RxData.H, m_RxData.F, m_RxData.A);
    for (int i = 0; i < 37; i++) {
        m_RxData.FCB[i] = PacketUtil::ReadEscapedByte(&buf[17 + i * 2]);
        verbose(" %.2x", m_RxData.FCB[i]);
    }
}

int PacketSender::RxHandler() {
    int n;

    int result = 0;

    pos += n = serial->read(&(buf[pos]), 1024 - pos);

    // Debug
    morbose("\n****************************************");
    morbose("\nRecvData: got %d bytes, pos=%d, last byte: 0x%.2x\n", n, pos, buf [pos]);
    for (int i = 0; i < pos; i++)
    {
        morbose("%.2x ", buf [i]);
    }
    morbose("\n****************************************\n");

    if (pos > 256) {
        pos = 0;
    }
    else {
        if ( (buf[pos - 1] == 0x83) || (buf[pos - 1] == 0x97)) {
            CheckPacket();
            result = 1;
        }
    }

    return result;
}
