# YodaDB (previously Yotta before Darth Vader took over)

All software in this package is part of YottaDB (https://YodaDB.com - coming soon ) each
file of which identifies its copyright holders. The software is made available
to you under the terms of a license. Refer to the [LICENSE](LICENSE) file for details.

Homepage: https://github.com/aa32555/YDB

Documentation: ...coming soon

- Installing YodaDB
 ## Ubuntu Linux OR Raspbian Linux ***Must be run as root***

* Install the required packages
```sh
 apt-get update
 apt-get install
 apt-get install -y git
 apt-get install -y wget
 apt-get install -y gnupg
 apt-get install -y tzdata 
 apt-get install -y pkg-config 
 apt-get install -y lsof
 apt-get install -y procps
 apt-get install -y cmake 
 apt-get install -y clang
 apt-get install -y llvm
 apt-get install -y lld
 apt-get install -y make
 apt-get install -y gcc
 apt-get install -y git
 apt-get install -y curl
 apt-get install -y tcsh
 apt-get install -y libconfig-dev
 apt-get install -y libelf-dev
 apt-get install -y libicu-dev
 apt-get install -y libncurses-dev
 apt-get install -y libreadline-dev
 apt-get install -y binutils
 apt-get install -y ca-certificates
 apt-get install -y libicu-dev
 apt-get install -y libasound2
 apt-get install -y libnss3-dev
 apt-get install -y libgdk-pixbuf2.0-dev
 apt-get install -y libgtk-3-dev
 apt-get install -y ibxss-dev
 apt-get install -y libgconf-2-4
 apt-get install -y libatk1.0-0
 apt-get install -y libatk-bridge2.0-0
 apt-get install -y libgdk-pixbuf2.0-0
 apt-get install -y libgtk-3-0
 apt-get install -y build-essential
 apt-get install -y bison
 apt-get install -y flex
 apt-get install -y xxd
 apt-get install -y libreadline-dev
 apt-get install -y libssl-dev
 apt-get install -y libconfig-dev
 apt-get install -y libcmocka-dev
 apt-get install -y default-jdk
 apt-get install -yexpect
 apt-get install -y golang-go
 apt-get install curl
 apt-get install libgcrypt20-dev
 curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.0/install.sh | bash
```
The ydbinstall.sh script downloads and installs YodaDB.
Download the script in a temporary directory, and make it executable, e.g.:

```
mkdir /tmp/tmp && cd /tmp/tmp && wget https://raw.githubusercontent.com/aa32555/YDB/master/sr_unix/ydbinstall.sh && chmod +x ydbinstall.sh
./ydbinstall.sh --utf8 default --verbose --octo

/ydbinstall.sh --utf8 default --verbose --octo --from-source https://github.com/aa32555/YDB.git 

./ydbinstall.sh --utf8 default --verbose --octo --from-source https://github.com/aa32555/YDB.git --overwrite-existing 
 ```

