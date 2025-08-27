# CS425 Assignment 3 – TCP Handshake (Client Side)


## Objective

This assignment implements the **client-side of a TCP three-way handshake** using **raw sockets** in C++. The goal is to:
- Understand how TCP connection setup works under the hood.
- Manually construct, send, and receive raw packets without the OS’s TCP stack.
- Implement the *SYN → SYN-ACK → ACK* exchange manually.


## ⚙ How to Run

### 🛠 Requirements

- A *Linux system* (e.g., Ubuntu).
- *g++ compiler*.
- *Root access* (required for raw sockets).

### ✅ Steps

1. **Compile the client side code:**

   ```bash
   g++ client.cpp -o client
   ```
   
2. **Compile and run the server side code in a separate terminal:**
  
    ```bash
    g++ server.cpp -o server
    sudo ./server
    ```
   
   **You will see this in the terminal:**
    ```bash
    [+] Server listening on port 12345..
    ```

3. **Run the client the side:**

    ```bash
    sudo ./client
    ```
    
4. **You will see this in the client side:**
    ```bash
    [+] Sending SYN...
    TCP Flags: SYN:1 ACK:0 FIN:0 RST:0 PSH:0 URG:0 SEQ:200 ACK:0
    [+] Received SYN-ACK from server
    TCP Flags: SYN:1 ACK:1 FIN:0 RST:0 PSH:0 URG:0 SEQ:400 ACK:201
    [+] Sending ACK...
    TCP Flags: SYN:0 ACK:1 FIN:0 RST:0 PSH:0 URG:0 SEQ:201 ACK:401
    [+] Handshake complete!
     ```
    
5. **You will see this in the server side:**
    ```bash
    [+] Server listening on port 12345...
    [+] TCP Flags:  SYN: 1 ACK: 0 FIN: 0 RST: 0 PSH: 0 SEQ: 200
    [+] Received SYN from 127.0.0.1
    [+] Sent SYN-ACK
    [+] TCP Flags:  SYN: 0 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 201
    ```
    
## 🔍 Explanation of the Code

1. **Creates a Raw Socket:**

   ```cpp
   int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
   ```

   Creates a raw socket to manually send and receive TCP packets.

2. **Manually Constructs the SYN Packet:**

   - Custom IP and TCP headers are filled manually.
   - Sequence number is set to **200**.
   - The **SYN** flag is enabled (`syn = 1`).

3. **Sends the SYN Packet:**

   ```cpp
   sendto(sock, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));
   ```

4. **Waits for SYN-ACK from Server:**

   - The client listens for a response using `recv()`.
   - It checks whether the received packet has `SYN = 1` and `ACK = 1`.
   - It also verifies `ack_seq == 201`, meaning the server acknowledged 200 + 1.

5. **Sends Final ACK:**

   - The **ACK** flag is set (`ack = 1`).
   - `ack_seq = 401` (server’s `seq = 400`, so ACK = 400 + 1).
   - `seq = 600`.

This completes the three-way TCP handshake.

---

### 🧾 Notes & Assumptions

1. The server is assumed to be listening on `127.0.0.1:12345`.

2. The sequence numbers are based on the server’s logic:

   - **Client SYN:** `seq = 200`
   - **Server SYN-ACK:** `seq = 400`, `ack = 201`
   - **Client ACK:** `seq = 600`, `ack = 401`

3. Only the **minimum required TCP fields** are filled in the headers.


## 🧪 Sample Outputs

### ✅ Valid Sequence

```bash
[+] Sending SYN...
TCP Flags: SYN:1 ACK:0 FIN:0 RST:0 PSH:0 URG:0 SEQ:200 ACK:0
[+] Received SYN-ACK from server
TCP Flags: SYN:1 ACK:1 FIN:0 RST:0 PSH:0 URG:0 SEQ:400 ACK:201
[+] Sending ACK...
TCP Flags: SYN:0 ACK:1 FIN:0 RST:0 PSH:0 URG:0 SEQ:201 ACK:401
[+] Handshake complete!
```

### ❌ Invalid Sequence (`--bad-seq`)

```bash
[+] Sending SYN...
TCP Flags: SYN:1 ACK:0 FIN:0 RST:0 PSH:0 URG:0 SEQ:123 ACK:0
[-] Timeout: No response from server after 5 seconds
[-] TCP handshake failed: server may not be running
```


## 🔄 Detailed Flowchart of TCP Three-Way Handshake

```text
Start the Server First
┌──────────────────────────────────────────────────────────┐
│ Server starts and listens on IP: 127.0.0.1, Port: 12345  │
└──────────────────────────────────────────────────────────┘

 CLIENT (Raw Socket)                        SERVER (Given Code)
───────────────────────                    ──────────────────────

[1] Connection Init
        |                                             |
        | --- SYN Packet ---------------------------> |
        |      Flags: SYN=1                           |
        |      SEQ: 200                               |
        |                                             |
        |                                             |
[2] Server Response
        | <--- SYN-ACK Packet ----------------------  |
        |      Flags: SYN=1, ACK=1                    |
        |      SEQ: 400, ACK: 201                     |
        |                                             |
        |                                             |
[3] Final ACK from Client
        | --- ACK Packet ---------------------------> |
        |      Flags: ACK=1                           |
        |      SEQ: 600, ACK: 401                     |
        |                                             |
        |                                             |
        |       [✓ Handshake Complete]                |
        |_____________________________________________|
```

## 🔍 Testing: Conditions for Success & Failure

### ✅ Handshake Should Succeed If:

- Client sends a **SYN packet with `SEQ = 200`**
- Server sends a **SYN-ACK with `ACK = 201`**
- Client responds with **ACK (`SEQ = 201`, `ACK = 401`)**

### ❌ Handshake Should Fail If:

- Client sends an incorrect **initial SEQ (e.g., `SEQ ≠ 200`)**
- Server sends a SYN-ACK with unexpected values
- Client detects incorrect `ACK` in response and aborts handshake
- Or, client times out without receiving a response

## Contributors 

All team members have equally contributed to this assignment:

- **Yash Chauhan** (221217)
- **Ansh Agarwal** (220165)
- **Pushkar Aggarwal** (220839)