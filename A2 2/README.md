# README: DNS Resolver Implementation

## Overview
This Python script implements both **iterative** and **recursive** DNS resolution. The iterative approach mimics how actual DNS resolvers work, starting from root servers and following the delegation chain until an authoritative answer is found. The recursive approach relies on a pre-configured DNS resolver (like Google's 8.8.8.8) to fetch results.

## Features Implemented

### 1. Implemented NS Resolution in `extract_next_nameservers`
- The original implementation extracted NS hostnames but did not resolve them to IP addresses.
- Now, the function checks if the **additional section** contains already resolved IPs.
- If not, it performs recursive queries to resolve NS hostnames into IP addresses.
- If any additional section records are available, they are used directly to improve efficiency.

#### Code Implementation:
```python
# TODO: Resolve the extracted NS hostnames to IP addresses
# To TODO, you would have to write a similar loop as above
# Check if additional section contains resolved IPs
if response.additional:
    for rrset in response.additional:
        if rrset.rdtype == dns.rdatatype.A:  # Additional section contains IP addresses
            for rr in rrset:
                ns_ips.append(rr.to_text())  # Use already resolved IP
                print(f"Resolved {rrset.name} to {rr.to_text()}")  # Print resolved IPs
    if ns_ips:
        return ns_ips  # Return early if additional section had IPs

# Resolve NS names to IP addresses
for ns_name in ns_names:
    try:
        answer = dns.resolver.resolve(ns_name, 'A')  # Resolve the nameserver hostname to an IP
        for rdata in answer:
            ns_ips.append(rdata.to_text())  # Store the resolved IP address
            print(f"Resolved {ns_name} to {rdata.to_text()}")  # Print resolved IPs
    except Exception as e:
        print(f"[ERROR] Failed to resolve NS {ns_name}: {e}")  # Handle resolution failure
```

### 2. Implemented UDP Query Sending in `send_dns_query`
- The `send_dns_query` function was incomplete in the original code.
- The new version correctly constructs and sends a DNS query using **UDP**.
- Proper error handling is added for **timeouts** and other exceptions.

#### Code Implementation:
```python
# TODO: Send the query using UDP 
# Note that above TODO can be just a return statement with the UDP query!
response = dns.query.udp(query, server, timeout=TIMEOUT)  # Send the query using UDP
return response  # Return the response received from the DNS server
except dns.exception.Timeout:
    print(f"[ERROR] Query to {server} timed out.")
except Exception as e:
    print(f"[ERROR] Query to {server} failed: {e}")  # Print error message if query fails
    return None  # If an error occurs (timeout, unreachable server, etc.), return None
```

### 3. Enhanced Iterative Resolution Logic
- The script now transitions through different resolution stages: **ROOT → TLD → AUTHORITATIVE**.
- If no nameservers respond, the resolution process exits with an error message.

#### Code Implementation:
```python
# TODO: Move to the next resolution stage, i.e., it is either TLD, ROOT, or AUTH
if next_ns_list:
    # If query is not resolved at the ROOT server then it is queried to the TLD server.
    if stage == "ROOT":
        stage = "TLD"
    # If query is not resolved at the TLD server then it is queried to the AUTH server.
    elif stage == "TLD":
        stage = "AUTH"
else:
    break  # If no nameservers found, stop
```

### 4. Improved Recursive Resolution Using System’s DNS Resolver
- The script now fetches **NS records** and **A records** in a structured manner.
- This ensures proper domain resolution before querying authoritative servers.

#### Code Implementation:
```python
# TODO: Perform recursive resolution using the system's DNS resolver
# Notice that the next line is looping through, therefore you should have something like answer = ??
# Fetching the NS records
answer = dns.resolver.resolve(domain, "NS")  # Performing recursive resolution
for rdata in answer:
    print(f"[SUCCESS] {domain} -> {rdata}")  # Print NS record resolution

# Fetching A records (IP addresses) after NS records
answer = dns.resolver.resolve(domain, "A")  # Performing recursive resolution
for rdata in answer:
    print(f"[SUCCESS] {domain} -> {rdata}")  # Print A record resolution
```

---
## How to Run the Script

### **Prerequisites**
- Python 3.x installed
- `dnspython` library installed (if not installed, run `pip install dnspython`)

### **Usage**
```bash
python3 dnsresolver.py <iterative|recursive> <domain>
```

#### **Example Usage**
1. **Iterative Lookup**:
   ```bash
   python3 dnsresolver.py iterative example.com
   ```
   - Starts from root servers and follows delegation to find an authoritative answer.

2. **Recursive Lookup**:
   ```bash
   python3 dnsresolver.py recursive example.com
   ```
   - Uses a pre-configured resolver to fetch the result directly.

### **Expected Output**
Depending on the resolution mode, you should see logs showing queries being sent, responses being processed, and the final resolved IP address. If the lookup fails, an error message is displayed.

---
## **DNS Resolution Flowcharts**
### **Iterative Resolution Process**
```plaintext
Start
    |
    v
User requests a domain (e.g., example.com)
    |
    v
Query sent to local DNS resolver
    |
    v
Check if response is cached
    /                   \
   Yes                  No
   |                    |
   v                    v
Return cached IP    Query sent to Root DNS Server
                            |
                            v
    Root DNS responds with TLD DNS info
               |
               v
    Query sent to TLD DNS Server
               |
               v
    TLD DNS responds with Authoritative DNS info
               |
               v
    Query sent to Authoritative DNS Server
               |
               v
    Authoritative DNS responds with IP address
               |
               v
    Resolver caches & returns IP to user
               |
               v
            End
```

### **Recursive Resolution Process**
```plaintext
Start
    |
    v
User requests a domain (e.g., example.com)
    |
    v
Query sent to local DNS resolver
    |
    v
Local DNS asks Root DNS Server
    |
    v
Root DNS asks TLD DNS Server
    |
    v
TLD DNS asks Authoritative DNS Server
    |
    v
Authoritative DNS returns IP address
    |
    v
TLD DNS returns IP to Root DNS Server
    |
    v
Root DNS returns IP to Local DNS Resolver
    |
    v
Local DNS caches & returns IP to user
    |
    v
End
```

---
## Contribution 
Each member had contributed equally equally to the assignment .
