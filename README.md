# SimpleWebServer

[![Build Status](https://travis-ci.org/1412661/SimpleWebServer.svg?branch=master)](https://travis-ci.org/1412661/SimpleWebServer)

Reference:
- [Simple web server](http://blog.abhijeetr.com/2010/04/very-simple-http-server-writen-in-c.html)
- Socket server [server.c](http://www.linuxhowtos.org/data/6/server.c)

Task:
* Fix bugs (Bao)
* Add database and search functions (Akasaka).
* Show capital request on console. Return HTTP/400 when capital request not found in HTTP request. (Danh)

Tutorial:
- [Merge branch into another branch](http://stackoverflow.com/questions/5601931/best-and-safest-way-to-merge-a-git-branch-into-master)
- [Syncing a fork](https://help.github.com/articles/syncing-a-fork/)
- [Use SSH key instead of password](http://stackoverflow.com/questions/1595848/configuring-git-over-ssh-to-login-once)
- [Sổ tay Git](http://rogerdudler.github.io/git-guide/index.vi.html)

# Usage

## Building from reposity

    make

## Run program

To run the program at default port 999

    ./webserver
To run the program at a specific port, use option -p

    ./webserver -p <port>
    
## Live demo

Please take a look at [http://webserver.baonq.xyz:9999](http://webserver.baonq.xyz:9999)

Live demo will be updated and recompiled hourly from source code from GitHub
