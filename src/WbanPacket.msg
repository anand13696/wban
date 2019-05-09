//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

enum WbanPacketType {
    WBAN_CONN_REQ = 0;
    WBAN_CONN_ACK = 1;
    WBAN_DISC_REQ = 2;
    WBAN_DISC_ACK = 3;
    WBAN_DATA = 4;
};

struct sensorLocation {
    double x;
    double y;
    double energy;
};

packet WbanPacket {
    int srcAddress;
    int destAddress;
    int priority;
    int serverProcId;
};

packet WbanDataPacket extends WbanPacket {
    string payLoad;
};