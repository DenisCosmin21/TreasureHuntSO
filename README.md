Instalation
If you don't have make installed on linux
```bash
sudo apt install make
```
When you have make installed
```bash
make
```

<h1>Treasure manager</h1>

Inside the folder and afterwards
```bash
./treasure_manager <command> <parrams>
```

Usage :
Add treasure :
```bash
./treasure_manager --add treasureId
```
It creates a hunt and adds the treasure in the hunt if treasure doesn't exist
List hunt :
```bash
./treasure_manager --list huntId
```
If no hunt with specified id found, it exits with code -1
View treasure :
```bash
./treasure_manager --view huntId treasureId
```
If no hunt withb specified Id, or no treausre with specified id found it exists with code -1
Remove treasure :
```bash
./treausre_manager --remove_treasure huntId treasureId
```
If no treasure with specified id, or hunt with specified id found it exists with code -1
```bash
./treasure_manager --remove_hunt huntId
```
If no hunt with specified id found it exits with code -1

Exit codes :
File problem : exit code 1
Directory problem : exit code 2
User input problem : exit code -1 (bad ids, bad data format for fields, bad command, etc...)

<h1>Treasure hub</h1>

Inside the folder and afterwards run the hub
```bash
./treasure_hub
```

Usage :
```bash
start_monitor
```
to start monitor when hub opened and monitor not opened yet
```bash
list_hunts
```
to list all hunts when monitor started
```bash
list_treasures huntId
```
to list all treasure from hunt with id huntId
```bash
view_treasure huntId treasureId
```
to list the treasure with id treasureId from hunt with id huntId
```bash
calculate_sore
```
to show scores of all users from hunts. It will not combine scores of the same user from different hunts
```bash
stop_monitor
```
to stop the monitor if it is started
```bash
exit
```
to exit the hub when the monitor is closed. If it is opened it will give a error message

Exit codes :
Any problem with signals : exit code 3