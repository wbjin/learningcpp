Query DNS server but with more information than `nslookup`

`dig [nameserver] [name] [type of record to look for]`

It returns the DNS message headers and each section.

If you want to do an inverse lookup, do

`dig -x <hostname>`
