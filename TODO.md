# V1 Brief

Build a minimal HTTP/1.1 server in pure C++ that only serves static files for `GET` requests and closes the connection immediately after each response.

So for v1: one request per TCP connection, one fixed static root, regular files only, minimal request parsing, basic path safety, basic status codes, simple content-type handling, basic logging, Linux only, simplest execution model.



High Level Execution Flow: 
1. Server starts DONE
- create a listening socket
- bind it to an IP/port
- mark it as listening
- now the kernel queues incoming connection attempts for you
2. Browser/client connects DONE
- client opens a TCP connection to your port
- your server accepts one connection
- after accept, you get a new connected socket just for that client
- the listening socket stays open for future clients
3. Server reads the request
- client sends bytes like an HTTP request line and headers
- you read bytes from the connected socket into memory
- for V1, you only need enough data to parse one simple GET request
4. Server parses meaning from bytes
- first line tells you method, path, version
- headers follow
- blank line ends the header section
- for V1, the main thing you care about is basically: “is this a valid GET /something HTTP/1.1 request?”
5. Server maps path to disk
- request path is not a file path yet
- you translate it into a file under your chosen static root
- you also reject unsafe paths that try to escape that root
6. Server builds response
- if file exists and is allowed: send status line, headers, blank line, then file bytes
- if not: send error response like 404 or 400
- HTTP response is just bytes too; the browser only understands it because both sides agree on the format
7. Server closes connection
- since V1 is “one request, then close,” you do not keep the socket around
- client sees end-of-response either from headers and/or connection close
- then your server goes back to waiting for the next client

# Todo

- understand on a high level what the actual workflow looks like (sockets, tcp, http headers etc) DONE
- Accept TCP connections and read one HTTP request per connection.
  - First open a listening socket, bind it to a port and accept connection from client DONE
  - read one HTTP Request from that client socket DONE
- Parse just enough of HTTP/1.1 to handle valid `GET` requests. NEXT
  - figure out what to do with the read bytes now NEXT
- Map request paths to files inside a chosen static root.
- Return the file contents with a valid HTTP response, then close the connection.
- Handle a few basic error cases cleanly.
