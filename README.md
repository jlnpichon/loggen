loggen is a tool that produce random log lines. I use it to test and feed logstash/elasticsearch/kibana architecture.

The output looks like:
```
{"action": "like", "login": "georgine", "gender": "w", "ip": "240.106.137.200", "timestamp": "1400682579"}
{"action": "dislike", "login": "abigail", "gender": "w", "ip": "60.2.75.243", "timestamp": "1399668729"}
{"action": "dislike", "login": "rodger", "gender": "m", "ip": "52.172.224.34", "timestamp": "1400599276"}
{"action": "comment", "data": "Project Gutenberg's s Alice's s great t hurry.", "login": "almeda", "gender":  
 "w", "ip": "37.68.82.34", "timestamp": "1400136284"}
{"action": "like", "login": "charlott", "gender": "w", "ip": "153.15.26.53", "timestamp": "1399490615"}
```

Each line represent an action done by an fake user. Action are 'buy', 'comment', 'like', 'dislike', 'post'.
You can easily extend it by modifying the loggen.c:'action_types' array.
An action can have data attached to it, the "data" field is generated with the help of markov chains.

## Building
$ make

## Using it
./loggen
