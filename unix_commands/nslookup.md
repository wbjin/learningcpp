Query local DNS server

`nslookup hostname` to get back the ip address of the hostname.
`nslookup hostname nameserver` to query a specific nameserver.

Even cooler, do
```
tcpdump -nnttttX 'dst <ip of nameserver>'
```
and when you do `nslookup` you'll be able to see the actual bits that are sent
with the request.
