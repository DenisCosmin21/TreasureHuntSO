Instalation
If you don't have make installed on linux
```bash
sudo apt install make
```
When you have make installed
```bash
make
```
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
