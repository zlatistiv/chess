Multi-user chess for the linux shell. Build with make.

![Screenshot from 2023-06-03 18-02-58](https://github.com/nzlatkov/chess/assets/44267903/06571245-6c0d-4c9b-98e0-86b076aed636)

Example usage:
First player hosts a game and listens for a connection
```
nikola@nameless:~/Code/chess $ ./chess -h localhost 2222
Waiting for an oponent to join, listening on: localhost:2222
```
Second player joins:
```
nikola@nameless:~/Code/chess $ ./chess -j localhost 2222
```

You might want to increase font size of your terminal to see the pieces better.
