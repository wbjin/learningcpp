## HTTP

Pages 95-115 Section 2.2

- HTTP 1.0: First version, no persistent connection. Without persistent
connections, each new HTTP request had to use a new TCP connection which meant
the TCP handshake overhead every time.
- HTTP 1.1: Persistent connection with pipelining. Persistent connection meant
multiple HTTP request/responses with one TCP connection. Pipelining allowed
multiple requests to be in flight. However, head of line blocking was still an
issue where if you requests a big object first, you would have to wait for the
big object to finish sending before you can receive the other objects. If you
wanted to multiplex, you had to open multiple TCP connections concurrently
which took away bandwidth from other applications that may need it.
- HTTP 2.0: Multiplexing over single TCP connectin with framing, request
prioritization, and server push. Fixed head of line blocking and allowed
multiplexing with framing.

HTTP Request Formatting
Request Line: method, url, version
Header Lines: header name and value eg Host: www.google.com
Blank Line
Data body

HTTP Response Formatting
Status Line: version, status code, phrase
Header Lines: header name and value eg Content-Length 200
Blank Line
Data body

HTTP 2 Framing:
Break HTTP messages into smaller frames and interleave frames instead of
sending one message at a time. For example, if you had a message that was 200
frames big and 5 smaller messages that were 2 frames each. You would take turns
sending a frame for each message so that the smaller messages don't have to
wait for the big message.

HTTP 2 Message prioritization and server push:
HTTP 2 allows you to customize the relative priority of a request. You can give
a request a weight between 1 and 256 where a higher number gets more prioritiy
in terms of frames. You can also specify message dependencies so that you can
get the messages that you need first.

Server push allows servers to send content that a requester didn't ask for. If
you want to load a web page that contains 10 objects, when you first request
for the HTML of the page, the server can give you the HTML as well as the other
objects needed to load the page.

HTTP 3:
Implement QUIC, a reliable transport protocol running at the application layer
over UDP. HTTP 3 builds on top of this QUIC protocol.
