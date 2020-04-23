Groot
<a href="https://microbadger.com/images/sivakesava/groot"><img align="right" src="https://img.shields.io/microbadger/image-size/sivakesava/groot.svg?style=flat&label=docker"></img></a>
==========
<!---
[![](https://img.shields.io/docker/cloud/build/sivakesava/groot.svg?logo=docker&style=popout&label=Docker+Image)][docker-hub]
[![](https://github.com/dns-groot/groot/workflows/Docker%20Image%20CI/badge.svg?logo=docker&style=popout&label=Docker+Image)](https://github.com/dns-groot/groot/actions?query=workflow%3A%22Docker+Image+CI%22)
-->
[![](https://img.shields.io/github/workflow/status/dns-groot/groot/Docker%20Image%20CI/master?logo=docker&style=popout&label=Docker+Image)](https://github.com/dns-groot/groot/actions?query=workflow%3A%22Docker+Image+CI%22)


Groot is a static verification tool for DNS. Groot consumes a collection of zone files along with a collection of user-defined properties and systematically checks if any input to DNS can lead to violation of the properties.

## Installation

### Using `docker` (recommended)

_**Note:** The docker image may consume  ~&hairsp;5&hairsp;GB of disk space._

We recommend running Groot within a docker container,
since they have negligible performance overhead.
(See [this report](http://domino.research.ibm.com/library/cyberdig.nsf/papers/0929052195DD819C85257D2300681E7B/$File/rc25482.pdf))

0. [Get `docker` for your OS](https://docs.docker.com/install).
1. Pull our docker image<sup>[#](#note_1)</sup>: `docker pull dnsgt/groot`.
2. Run a container over the image: `docker run -it dnsgt/groot`.<br>
   This would give you a `bash` shell within groot directory.

<a name="note_1"><sup>#</sup></a> Alternatively, you could also build the Docker image locally:

```bash
docker build -t dnsgt/groot github.com/dns-groot/groot
```
Docker containers are isolated from the host system.
Therefore, to run Groot on zones files residing on the host system,
you must first [bind mount] them while running the container:

```bash
docker run -v /host/dir:/home/groot/groot/shared -it dnsgt/groot
```

The `/host/dir` on the host system would then be accessible within the container at `~/groot/shared` (with read+write permissions). The executable would be located at `~/groot/build/bin/`.

### Manual Installation

<details>

<summary><kbd>CLICK</kbd> to reveal instructions</summary>

#### Installation for Windows
1. Install [`vcpkg`](https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=vs-2019) package manager to install dependecies. 
2. Install the C++ libraries (64 bit versions) using:
    - vcpkg install boost-serialization:x64-windows boost-flyweight:x64-windows boost-dynamic-bitset:x64-windows boost-graph:x64-windows  docopt:x64-windows nlohmann-json:x64-windows spdlog:x64-windows
    - vcpkg integrate install 
3. Clone the repository (with  `--recurse-submodules`) and open the solution (groot.sln) using Visual studio. Set the platform to x64 and mode to Release.
4. Configure the project properties to use ISO C++17 Standard (std:c++17) for C++ language standard.
5. Build the project using visual studio to generate the executable. The executable would be located at `~\groot\x64\Release\`.

#### Installation for Ubuntu 18.04 or later
1. Follow the instructions mentioned in the `DockerFile` to natively install in Ubuntu 18.04 or later.
2. The executable would be located at `~/groot/build/bin/`.

</details>

## Property Verification
Check for any violations of the input properties by invoking Groot as:

For docker (Ubuntu):
```bash
$ .~/groot/build/bin/groot ~/groot/demo/zone_files --jobs=~/groot/demo/jobs.json --output=output.json
```
For Windows:
```bash
$ .~\groot\x64\Release\groot.exe ~\groot\demo\zone_files --jobs=~\groot\demo\jobs.json --output=output.json
```
Groot outputs any violations to the `output.json` file. 

### Logging
Groot by default logs debugging messages to `log.txt` file and you may use `-v` flag to log more detailed information.

### Packaging zone files data
Groot expects all the required zone files to be available in the input directory along with a special file `metadata.json`. The `metadata.json` file has to be created by the user and has to list the file name and the name server from which that zone file was obtained. If the zone files for a domain are obtained from multiple name servers, make sure to give the files a distinct name and fill the metadata accordingly. The user also has to provide the root (top) name servers for his domain in the `metadata.json`. 

<details>

<summary><kbd>CLICK</kbd> to reveal an <a href="https://github.com/dns-groot/groot/blob/master/demo/zone_files/metadata.json">example<code>metadata.json</code></a></summary>

```json5
{  
  "TopNameServers" : ["ns1.tld.sy."],  //List of top name servers as strings
  "ZoneFiles" : [
      {
         "FileName": "net.sy.txt", //net.sy. zone file from ns1.tld.sy. name server
         "NameServer": "ns1.tld.sy."
      },
      {
         "FileName": "mtn.net.sy.txt", //mtn.net.sy. zone file from ns1.mtn.net.sy. name server
         "NameServer": "ns1.mtn.net.sy."
      },
      {
         "FileName": "child.mtn.net.sy.txt", //child.mtn.net.sy. zone file from ns1.child.mtn.net.sy. name server
         "NameServer": "ns1.child.mtn.net.sy."
      },
      {
         "FileName": "child.mtn.net.sy-2.txt", //child.mtn.net.sy. zone file from ns2.child.mtn.net.sy. name server 
         "NameServer": "ns2.child.mtn.net.sy." //for same domain (child.mtn.net.sy.) as the last one but from different name server
      }
  ]
}
```
</details>

### Inputting Jobs
Groot can currently verify properties shown below on the zone files and expects the input list in a `json` file format. A **job** verifies properties on a domain and optionally on all its subdomains. The input `json` file can have a list of jobs.

<details>
<summary><kbd>CLICK</kbd> to reveal an <a href="https://github.com/dns-groot/groot/blob/master/demo/jobs.json">example job</a></summary>

```json5
{
   "Domain": "mtn.net.sy." // Name of the domain to check
   "SubDomain": true, //Whether to check the properties on all the subdomains also
   "Properties":[ 
      {
         "PropertyName": "QueryRewrite",
         "Value": ["foo.mtn.net.sy.", "bar.mtn.net.sy"]
      },
      {
         "PropertyName": "Rewrites",
         "Value": 0
      },
      {
         "PropertyName": "RewriteBlackholing"
      }
   ]
}
```
</details>

#### Available Properties
<details>
<summary>Delegation Consistency</summary>
   
The parent and child zone files should have the same set of _NS_ and glue _A_ records for delegation.
Input `json` format:
```json5
      {
         "PropertyName": "DelegationConsistency"
      }
```
</details>

<details>
<summary>Finding all aliases</summary>
Lists all the input query names (aliases) that are eventually rewritten to one of the canonical names.   

Input `json` format:
```json5
      {
         "PropertyName": "AllAliases",
         "Value": ["baz.mtn.net.sy"] //List of canonical names
      }
```
</details>

<details>
<summary>Lame Delegation</summary>
   
A name server that is authoritative for a zone should provide authoritative answers, otherwise it is a lame delegation.
Input `json` format:
```json5
      {
         "PropertyName": "LameDelegation"
      }
```
</details>

<details>
<summary>Nameserver Contact</summary>
   
The query should not contact any name server that is not a subdomain of the allowed set of domains for any execution in the DNS.
Input `json` format:
```json5
      {
         "PropertyName": "NameserverContact",
         "Value": ["tld.sy."] //List of allowed domains
      }
```
</details>

<details>
<summary>Number of Hops</summary>
   
The query should not go through more than _X_ number of hops for any execution in the DNS.
Input `json` format:
```json5
      {
         "PropertyName": "Hops",
         "Value": 2
      }
```
</details>

<details>
<summary>Number of Rewrites</summary>
   
The query should not be rewritten more than _X_ number of time for any execution in the DNS.
Input `json` format:
```json5
      {
         "PropertyName": "Rewrites",
         "Value": 3
      }
```
</details>

<details>
<summary>Query Rewritting</summary>
   
The query should not be rewritten to any domain that is not a subdomain of the allowed set of domains for any execution in the DNS.
Input `json` format:
```json5
      {
         "PropertyName": "QueryRewrite",
         "Value": ["foo.mtn.net.sy.", "bar.mtn.net.sy"] //List of allowed domains
      }
```
</details>

<details>
<summary>Response Consistency</summary>
Different executions in DNS that might happen due to multiple name servers should result in the same answers.
   
Input `json` format:
```json5
      {
         "PropertyName": "ResponseConsistency"
         "Types": ["A", "MX"] //Checks the consistency for only these types
      }
```
</details>

<details>
<summary>Response Returned</summary>
Different executions in DNS that might happen due to multiple name servers should result in some non-empty response.
   
Input `json` format:
```json5
      {
         "PropertyName": "ResponseReturned"
         "Types": ["CNAME", "A"] //Checks that some non-empty response is returned for these types
      }
```
</details>

<details>
<summary>Response Value</summary>
Every execution in DNS should return an answer that matches the user input answer.

Input `json` format:
```json5
      {
         "PropertyName": "ResponseValue"
         "Types": ["A"],
         "Value": ["2.2.2.1"] //The expected response
         
      }
```
</details>

<details>
<summary>Rewrite Blackholing</summary>
   
If the query is rewritten for any execution in the DNS, then the new query's domain name should have at least one resource record.

Input `json` format:
```json5
      {
         "PropertyName": "RewriteBlackholing"
      }
```
</details>

<!-- Groot, by default, checks for cyclic zone dependency and other loops while verifying any of the above properties.  -->

[docker-hub]:         https://hub.docker.com/r/sivakesava/groot
[bind mount]:         https://docs.docker.com/storage/bind-mounts
