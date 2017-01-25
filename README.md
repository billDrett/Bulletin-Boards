<h1/>Bulletin Boards </h1>
<h2/>Introduction </h2>
A multi-process application with the use of named pipes and Low-Level I/O. Clients communicate with a server with named pipes and gives commands to write text, send files (etc images), create channels while others can read and download them. Also a bash script to monitors which processes of Bulletin Boards app are running and which have stoped. 
<h2/>Compile</h2>
 Make command to compile and make clean to remove all object files. 

<h2/>Run	</h2></h2>

To run the server </br>
./board path</br>
For example:</br>
./board ./myboard</br>

To run the client </br>
./boardPost ./path</br>
For example:</br>
./boardPost ./myboard</br>

<h2/>Functionality</h2>
<h4>Board</h4>
By running the board process a new board is created(or connected if it aldready exists) in the path which was given as an attribute. A board can have a lot of channels for 
receiving files(the files are send throught named pipes).
The files are then saved to the disk and the messages are printed to the stdin.
Commands</br>
<lu>
<li>createchannel id name </li>
Creates a new chanel to the board with id being the identifier and the name of the channel.
<li>getmessages <id> </li>
Prints the messages of a channel and the files are saved to the disk, if the file already exists it add an extension at the name of the file.
<li>exit</li>
closes the client while the server is still running(I use fork to create a server and a client)
<li>shutdown</li>
Terminates the server
</lu>
<h4>BoardPost</h4>
By running the boardPost process the user is connected with a board server and can send files and messages throught the named pipes.
Commands</br>
<lu>
<li>list </li>
Prints all the available channels
<li>write id message </li>
Send a message at the channel with id
<li>send id file </li>
Sends a file to the channel with id 
</lu>
