# EE 250L Fall 2026 - Lab 2 Responses

**Name:** Chimdinma Nwamgbe
**USC username:** nwamgbe@usc.edu

Answer the questions in your own words. Add screenshots to the PDF submission as described in the lab handout.

## Part 1: Evaluating TCP and UDP

### 1. UDP under packet loss
How did introducing 50% packet loss affect the UDP messages you observed? Explain why.

**Answer:** The 50% packet loss caused the messages from the client to be sent randomly or not at all. Since UDP is less reliable than TCP, it doesn’t guarantee that the data will be received in the same order it was sent. UDP doesn’t resend a client’s message if there is a connection error. 

### 2. TCP under packet loss
How did introducing 50% packet loss affect the TCP exchange? Did the application data disappear, arrive later, or behave differently? Explain why.

**Answer:** The application data arrived much later, but it all arrived in the same order it was typed in. TCP is more reliable than UDP, and when sending messages, it ensures that even if there is a delay, messages are still received in the order they were sent. TCP will resend a client’s message until it is received by the server. 

### 3. Why localhost is useful
Why is `localhost` useful for testing applications even though no packets need to traverse the physical Wi-Fi network?

**Answer:** Localhost is useful for testing applications as it allows us to test how connections work without having to connect to a separate device or network. Everything can stay locally on the system. 

## Part 2: Python TCP Client and Server

### 1. Client/server socket roles
Which side calls `listen()` / `accept()`, and which side calls `connect()`? Why?

**Answer:** The client calls connect() because it needs to be able to connect to the server IP address and port so it knows where to send messages/information to. The server calls listen() and accept() so it will be able to know when the client is sending messages and receive them. 

### 2. Information needed to connect
What two pieces of information does the client need in order to connect to the server?

**Answer:** The client needs the IP address of the server and the port number the server is on. 

### 3. Loopback versus VM network address
What is the difference between `127.0.0.1` and the VM IPv4 address you found with `hostname -I` or `ip addr`?

**Answer:** The 127.0.0.1 address is the localhost IP address for the VM. This one does not change and is the default that is assigned to the VM. It’s used for internal communication within the VM. The VP IPv4 address is unique to the VM and can change depending on what network the VM is connected to. It allows the VM to communicate with external devices on a network. 

### 4. Bidirectional communication
The same TCP connection can carry data in both directions. Identify where this happens in the client/server exchange.

**Answer:** This data transfer happens when the client and server both connect. The client sends the server a packet to initiate the connection, while the server sends the client an acknowledgement packet letting the client know that it’s received the initiation packet. 

## Part 3: ESP32-S3 TCP Client to the Same Python Server

### 1. Cross-platform interoperability
Did `tcp_server.py` need to know that the second client was an ESP32-S3 written in C rather than a Python program? Why or why not?

**Answer:** Tcp_server.py didn’t need to know that the second client wasn’t a Python program because TCP is a standard protocol layer, meaning that it doesn't need to know what's happening at other levels, such as the programming level, to be able to share information. The TCP layer treats the ESP32-S3’s data as opaque, and sees it as a sequence of bytes. 

### 2. Why the ESP32-S3 cannot use 127.0.0.1
Why can the ESP32-S3 not use `127.0.0.1` to reach the server running in the Ubuntu VM?

**Answer:** The ESP32-S3 can’t use 127.0.0.1 to reach the Ubuntu server because that address is only local to the Ubuntu VM. Any data sent to that address can’t leave the VM. The 127.0.0.1 represents a local address, and so if the ESP32-S3 tried to connect to that address, it would be sending information back to itself. 

### 3. NAT versus incoming reachability
Why can a VM in NAT mode have Internet access yet still be difficult for the ESP32-S3 to contact directly?

**Answer:** Having the VM in NAT mode can make it difficult for the ESP32-S3 to connect because NAT acts as a private router that allows the VM to communicate with other VMs on the same network while being connected to the internet via my computer. Since this connection is private, outside devices, like the ESP32-S3, won’t be able to connect and send messages to the VM’s private IP address while in NAT. 

### 4. Role of the TCP port
What role does the TCP port number play when both the ESP32-S3 and VM already have IP addresses?

**Answer:** The TCP port number allows the communication between the devices to go through the correct application and ensures that the data doesn’t go somewhere it isn’t meant to. 

### 5. TCP versus the application protocol
What does TCP guarantee for the byte stream, and what does the application still have to define for itself?

**Answer:** TCP guarantees that the byte stream will be sent in the order it was given in, and the application must define its message boundaries and the meaning of the data. The message boundaries can include things like line breaks, which help frame a message and have it appear the way the user intends. TCP cannot do this. The meaning of the data also has to be defined by the application, specifically what kind of data is being sent; for example: text, binary, or JSON. 

## Assistance acknowledgment

List all assistance received, including AI tools, people, websites, or other resources, and briefly describe what help each provided.

**Answer:** Since most of the content in this lab was new to me (we didn’t go over TCP/UDP in class until the day after), I relied on Google searches to help me understand various concepts and how the skeleton code was meant to work. I also used the provided Python socket documentation to figure out how the commands worked and how to use them. In part 3, I ran into an issue with my ESP-IDF where it wouldn’t let me run “idf.py build” without an error. I used Gemini to help me understand where the issue was coming from and how to fix it; I had to remove the existing build directory and create a new one. I also had connection issues with my ESP32 because it wasn’t connecting to the internet. I ended up switching to using my hotspot, and the connection worked. 

