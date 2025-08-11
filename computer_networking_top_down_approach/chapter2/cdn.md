##  CDN

Pages 143-152 Section 2.6

HTTP Streaming:

Client requests for video with a HTTP GET request and server sends video as
fast as it can. Client receives it in a buffer and once you have enough video
feed in your buffer, the video starts playing. The client continues to receive
the video from the server while displaying the video. However, this has
shortcomings as all clients receive the same encoding of the video regardless
of the bandwidth availability at the time.

Dynamic Adaptive Streaming over HTTP:

The video is encoded in vafrious qualities in numerous chunks. The client
requests for specific chunks of specific encodings depending on the bitrate.
The HTTP server contains a manifest file that provides the URLS for the various
versions along with their bitrates. The client, or any middle proxy can
dynamically adjust the bitrate.

Content Distribution Networks:

Manage geographically distributed servers that contain copies of static web
content like videos, photos, JS bundles. CDNs have two different server
placement strategies
- Enter Deep: Deploy server clusters in access ISPs all over the world. This
lets CDN get as close to end user. Akamai uses this.
- Bring Home: Build large clusters at smaller numbers and place them in IXPs.
CDNs often employ a pull strategy. If a user requests an object but the server
doesn't have it, it will request from a central source and respond to the user
with what the central source gave back to it while also storing it locally.

CDNs have to intercept requests from the client and redirect the request to the
correct cluster. For example, when a user queries for a video on a page, the
user will send a request to its local DNS for the IP of the video. The local
DNS will query the site's authoritative DNS which will see that the requested
content is stored in an external CDN. The authoritative CDN will return the
hostname (instead of IP) of the content in the external DNS's domain. The local
DNS will then query for the IP address of the new hostname to get the IP
address of the content in the CDN's domain.

CDNs dynamically direct clients to a server cluster or a data center within the
CDN. When a client's local DNS does a query for the content's IP, the CDN's DNS
knows the IP of the local DNS. From this, it has to employ a cluster selection
strategy to distribute load across servers. The strategy can be a simple
geographically closest selection or it can be more complicated based on
real-time measurements of delay and loss performances between clusters and
clients.
