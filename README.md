# B.Sc. Thesis Binary Translation Programming Assignment

## Installation

You can compile both programs by executing:
```bash
$ make
```
You will get two binaries: *server* and *client*.

## Usage

### Server

1. Execute the server:
```bash
$ ./server <number-of-buckets>
```

### Client

2. Execute the client:
```bash
$ ./client
```
You interact with this program through text-based prompts.

At first, you will be asked if you want to do an (i)nsert, a (g)et, a (r)emove or a (p)rint on the hashtable.

Depending on the operation, you can be asked to give a key and/or a value. 

The hashtable is implemented with <int, int>, invalid arguments will be catched.

## Assignment

### General instructions

- Languages: C/C++
- Initiate a github repository for each task
- Provide the code accompanied with a Makefile to compile it as well as instructions on how to run your applications

### Server/client application

Task description: Implement the following two programs:

#### Server:

- Initializes a hash table of given size (provided via the command line)
- Supports the insertion of items in the hash table
- Hash table collisions are resolved by maintaining a linked list for each bucket/entry in the hash table
- Supports concurrent operations (multithreading) to perform insert, get and delete operations on the hash table
- Use readers-writer lock to ensure safety of concurrent operations; try to optimize the granularity
- Communicates with the client program using shared memory buffer (POSIX shm)

#### Client:

- Enqueues requests/operations (insert, read, delete) to the server (that will operate on the the hash table) via a shared memory buffer (POSIX shm)
