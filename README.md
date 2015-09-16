Socket programming project for EE 450

The project was to simulate a Recursive Domain Name Server (DNS) system to implement distributed file system.
- A set of (key, value) pairs will be distributed among three servers.
- The clients will contact a designated server with a key in order to retrieve its corresponding value.
- The designated server will directly reply with the value if it is stored in its memory.
- Otherwise, it will contact other servers in a manner similar to recursive DNS.
- Once the designated server will retrieve the value, it stores a copy of the pair (key, value) in its local memory and will send back the result to the client.
- The servers communicate with each other using TCP sockets.
- The clients communicate with the designated server using Bidirectional UDP sockets.
